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

	gameserver.cpp - main file of the gameserver

	MODIFIED: 31 Aug 2005 Akiko
	REASON: - added modifications by "Sting" to get a running gameserver again
		- added this header
	MODIFIED: 28 Sep 2005 Akiko
	REASON:	- define for game server port
	MODIFIED: 29 Sep 2005 Sting (modified by Akiko)
	REASON: - configurable UDP port for testing
		- tidied up/corrected packet sent with most character stats (character now has monk stats for testing)

*/


#include "tinns.h"

struct PGameState
{
	struct TCP
	{
		enum State
		{
			GS_UNKNOWN,
			GS_CONNECTED,
			GS_HANDSHAKE0,
			GS_AUTHENTICATE,
			GS_GAMEDATA,
			GS_REQUESTCHARS,
			GS_CHARLIST,
			GS_GETSTATUS,
			GS_GAMEINFO,
			GS_INGAME
		} mState;
		
		bool mWaitSend;	// wait-for-completition flag
	} TCP;
	
	struct UDP
	{
		enum State
		{
			GUS_UNKNOWN,
			GUS_SYNC0,
			GUS_SYNC1,
			GUS_SYNC2,
			GUS_SYNC3
		} mState;
		
		bool mSynced;
		u16 mServerPacketNum;
		u16 mSequence;
		//u16 mClientPacketNum;
	} UDP;
	
	PGameState()
	{
		TCP.mState = TCP::GS_UNKNOWN;
		TCP.mWaitSend = false;
		
		UDP.mState = UDP::GUS_UNKNOWN;
		UDP.mSynced = false;
		UDP.mServerPacketNum = 0x9c9f;
		UDP.mSequence = 2;
		//UDP.mClientPacketNum = 0;
	};
};

// ------------------------------------

PGameServer::PGameServer()
{
	mListener = NL_INVALID;
	mNumClients = 0;
}

PGameServer::~PGameServer()
{
	if (mListener != NL_INVALID)
	{
		nlClose(mListener);
		mListener = NL_INVALID;
	}
}

void PGameServer::Start()
{
	u16 Port = Config->GetOptionInt("gameserver port");
	if (Port == 0)
		Port = GAME_PORT;
	Console->Print("Starting gameserver on port %i...", Port);
	mListener = nlOpen(Port, NL_TCP);
	if (mListener != NL_INVALID)
	{
		nlListen(mListener);
	}
	else
	{
		int err = nlGetError();
		if (err == NL_SYSTEM_ERROR)
			Console->Print("Gameserver error: %s", nlGetSystemErrorStr(nlGetSystemError()));
		else
			Console->Print("Gameserver error: %s", nlGetErrorStr(err));
	}
}

void PGameServer::Update()
{
	if (mListener == NL_INVALID)
		return;

	NLsocket temp = nlAcceptConnection(mListener);		
	if (temp != NL_INVALID)
	{
		int clid = Server->NewClient();
		if (clid != -1)
		{
			Console->Print("Gameserver: client %i connected", clid);
			PClient *Client = Server->GetClient(clid);
			Client->GameConnect(temp);
			Console->Print("Client address: %s", Client->GetAddress());
			++mNumClients;
			
			PGameState *state = new PGameState();
			ClientStates.insert(std::make_pair(Client, state));
			state->TCP.mState = PGameState::TCP::GS_CONNECTED;
		}
		else
		{
			Console->Print("Gameserver: Client connection refused (server full?)");
		}
	}

	for (GameStateMap::iterator i = ClientStates.begin(); i != ClientStates.end();)
	{
		PClient *Client = i->first;
		PGameState *State = i->second;
		// node gets erased in FinalizeClient, increment iterator now
		++i;
		if (!ProcessClient(Client, State))
			FinalizeClient(Client, State);
	}	
}

bool PGameServer::HandleHandshake(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	//static const u8 HANDSHAKE1A[6]={0xfe, 0x03, 0x00, 0x80, 0x03, 0x68};
	//PGameSocket *Socket = Client->GetGameSocket();

	switch (State->TCP.mState)
	{
		case PGameState::TCP::GS_HANDSHAKE0 :
		{
			if (PacketSize == 6 && *(u16*)&Packet[3] == 0x0080 && Packet[5] == 0x78)
			{
				//FIXME: this packet seems to be unnecessary,
				// although it appears in traffic dumps
				// (causes clientside "Wrong protocol" errors)

				//Socket->Write(HANDSHAKE1A, 6);
				State->TCP.mState = PGameState::TCP::GS_AUTHENTICATE;
			}
			else
			{
				Console->Print("Gameserver protocol error (GS_HANDSHAKE0): invalid packet [%04x]", *(u16*)&Packet[3]);
				return (false);
			}
			
			break;
		}
		default:
		{
			break;
		}		
	}
	
	return true;
}

