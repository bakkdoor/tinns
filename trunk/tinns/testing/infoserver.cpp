/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community
	maintainer Akiko <akiko@gmx.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.
*/



/*
	infoserver.cpp - this is the main part of the information server

	MODIFIED: 26 Sep 2005 Akiko
	REASON:	- replaced port by a define
		- added GPL
		- added this infomartion part
*/

#include "tinns.h"

struct PInfoState
{
	enum State
	{
		IS_UNKNOWN,
		IS_CONNECTED,
		IS_HANDSHAKE0,
		IS_AUTHENTICATE,
		IS_SERVERLIST
	} mState;
	
	bool mWaitSend;	// wait-for-completition flag
	PInfoState()
	{
		mState = IS_UNKNOWN;
		mWaitSend = false;
	};
};

// ------------------------------------

PInfoServer::PInfoServer()
{
	mListener = NL_INVALID;
	mNumClients = 0;
}

PInfoServer::~PInfoServer()
{
	if(mListener != NL_INVALID)
	{
		nlClose(mListener);
		mListener=NL_INVALID;
	}
	
	for(InfoStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end(); i++)
		delete i->second;
}

void PInfoServer::Start()
{
	u16 Port = Config->GetOptionInt("infoserver port");
	if(Port == 0)
		Port = INFO_PORT;
	Console->Print("Starting infoserver on port %i...", Port);
	mListener = nlOpen(Port, NL_TCP);
	if(mListener != NL_INVALID)
	{
		nlListen(mListener);
	} else
	{
		int err = nlGetError();
		if(err==NL_SYSTEM_ERROR)
			Console->Print("Infoserver error: %s", nlGetSystemErrorStr(nlGetSystemError()));
		else
			Console->Print("Infoserver error: %s", nlGetErrorStr(err));
	}
}

void PInfoServer::Update()
{
	if(mListener==NL_INVALID)
		return;

	NLsocket temp = nlAcceptConnection(mListener);		
	if(temp != NL_INVALID)
	{
		int clid = Server->NewClient();
		if(clid!=-1)
		{
			Console->Print("Infoserver: client %i connected", clid);
			PClient *Client = Server->GetClient(clid);
			Client->InfoConnect(temp);
			Console->Print("Client address: %s", Client->GetAddress());
			++mNumClients;
			
			PInfoState *state = new PInfoState();
			ClientStates.insert(std::make_pair(Client, state));
			state->mState = PInfoState::IS_CONNECTED;
		} else
		{
			Console->Print("Infoserver: Client connection refused (server full?)");
		}
	}
	
	for(InfoStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end();)
	{
		PClient *Client = i->first;
		PInfoState *State = i->second;
		// node gets erased in FinalizeClient, increment iterator now
		++i;
		if(!ProcessClient(Client, State))
			FinalizeClient(Client, State);
	}	
}

void PInfoServer::ClientDisconnected(PClient *Client)
{
	InfoStateMap::iterator node = ClientStates.find(Client);
	if(node == ClientStates.end())
		return;

	PInfoState *State = node->second;	
	FinalizeClient(Client, State);
}

bool PInfoServer::HandleHandshake(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize)
{
	//static const u8 HANDSHAKE1A[6]={0xfe, 0x03, 0x00, 0x80, 0x03, 0x68};

	switch(State->mState)
	{
		case PInfoState::IS_HANDSHAKE0 :
		{
			if(PacketSize==6 && *(u16*)&Packet[3]==0x0080 && Packet[5]==0x78)
			{
				//FIXME: this packet seems to be unnecessary, although it appears in traffic dumps
				// (causes clientside "Wrong protocol" errors)				
				//Socket->Write(HANDSHAKE1A, 6);
				State->mState = PInfoState::IS_AUTHENTICATE;
			}
			else
			{
				Console->Print("Infoserver protocol error (IS_HANDSHAKE0): invalid packet [%04x]", *(u16*)&Packet[3]);
				return false;
			}
			break;
		}
		default:
			break;		
	}
	
	return true;
}