bool PGameServer::HandleAuthenticate(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	PGameSocket *Socket = Client->GetGameSocket();

	if (PacketSize > 20 && *(u16*)&Packet[3] == 0x8084)
	{
		// authentication method #1
		const u8 *Key = &Packet[5];		// password key
		u16 ULen = *(u16*)&Packet[16];		// username length
		u16 PLen = *(u16*)&Packet[18];		// password length
		char *UserID = (char*)&Packet[20];	// account name
		const u8 *PW = &Packet[20+ULen];	// encoded password
		PAccount *Account = Database->GetAccounts()->Authenticate(UserID, PW, PLen, Key);
		bool Failed = false;

		if (Account)
		{
			Console->Print("Gameserver: User '%s' logged in", UserID);
			if (Account->GetLevel() == PAL_BANNED)
			{
				Console->Print("User %s is banned, connection refused", UserID);
				// TODO: ban ip for an adjustable time span?
				Failed = true; // player is banned
			}
			
			if (Account->GetLevel() == PAL_PLAYER)
			{
				if (Server->GetNumClients() > Server->GetMaxClients())
				{
					Console->Print("Server full, refusing connection from user '%s'", UserID);
					Failed = true;	// server full
				}
			}
		}
		else
		{
			Console->Print("Gameserver: User '%s': authentication failed", UserID);
			Failed = true;	// auth failed
		}
		
		if (Failed)
		{
			// TODO: is this packet correct here?
			u8 AUTHFAILED[15] = {0xfe, 0x0c, 0x00, 0x83, 0x86, 0x05, 0x00, 0x06, 0x00, 'E', 'R',
					     'R', 'O', 'R', 0};
			// TODO: send actual reason instead of ERROR
			Socket->Write(AUTHFAILED, 15);
			FinalizeClientDelayed(Client, State);
			State->TCP.mState=PGameState::TCP::GS_UNKNOWN;
		}
		else
		{
			Client->LoggedIn(Account);
			u8 AUTHOK[28] = {0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			*(u32*)&AUTHOK[5] = Account->GetID();
			Socket->Write(AUTHOK, 28);
			State->TCP.mState = PGameState::TCP::GS_GAMEDATA;
		}
	}
	else
	if(PacketSize > 29 && *(u16*)&Packet[3] == 0x0183)
	{
		// authentication method #2, sent when game starts
		const u8 *Key = &Packet[13];		// password key
		u16 PLen = *(u16*)&Packet[25];		// password length
		u16 ULen = *(u16*)&Packet[27];		// username length
		char *UserID = (char*)&Packet[29];	// account name
		const u8 *PW = &Packet[29+ULen];	// encoded password
		PAccount *Account = Database->GetAccounts()->Authenticate(UserID, PW, PLen, Key);
		bool Failed = false;

		if (Account)
		{
			/***************************************///NEW
			//Console->Print("inside : handleautenticate2");
			//PrintPacket((u8*)Packet,PacketSize);

			int value = *(u32*)&Packet[21];//i think here we must read u32 instead of u8
			PChar *Char = Database->GetChar(Account->GetChar(value));

			Client->SetCharID(Char->GetID());
			/*************************************///END NEW

			if (Account->GetLevel() == PAL_BANNED)
			{
				Console->Print("User %s is banned, connection refused", UserID);
				// TODO: ban ip for an adjustable time span?
				Failed = true; // player is banned
			}
			
			if (Account->GetLevel() == PAL_PLAYER)
			{
				if(Server->GetNumClients() > Server->GetMaxClients())
				{
					Console->Print("Server full, refusing connection from user '%s'", UserID);
					Failed = true;	// server full
				}
			}
		}
		else
		{
			Console->Print("Gameserver: User '%s': authentication failed", UserID);
			Failed = true;	// auth failed
		}
		
		if (Failed)	// something strange happened
			FinalizeClientDelayed(Client, State);
		else
		{
			Client->LoggedIn(Account);
			/*u8 AUTHOK[28]={0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00, 
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			*(u32*)&AUTHOK[5]=Account->GetID();
			Socket->Write(AUTHOK, 28);*/
			Client->SetRemoteUDPAddr(*(u32*)&Packet[5], *(u16*)&Packet[9]);
			State->TCP.mState = PGameState::TCP::GS_GETSTATUS;
			Console->Print("Gameserver: User '%s' entered game (%08x:%04x)", UserID, *(u32*)&Packet[5], *(u16*)&Packet[9]);
		}
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_AUTHENTICATE): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}

	return (true);
}

bool PGameServer::HandleGameData(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	static const u8 GAMEDATA[5] = {0xfe, 0x02, 0x00, 0x87, 0x3a};
	PGameSocket *Socket = Client->GetGameSocket();

	if (*(u16*)&Packet[3] == 0x3787)
	{
		Socket->Write(GAMEDATA, 5);
		State->TCP.mState = PGameState::TCP::GS_REQUESTCHARS;
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_GAMEDATA): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}
	
	return (true);
}

bool PGameServer::HandleRequestChars(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	PAccount *Account = Client->GetAccount();

	if (!Account)
		return false;
		
	PGameSocket *Socket = Client->GetGameSocket();
	
	struct PCharList
	{
		u16 Unknown1;
		u16 NumSlots;
		u16 Unknown2;		
	} CharList;
	
	struct PCharEntry
	{
		u32 CharID;
		u16 Type;
		u16 Color0;
		u16 Unknown1;
		u8 Head;
		u8 Torso;
		u8 Legs;
		u32 Location;
		u8 NameLen;
		u8 Unknown3;
		u8 Unknown4;
		u8 Unknown5;
		u8 Unknown6;
		u8 Unknown7;
		u8 Unknown8;
		u8 Unknown9;
		u8 Unknown10;
		u8 Unknown11;
		u8 Unknown12;
		std::string Name;
	} CharEntry[4];
	
	const int CHARBASESIZE = 28;

	if (*(u16*)&Packet[3] == 0x8284)
	{		
		CharList.NumSlots = 4;
		int NameLengths = 0;

		for (int i = 0; i < 4; i++)
		{
			CharEntry[i].Type = 0;
			CharEntry[i].Color0 = 0;
			CharEntry[i].Location = 1;
			CharEntry[i].Unknown1 = 0;
			CharEntry[i].Head = 0;
			CharEntry[i].Torso = 0;
			CharEntry[i].Legs = 0;
			CharEntry[i].Unknown3 = 1;
			CharEntry[i].Unknown4 = 1;
			CharEntry[i].Unknown5 = 1;
			CharEntry[i].Unknown6 = 1;
			CharEntry[i].Unknown7 = 1;
			CharEntry[i].Unknown8 = 0;
			CharEntry[i].Unknown9 = 0;
			CharEntry[i].Unknown10 = 0;
			CharEntry[i].Unknown11 = 0;
			CharEntry[i].Unknown12 = 0;
			PChar *Char = Database->GetChar(Account->GetChar(i));

			if (Char)
			{
				CharEntry[i].Type = Char->GetType();
				CharEntry[i].CharID = Char->GetID();
				CharEntry[i].NameLen = Char->GetName().length()+1;
				CharEntry[i].Name = Char->GetName();
				CharEntry[i].Location = Char->GetLocation();
				NameLengths += CharEntry[i].NameLen;
			}
			else
			{
				CharEntry[i].CharID = 0xffffffff;
				CharEntry[i].NameLen = 0;
			}
		}
		
		u8 PacketHeader[5] = {0xfe, 0x00, 0x00, 0x83, 0x85};
		*(u16*)&PacketHeader[1] = sizeof(u16) * 3 + (4 * CHARBASESIZE) + NameLengths + 2;
		Socket->Write(PacketHeader, 5);
		
		CharList.Unknown1 = 0x0000;
		CharList.Unknown2 = CHARBASESIZE;
		Socket->Write(CharList.Unknown1);
		Socket->Write(CharList.NumSlots);
		Socket->Write(CharList.Unknown2);

		for (int i = 0; i < 4; i++)
		{
			Socket->Write(CharEntry[i].CharID);
			Socket->Write(CharEntry[i].Type);
			Socket->Write(CharEntry[i].Color0);
			Socket->Write(CharEntry[i].Unknown1);
			Socket->Write(CharEntry[i].Head);
			Socket->Write(CharEntry[i].Torso);
			Socket->Write(CharEntry[i].Legs);
			Socket->Write(CharEntry[i].Location);
			Socket->Write(CharEntry[i].NameLen);
			Socket->Write(CharEntry[i].Unknown3);
			Socket->Write(CharEntry[i].Unknown4);
			Socket->Write(CharEntry[i].Unknown5);
			Socket->Write(CharEntry[i].Unknown6);
			Socket->Write(CharEntry[i].Unknown7);
			Socket->Write(CharEntry[i].Unknown8);
			Socket->Write(CharEntry[i].Unknown9);
			Socket->Write(CharEntry[i].Unknown10);
			Socket->Write(CharEntry[i].Unknown11);
			Socket->Write(CharEntry[i].Unknown12);

			if (CharEntry[i].Name.length() > 0)
			{
				Socket->Write(CharEntry[i].Name.c_str());
				Socket->Write((u8)0);
			}
		}
		
		State->TCP.mState = PGameState::TCP::GS_CHARLIST;
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_REQUESTCHARS): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}

	return (true);
}