bool PInfoServer::HandleAuthenticate(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize)
{
	PGameSocket *Socket = Client->GetInfoSocket();
	if(PacketSize > 20 && *(u16*)&Packet[3]==0x8084)
	{
		const u8 *Key = &Packet[5];			// password key
		u16 ULen = *(u16*)&Packet[16];		// username length
		u16 PLen = *(u16*)&Packet[18];		// password length
		char *UserID = (char*)&Packet[20];	// account name
		const u8 *PW = &Packet[20+ULen];	// encoded password
		PAccount *Account = Database->GetAccounts()->Authenticate(UserID, PW, PLen, Key, true);
		bool Failed = false;
		if(Account)
		{
			Console->Print("Infoserver: User '%s' logged in", UserID);
			if(Account->GetLevel() == PAL_BANNED)
			{
				Console->Print("User %s is banned, connection refused", UserID);
				// TODO: ban ip for an adjustable time span?
				Failed = true; // player is banned
			}
			
			if(Account->GetLevel() == PAL_PLAYER)
			{
				if(Server->GetNumClients() > Server->GetMaxClients())
				{
					Console->Print("Server full, refusing connection from user '%s'", UserID);
					Failed = true;	// server full
				}
			}
		} else
		{
			Console->Print("Infoserver: User '%s': authentication failed", UserID);
			Failed = true;	// auth failed
		}
		
		if(Failed)
		{
			u8 AUTHFAILED[15]={0xfe, 0x0c, 0x00, 0x83, 0x86, 0x05, 0x00, 0x06, 0x00, 'E', 'R', 'R', 'O', 'R', 0};
			// TODO: send actual reason instead of ERROR
			Socket->Write(AUTHFAILED, 15);
			FinalizeClientDelayed(Client, State);
			State->mState=PInfoState::IS_UNKNOWN;
		} else
		{
			Client->LoggedIn(Account);
			u8 AUTHOK[28]={0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
							0x00, 0x00, 0x00, 0x00, 0x00 };
			*(u32*)&AUTHOK[5]=Account->GetID();
			Socket->Write(AUTHOK, 28);
			State->mState = PInfoState::IS_SERVERLIST;
		}
	} else
	{
		Console->Print("Infoserver protocol error (IS_AUTHENTICATE): invalid packet [%04x]", *(u16*)&Packet[3]);
		return false;
	}
	
	return true;
}

bool PInfoServer::HandleServerList(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize)
{
	static u8 SERVERLIST[]={0xfe, 0x00, 0x00, 0x83, 0x83, 0x01, 0x00, 0x0d, 0x00, 
		0x00, 0x00, 0x00, 0x00,	// ip
		0x00, 0x00, // port
		0x00, 0x00, // unknown
		0x00, // name len
		0x00, 0x00, // # players
		0x05, 0x00, // status
		};
	PGameSocket *Socket = Client->GetInfoSocket();
	PAccount *Account = Client->GetAccount();
	if(!Account)
	{
		Console->Print("Infoserver: unknown client %i requested server list", Client->GetIndex());
		return false;
	}
		
	if(PacketSize == 31 && *(u16*)&Packet[3]==0x8284 && *(u32*)&Packet[5]==Account->GetID())
	{
		const std::string &ServerName = Config->GetOption("server name");
		u16 len = sizeof(SERVERLIST)+ServerName.length()+1;
		*(u16*)&SERVERLIST[1]=len-3;
		SERVERLIST[17]=(u8)len+1;
		
		u32 dip = IPStringToDWord(Socket->GetLocalAddress());
		u16 Port = Config->GetOptionInt("gameserver port");		
		if(dip==0)
			dip=0x0100007f;
		if(Port==0)
			Port=12000;
		
		*(u32*)&SERVERLIST[9]=dip;
		*(u32*)&SERVERLIST[13]=Port;
		
		// TODO: use gameserver # clients
		*(u16*)&SERVERLIST[18]=Server->GetNumClients()-1;
		
		Socket->Write(SERVERLIST, sizeof(SERVERLIST));
		Socket->Write(ServerName.c_str());
		Socket->Write((u8)0);
	} else
	{
		Console->Print("Infoserver protocol error (IS_SERVERLIST): invalid packet [%04x]", *(u16*)&Packet[3]);
		return false;
	}
	
	return true;
}

bool PInfoServer::ProcessClient(PClient *Client, PInfoState *State)
{
	static const u8 HANDSHAKE0A[6]={0xfe, 0x03, 0x00, 0x80, 0x01, 0x66};
	
	if(!State)
	{
		InfoStateMap::iterator node = ClientStates.find(Client);
		if(node == ClientStates.end())
			return false;

		State = node->second;
	}

	PGameSocket *Socket = Client->GetInfoSocket();

	if(State->mWaitSend && Socket->GetSendBufferSize()==0)
		return false;
	
	if(State->mState==PInfoState::IS_CONNECTED)
	{
		Socket->Write(HANDSHAKE0A, 6);
		State->mState = PInfoState::IS_HANDSHAKE0;
	}
	
	int PacketSize=0;
	const u8 *Packet = Socket->Read(&PacketSize);
	if(PacketSize > 0)
	{
		switch(State->mState)
		{		
			case PInfoState::IS_HANDSHAKE0:
				return HandleHandshake(Client, State, Packet, PacketSize);
						
			case PInfoState::IS_AUTHENTICATE:
				return HandleAuthenticate(Client, State, Packet, PacketSize);			
			
			case PInfoState::IS_SERVERLIST:
				return HandleServerList(Client, State, Packet, PacketSize);
			default:
				break;
		}
	}	
	return true;
}

void PInfoServer::FinalizeClient(PClient *Client, PInfoState *State)
{
	Console->Print("Infoserver: client %i disconnected", Client->GetIndex());
	Client->InfoDisconnect();
	ClientStates.erase(Client);
	delete State;
	--mNumClients;
}

// completes pending packets before disconnect
void PInfoServer::FinalizeClientDelayed(PClient *Client, PInfoState *State)
{
	Console->Print("Infoserver: client %i is about to be disconnected", Client->GetIndex());
	State->mWaitSend = true;
}