bool PGameServer::HandleCharList(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	static u8 Answer[10] = {0xfe, 0x07, 0x00, 0x83, 0x86, 0, 0, 0, 0, 0};				
	PGameSocket *Socket = Client->GetGameSocket();	
	
	if (PacketSize > 9 && *(u16*)&Packet[3] == 0x8284)
	{
		u8 Mode = Packet[9];

		switch (Mode)
		{
			case 6: // request list
				return (HandleRequestChars(Client, State, Packet, PacketSize));
			
			case 5: // validate name
			{
				if (PacketSize < 30)
					return (false);
					
				// check for valid name string
				bool ValidString = false;
				for (int i = 30; i < PacketSize; i++)
					if (Packet[i] == 0)
					{
						ValidString = true;
						break;
					}
				
				if (!ValidString)
					return (false);
					
				const char *Name = (char*)&Packet[30];								
				PChar *Char = Database->GetChar(Name);
				if (!Char)
					Answer[5] = 1;	// ok
				else
					Answer[5] = 2;	// nope
				
				Socket->Write(Answer, 10);
				return (true);
			}
			
			case 3: // delete char
			{
				PAccount *Acc = Client->GetAccount();
				u8 Num = Packet[PacketSize-1];

				if (Acc)
				{
					u32 CharID = Acc->GetChar(Num);
					Acc->RemoveChar(Num);

					if (CharID != 0)
					{
						string file = PATH_CHARS;
						file += "/";
						file += CharID;
						file += ".xml";

						if (remove(file.c_str()))
							Console->Print("Char %s removed!", CharID);
						else
							Console->Print("Char %s not removed!", CharID);
					}
				}
				return (true);
			}

			case 7: // create char
			{
				if (PacketSize < 64)
					return (false);

				u32 Profession =* (u32*)&Packet[34];
				u32 Gender =* (u32*)&Packet[42];
				//u32 Faction=*(u32*)&Packet[58];
				u8 NameLen = Packet[62];

				//Console->Print("prof %i gender %i", Profession, Gender);
				char TempName[256];
				std::strncpy(TempName, (const char*)&Packet[64], NameLen);
				TempName[NameLen] = 0;

				Answer[5] = 2;	// return error if char creation fails

				// check for already used char name - should not happen though
				PChar *Char = Database->GetChar(TempName);
				if (!Char)
				{
					PAccount *Acc = Client->GetAccount();
				/************************///NEW FROM RD2D
					u32 Type = 0;
					switch (Profession)
					{
						case 0:
						{
							Type = Profession + Gender;
							break;
						}
						case 1:
						{
							Type = 2 + Gender;
							break;
						}
						case 2:
						{
							Type = 4 + Gender;
							break;
						}
						case 3:
						{
							Type = 6 + Gender;
							break;
						}
					} 

					PChar *c = Database->CreateChar(Acc->GetID(), TempName, Type, 0);

					/****************************************///END NEW FROM RD2D
					if (c)
					{
						Acc->AddChar(c->GetID());
						Answer[5] = 1;	// return success
					}
				}
				
				Socket->Write(Answer, 10);
				return (true);
			}
		}				
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_CHARLIST): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}
	
	return true;
}

bool PGameServer::HandleGetStatus(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	PGameSocket *Socket = Client->GetGameSocket();

	if (*(u16*)&Packet[3]==0x3787)
	{
		static const u8 STATUS[9] = {0xfe, 0x06, 0x00, 0x87, 0x3a, 0x11, 0x00, 0x00, 0x00};

		Socket->Write(STATUS, 9);
		State->TCP.mState = PGameState::TCP::GS_GAMEINFO;
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_GETSTATUS): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}
	
	return (true);
}

bool PGameServer::HandleGameInfo(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	Console->Print("Inside HandleGameInfo");//NEW added

	static u8 GameInfo[31] = {0xfe, 0x1c, 0x00, 0x83, 0x05,	// header
				  0x00, 0x00, 0x00, 0x00,	// account id
				  0x00, 0x00, 0x00, 0x00,	// char id
				  0x00, 0x00, 0x00, 0x00,	// udp ip
				  0x00, 0x00,			// udp port
				  0x00, 0x00, 0x00, 0x00,	// unknown
				  0x00, 0x00, 0x00, 0x00,
				  0x00, 0x00, 0x00, 0x00,	// session key		
				 };
	
	PGameSocket *Socket = Client->GetGameSocket();

	if (*(u16*)&Packet[3] == 0x3c87)
	{
		int PortFix = Config->GetOptionInt("Debug Mode");
		u16 Port = Client->OpenUDP();

		if (Port == 0)
			Console->Print("Client->OpenUDP() failed");

		if(PortFix == 1)
		{
			Port = Config->GetOptionInt("UseUDPPort");
			Console->Print("\033[22;31mUDP Port set to non-standard for debugging!\033[22;30m");
		};

		Console->Print("Using port %i for UDP connection", Port);

		*(u32*)&GameInfo[5] = Client->GetAccount()->GetID();
		*(u32*)&GameInfo[9] = Client->GetCharID(); // NEW : SOLVED ? was TODO : charid
		Console->Print("Serving char id :%d", Client->GetCharID()); //NEW

		PSocket *UDPSocket = Client->GetUDPSocket();
		u32 IP = IPStringToDWord(UDPSocket->GetLocalAddress());

		if (IP == 0)
			IP = 0x0100007f;		
		*(u32*)&GameInfo[13] = IP;
		*(u16*)&GameInfo[17] = Port;				
		Socket->Write(GameInfo, 31);
		Socket->FlushSendBuffer();
		
		static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
		Socket->Write(READY, 7);
		
		State->TCP.mState = PGameState::TCP::GS_INGAME;		
		State->UDP.mState = PGameState::UDP::GUS_SYNC0;
		//Console->Print("UDP Setup: %s", nlGetErrorStr(nlGetError()));
	}
	else
	{
		Console->Print("Gameserver protocol error (GS_GAMEINFO): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}
	
	return (true);
}

bool PGameServer::HandleUDPType03(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	if(State->UDP.mState == PGameState::UDP::GUS_SYNC1)

	{
		
		State->UDP.mState = PGameState::UDP::GUS_SYNC2;
	}
	
	return (true);
}

u32 GetTickCount()
{
	return (0);
}

bool PGameServer::HandleUDPType13(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	//u8 size = *(u8*)&Packet[0];
	u8 type = *(u8*)&Packet[1];

	switch (type)
	{
		/******************************///NEW trying to zone :-)
		case 0x03:
		{
			if (PacketSize == 18)
			{
				u32 value = *(u32*)&Packet[11]; // think we must read u32 instead of u8
				PChar *Char = Database->GetChar(Client->GetCharID());

				Char->SetLocation(value); // Warning : i setup the method SetLocation as public
				Char->SetDirtyFlag(true); // Warning : i setup the method SetLocation as public

				// TODO 1: i think we must also make a permanet save of char to hold the current position
				//		for the next time player log in
				// TODO 2: which State->UDP.mState = PGameState i must set ???
			}
			else
			{}; // TODO : decode packet's function if PacketSize!=18

			break;
		}
		/******************************///END NEW
		case 0x0b:	// ping
		{
			u32 Time = *(u32*)&Packet[2];			
			u32 LocalTime = GetTickCount();
			u8 answer[15] = {0x13, 0x02, 0x00, 0x9f, 0x9c, 0x09, 0x0b, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00};
			*(u32*)&answer[7] = LocalTime;
			*(u32*)&answer[11] = Time;
			Client->GetUDPSocket()->Write(answer, 15);
			break;
		}
		
		case 0x0c:
		{
			if (State->UDP.mState == PGameState::UDP::GUS_SYNC2)
			{
				SendBaseLine(Client, State);
				State->UDP.mState = PGameState::UDP::GUS_SYNC3;
			}
			break;
		}
		
		case 0x2a:
		{
			static u8 packet0[] = {0x13, 0x01, 0x00, 0x1e, 0x94, 0x3d, 0x03, 0x01, 0x00, 0x2c,
					       0x01, 0x01, 0x45, 0xbf, 0x30, 0x4f, 0x00, 0x00, 0x90, 0x43,
					       0x33, 0xf3, 0x87, 0xc3, 0x00, 0x00, 0xac, 0xc3, 0x00, 0x00,
					       0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					       0x02, 0x00, 0x07, 0x03, 0x06, 0xfa, 0xfa, 0xfa, 0xd2, 0xd2,
					       0xd2, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x0f,
					       0x01, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x32	
					      };
			
			Client->GetUDPSocket()->Write(packet0, sizeof(packet0));
			break;
		}
		/************************///NEW added
		case 0x20:
		{
			break;
		}

		default:
		{
			Console->Print("inside :HandleUDPType13");
			PrintPacket((u8*)Packet,PacketSize);
			break;
		}
		/*****************************///END NEW
	}

	return (true);
}

void PGameServer::SendUDPType13(PClient *Client, PGameState *State, u8 *Packet, int PacketSize)
{
	if (PacketSize > 229)
		return;

	PSocket *Socket = Client->GetUDPSocket();

	u8 Header[] = {0x13,		// type
		       0x00, 0x00,	// sequence end
		       0x00, 0x00,	// packet
		       0x00,		// size
		      };

	*(u16*)&Header[1] = State->UDP.mSequence;
	*(u16*)&Header[3] = State->UDP.mServerPacketNum++;
	Header[5] = (u8)PacketSize;

	Socket->Write(Header, sizeof(Header));
	Socket->Write(Packet, PacketSize);

	//Console->Print("Packet %04x, Seq %04x, Size %i", State->UDP.mServerPacketNum, State->UDP.mSequence, PacketSize);
}

void PGameServer::SendBaseLine(PClient *Client, PGameState *State)
{
	static u8 packet0[] = {0x13, 0x05, 0x00, 0x1f, 0x94, 
			       0xe5, 0x03, 0x05, 0x00, 0x07, 0x00, 0x00, 0x02, 0x00, 0x19, 
			       0x22, // message ? 
			       0x02, 0x01, 0x01, 0x0a, 0x00, 0xfa,
			       0x04, // Sub class (see pak_charkinds.def)
			       0x02, 0x03, 
			       0x01, 0x00, 0x00, 0x00, // char id
			       0x0d, 0x00, 0x02, 0x1c, 0x00, 0x04, 0x04,
			       0x29, 0x01, // current HLT
			       0x29, 0x01, // maximum HLT
			       0x7e, 0x01, // current PSI
			       0x7e, 0x01, // maximum PSI
			       0x65, 0x00, // current Stamina
			       0x65, 0x00, // maximum Stamina
			       0xff, 0x00, 0xff, 0x00, 
			       0x2a, 0x01, 0x2a, 0x01, 0x2a, 0x01, // maximum HLT +1 (3 times!)
			       0x64, // Synaptic Impairmant (100 - decimal of this)
			       0x80, 0x00, 0x00, 0x03,
			       0x3c, 0x00, // skill table data size
			       0x06, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x84,
			       0x14, // STR
			       0x00, 0x00, // skill pts
			       0x5e, 0x1a, 0x3c, 0x47, // current STR XP (IEEE754 format)
			       0x64, // STR grow rate
			       0x14, // maximum STR
			       0x1b, // DEX
			       0x00, 0x00, // skill pts 
			       0xda, 0x86, 0xcc, 0x47, // current DEX XP (IEEE754 format)
			       0x40, // DEX grow rate	 
			       0x23, // maximum DEX
			       0x2c, // CON
			       0x05, 0x00, // skill pts
			       0xf2, 0x47, 0x26, 0x49, // current CON XP (IEEE754 format)
			       0x28, // CON grow rate
			       0x2d, // maximum CON
			       0x43, // INT
			       0x00, 0x00, // skill pts
			       0x7f, 0x89, 0xd8, 0x4a, // current INT XP (IEEE754 format)
			       0x10, // INT grow rate
			       0x64, // maximum INT
			       0x54, // PSI
			       0x00, 0x00, // skill pts
			       0x39, 0x0c, 0x08, 0x4c, // current PSI XP (IEEE754 format)
			       0x0c, // PSI grow rate
			       0x64, // maximum PSI
			       0x00, 0x00, 0x00, 0x00, 0x04,
			       0x5e, 0x00, // subskill table size
			       0x2e, // num subskills
			       0x02, 0x00, 0x01, 
			       0x00, 0x01, // M-C, cost to raise skill
			       0x00, 0x01, // H-C, cost to raise skill
			       0x2d, 0x01, // TRA, cost to raise skill
			       0x00, 0x01, // Spare? 4
			       0x00, 0x01, // Spare? 5
			       0x00, 0x01, // Spare? 6
			       0x00, 0x01, // Spare? 7
			       0x00, 0x01, // Spare? 8
			       0x00, 0x01, // Spare? 9 (no, never!)
			       0x00, 0x01, // P-C, cost to raise skill
			       0x00, 0x01, // R-C, cost to raise skill
			       0x00, 0x01, // T-C, cost to raise skill
			       0x19, 0x01, // VHC, cost to raise skill
			       0x37, 0x02, // AGL, cost to raise skill
			       0x32, 0x02, // REP, cost to raise skill
			       0x0a, 0x01, // REC, cost to raise skill
			       0x00, 0x01, // RCL, cost to raise skill
			       0x00, 0x01, // Spare? 18
			       0x00, 0x01, // Spare? 19
			       0x29, 0x01, // ATL, cost to raise skill
			       0x27, 0x01, // END, cost to raise skill
			       0x31, 0x02, // FOR, cost to raise skill
			       0x1c, 0x01, // FIR, cost to raise skill
			       0x00, 0x01, // ENR, cost to raise skill
			       0x05, 0x01, // XRR, cost to raise skill
			       0x00, 0x01, // POR, cost to raise skill
			       0x4e, 0x03, // HLT, cost to raise skill
			       0x00, 0x01, // Spare? 28
			       0x00, 0x01, // Spare? 29
			       0x00, 0x01, // HCK, cost to raise skill
			       0x02, 0x01, // BRT, cost to raise skill
			       0x72, 0x05, // PSU, cost to raise skill
			       0x00, 0x01, // WEP, cost to raise skill
			       0x00, 0x01, // CST, cost to raise skill
			       0x00, 0x01, // RES, cost to raise skill
			       0x4b, 0x03, // IMP, cost to raise skill
			       0x00, 0x01, // Spare? 37
			       0x00, 0x01, // Spare? 38
			       0x00, 0x01, // Spare? 39
			       0x71, 0x05, // PPU, cost to raise skill
			       0x00, 0x01, // APU, cost to raise skill
			       0x52, 0x03, // MST, cost to raise skill
			       0x4f, 0x03, // PPW, cost to raise skill
			       0x00, 0x01, // PSR, cost to raise skill
			       0x00, 0x01, // WPW, cost to raise skill
			       0x05,
			       0x32, 0x00, // inventory data size		
			       0x04, 0x00, // num inventory items
			       0x07, 0x00, // data size
			       0x00,
			       0x05, 0x00, // position
			       0x00, 0x00, // item id
			       0x01
			      };

	Client->GetUDPSocket()->Write(packet0, sizeof(packet0));
	Client->GetUDPSocket()->FlushSendBuffer();
	
	static u8 packet1[] = {0x13, 0x03, 0x00, 0x20, 0x94,
			       0xe0, 0x03, 0x03, 0x00, 0x07, 0x01, 0x00, 0x02, 0x00, 0x19,
			       0xff,
			       0x07, 0x00,
			       0x00,
			       0x01, 0x00,
			       0x00, 0x03,
			       0x01,
			       0x01,
			       0x14, 0x00,
			       0x00,
			       0x02, 0x00,
			       0x00, 0x00,
			       0x22,
			       0x01, 0x82,
			       0x0a, 0x00,
			       0x04, 0x0a,
			       0xb0, 0x00,
			       0x00, 0x00,
			       0x15, 0x57,
			       0x00, 0x00,
			       0x06, 0x00, 0x00,
			       0x04, 0x00, 0xef, 0x03, 0x00, 0x06, 0x0b, 0x00, 0x01, 0x08,
			       0x00, 0x29, 0x00, 0x2d, 0x07, 0x02, 0x02, 0xff, 0xff, 0x07,
			       0x01, 0x00, 0x00, 0x0c, 0x01, 0x00, 0x00, 0x08, 0x23, 0x00,
			       0x0a,
			       0x00, 0x00, 0x00, 0x00, // cash
			       0x00, 0x00, 0x04, 0x04, 0x04,
			       0x00, 0x00, 0x00, 0x00, 0x4f, 0x02, 0x41, 0x10, 0x00, 0x00,
			       0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x07, 0x03, 0x06, 0x02,
			       0x6f, 0x49, 0x02, 0x00, 0x01, 0x09, 0x5e, 0x00, 0x14, 0x00,
			       0x01, 0x00, 0x04, 0x00, 0x40, 0x1c, 0x46, 0x00, 0x24, 0x74,
			       0x49, 0xec, 0x51, 0x38, 0x46, 0x00, 0x42, 0x6f, 0x46, 0x00,
			       0x92, 0x40, 0x46, 0x00, 0xd2, 0x69, 0x46, 0xec, 0x51, 0x38,
			       0x46, 0x0a, 0xd7, 0xa3, 0x46, 0x00, 0x40, 0x1c, 0x46, 0x00,
			       0x40, 0x1c, 0x46, 0x00, 0x40, 0x1c, 0x46, 0x00, 0x40, 0x1c,
			       0x46, 0x00, 0x40, 0x1c, 0x46, 0x00, 0x40, 0x1c, 0x46, 0x00,
			       0x40, 0x1c, 0x46, 0x00, 0x40, 0x1c, 0x46, 0x00, 0x00, 0x80,
			       0xc9, 0x00, 0x00, 0x80, 0xc9, 0x00, 0x00, 0x80, 0xc9, 0x00,
			       0x00, 0x80, 0xc9, 0x00, 0x40, 0x1c, 0x46, 0x00, 0x00, 0x00,
			       0x00, 0x01, 0x0a, 0x00, 0x00, 0x0b, 0x01, 0x00, 0x00, 0x0d,
			       0x08, 0x00, 0xfa, 0x1e, 0x4f, 0x02, 
			       0x01, 0x00, 0x00, 0x00 // char id 
			      };

	Client->GetUDPSocket()->Write(packet1, sizeof(packet1));
	Client->GetUDPSocket()->FlushSendBuffer();
	
	static u8 packet2[] = {0x13, 0x04, 0x00, 0x21, 0x94, 0x10, 0x03, 0x04, 0x00, 0x0d, 
			       0x8b, 0xdd, 0x0b, 0x00, 0x47, 0xc0, 0x22, 0x00, 0xe5, 0x0a, 
			       0xbb, 0x00
			      };

	Client->GetUDPSocket()->Write(packet2, sizeof(packet2));
	Client->GetUDPSocket()->FlushSendBuffer();
}

bool PGameServer::HandleGame(PClient *Client, PGameState *State)
{
	//PGameSocket *Socket = Client->GetGameSocket();
	PSocket *UDPSocket = Client->GetUDPSocket();
	
	int Size = 0;
	const u8 *Buf = UDPSocket->Read(&Size);

	if (Size != 0)
	{
		if (!State->UDP.mSynced && State->UDP.mState == PGameState::UDP::GUS_SYNC0 && Buf[0] == 0x01 && Size==10)
		{
			/*******************************///NEW
			PChar *Char = Database->GetChar(Client->GetCharID());
			int loc = Char->GetLocation();

			Console->Print("inside HandleGame : loc vale : %d", loc);

			SendZone(Client, State, loc);

			// "aliverep" ?
			u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
			UDPSocket->Write(up, sizeof(up));
			//State->UDP.mSynced=true;
			State->UDP.mState = PGameState::UDP::GUS_SYNC1;//was GUS_SYNC1
			/*******************************///END NEW
		}

		switch (Buf[0])
		{
			case 0x13:
			{
				u16 LastPacket = *(u16*)&Buf[1];
				u16 PID = *(u16*)&Buf[3];

				State->UDP.mSequence = LastPacket;
				State->UDP.mServerPacketNum = PID;

				int Offset = 5;
				while (Offset < Size)
				{
					u8 PSize = *(u8*)&Buf[Offset];
					u8 PType = *(u8*)&Buf[Offset+1];
					u16 PSeq = 0;
					if (PType == 0x03)
						PSeq = *(u16*)&Buf[Offset+2];
					
					HandleUDPType13(Client, State, &Buf[Offset], PSize);
					Offset += PSize+1;
				}
				break;
			}
			
			case 0x03:
			{
				HandleUDPType03(Client, State, Buf, Size);
				break;
			}
		}
		
		/*std::strstream str;
		for (int i = 0; i < Size; i++)
		{
			char hex[16];
			sprintf(hex, "%02x", Buf[i]);
			str << hex << " ";
		}
		str << '\0';
		Console->Print("-- %s", str.str());*/
	}
									 
	return (true);
}

bool PGameServer::ProcessClient(PClient *Client, PGameState *State)
{
	static const u8 HANDSHAKE0A[6] = {0xfe, 0x03, 0x00, 0x80, 0x01, 0x66};

	if (!State)
	{
		GameStateMap::iterator node = ClientStates.find(Client);
		if (node == ClientStates.end())
			return (false);

		State = node->second;
	}

	PGameSocket *Socket = Client->GetGameSocket();
	if (State->TCP.mWaitSend && Socket->GetSendBufferSize() == 0)
		return (false);

	if (State->TCP.mState==PGameState::TCP::GS_CONNECTED)
	{
		Socket->Write(HANDSHAKE0A, 6);
		State->TCP.mState = PGameState::TCP::GS_HANDSHAKE0;
	}

	if(State->TCP.mState == PGameState::TCP::GS_INGAME)
	{
		return (HandleGame(Client, State));
	}
	else
	{
		int PacketSize = 0;
		const u8 *Packet = Socket->Read(&PacketSize);

		if (PacketSize > 0)
		{
			switch (State->TCP.mState)
			{
				case PGameState::TCP::GS_HANDSHAKE0:
					return (HandleHandshake(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_AUTHENTICATE:
					return (HandleAuthenticate(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_GAMEDATA:
					return (HandleGameData(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_REQUESTCHARS:
					return (HandleRequestChars(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_CHARLIST:
					return (HandleCharList(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_GETSTATUS:
					return (HandleGetStatus(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_GAMEINFO:
					return (HandleGameInfo(Client, State, Packet, PacketSize));
				default:
					break;
			}
		}
	}

	return (true);
}

void PGameServer::ClientDisconnected(PClient *Client)
{
	GameStateMap::iterator node = ClientStates.find(Client);

	if (node == ClientStates.end())
		return;

	PGameState *State = node->second;	
	FinalizeClient(Client, State);
}

void PGameServer::UDPStreamClosed(PClient *Client)
{
	GameStateMap::iterator node = ClientStates.find(Client);

	if (node == ClientStates.end())
		return;

	PGameState *State = node->second;	
	FinalizeClient(Client, State);
}

void PGameServer::FinalizeClient(PClient *Client, PGameState *State)
{
	Console->Print("Gameserver: client %i disconnected", Client->GetIndex());
	Client->GameDisconnect();
	ClientStates.erase(Client);
	delete State;
	--mNumClients;
}

void PGameServer::FinalizeClientDelayed(PClient *Client, PGameState *State)
{
	Console->Print("Gameserver: client %i is about to be disconnected", Client->GetIndex());
	State->TCP.mWaitSend = true;
}
