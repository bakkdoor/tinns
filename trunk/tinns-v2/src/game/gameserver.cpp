/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
	MODIFIED: 27 Nov 2005 Akiko
	REASON: - added Namikons fix (IPStringToDWord)
		- added bluehrs packet fix
	MODIFIED: 17 Dec 2005 bakkdoor
        REASON: - Due the new structure for chatsystem, changed
                    HandleGameChat(Client, Packet);
                  to
                    Chat->HandleGameChat(Client, Packet);
                - Also added ClientManager->addClientToList and ClientManager->deleteClientFromList()
                  (check Update() & FinalizeClient())
	MODIFIED: 18 Dec 2005 Namikon
	REASON: - Fixed several small issues in the main-packet if-clause (sizeof(<char>), etc)
	MODIFIED: 21 Dec 2005 Namikon
	REASON: - Fixed zoning (for now). The basic packet is broken somehow, still working on that
	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Basepacket 1 and 2 fixed. (Thanks to MaxxJag!)
              Zoning still works, same goes for skills/factions.
              Not sure about quickbelt and inventory, but it seems to be ok
	MODIFIED: 26 Dec 2005 Namikon
	REASON: - Added doors
            - Added support for Worlditem usage (Gogo/Genrep/Appitem/...)
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Changed FmtTxt() to sprintf(). It does... uhm, the same :D
            - Added SetOnlineStatus to do instand updates when char is logging in
	MODIFIED: 02 Jan 2006 Namikon
	REASON: - Added debug output function
    MODIFIED: 06 Jan 2006 Namikon
    REASON: - Added color to console outputs

    ToDo:
    - Deny login if char is already online (More information about the login procedure is necessary to do that)
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
	mNumClients = 0;
}

PGameServer::~PGameServer()
{
    ServerSock->closeServer();
}

void PGameServer::Start()
{
	u16 Port = Config->GetOptionInt("gameserver_port");
	if (Port == 0)
		Port = GAME_PORT;
	Console->LPrint("Starting gameserver on port %i...", Port);

	if (ServerSock->open(Port))
	{
		Console->LPrint(GREEN, BLACK, "Success");
	}
	else
	{
        Console->LPrint(RED, BLACK, "Failed");
	}
	Console->LClose();
}

void PGameServer::Update()
{
	if (ServerSock->newConnection())
	{
		int clid = Server->NewClient();
		if (clid != -1)
		{
			Console->Print(GREEN, BLACK, "Gameserver: client [%i] connected", clid);
			PClient *Client = Server->GetClient(clid);

			ConnectionTCP* tcpConn = ServerSock->getTCPConnection();
			Client->setTCPConnection(tcpConn);

			//ConnectionUDP* udpConn = ServerSock->getUDPConnection();
			//Client->setUDPConnection(udpConn);

			Console->Print("Client address: %s", Client->GetAddress());
			++mNumClients;

			PGameState *state = new PGameState();
			ClientStates.insert(std::make_pair(Client, state));
			state->TCP.mState = PGameState::TCP::GS_CONNECTED;

			// add the new connected client to the global clientmanager for further use in chat, etc...
			ClientManager->addClientToList(Client);
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

bool PGameServer::HandleHandshake(PGameState *State, const u8 *Packet, int PacketSize)
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
	ConnectionTCP *Socket = Client->getTCPConn();

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

			if (Account->GetLevel() == PAL_UNREGPLAYER || Account->GetLevel() == PAL_REGPLAYER)
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
			Socket->write(AUTHFAILED, 15);
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
			Socket->write(AUTHOK, 28);
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

			if (Account->GetLevel() == PAL_UNREGPLAYER || Account->GetLevel() == PAL_REGPLAYER)
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
			//Client->SetRemoteUDPAddr(*(u32*)&Packet[5], *(u16*)&Packet[9]);
			State->TCP.mState = PGameState::TCP::GS_GETSTATUS;
			Console->Print("Gameserver: User '%s' entered game (%08x:%04x)", UserID, *(u32*)&Packet[5], *(u16*)&Packet[9]);
			Client->SetTMPUDPPort(*(int*)&Packet[9]);
		}
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_AUTHENTICATE): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}

	return (true);
}

bool PGameServer::HandleGameData(PClient *Client, PGameState *State, const u8 *Packet)
{
	static const u8 GAMEDATA[5] = {0xfe, 0x02, 0x00, 0x87, 0x3a};
	ConnectionTCP *Socket = Client->getTCPConn();

	if (*(u16*)&Packet[3] == 0x3787)
	{
		Socket->write(GAMEDATA, 5);
		State->TCP.mState = PGameState::TCP::GS_REQUESTCHARS;
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_GAMEDATA): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}

	return (true);
}

bool PGameServer::HandleRequestChars(PClient *Client, PGameState *State, const u8 *Packet)
{
	PAccount *Account = Client->GetAccount();

	if (!Account)
		return false;

	ConnectionTCP *Socket = Client->getTCPConn();

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
		Socket->write(PacketHeader, 5);

		CharList.Unknown1 = 0x0000;
		CharList.Unknown2 = CHARBASESIZE;
		Socket->write(CharList.Unknown1);
		Socket->write(CharList.NumSlots);
		Socket->write(CharList.Unknown2);

		for (int i = 0; i < 4; i++)
		{
			Socket->write(CharEntry[i].CharID);
			Socket->write(CharEntry[i].Type);
			Socket->write(CharEntry[i].Color0);
			Socket->write(CharEntry[i].Unknown1);
			Socket->write(CharEntry[i].Head);
			Socket->write(CharEntry[i].Torso);
			Socket->write(CharEntry[i].Legs);
			Socket->write(CharEntry[i].Location);
			Socket->write(CharEntry[i].NameLen);
			Socket->write(CharEntry[i].Unknown3);
			Socket->write(CharEntry[i].Unknown4);
			Socket->write(CharEntry[i].Unknown5);
			Socket->write(CharEntry[i].Unknown6);
			Socket->write(CharEntry[i].Unknown7);
			Socket->write(CharEntry[i].Unknown8);
			Socket->write(CharEntry[i].Unknown9);
			Socket->write(CharEntry[i].Unknown10);
			Socket->write(CharEntry[i].Unknown11);
			Socket->write(CharEntry[i].Unknown12);

			if (CharEntry[i].Name.length() > 0)
			{
				Socket->write(CharEntry[i].Name.c_str());
				Socket->write((u8)0);
			}
		}

		State->TCP.mState = PGameState::TCP::GS_CHARLIST;
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_REQUESTCHARS): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}

	return (true);
}

bool PGameServer::HandleCharList(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize)
{
	static u8 Answer[10] = {0xfe, 0x07, 0x00, 0x83, 0x86, 0, 0, 0, 0, 0};
	ConnectionTCP *Socket = Client->getTCPConn();

	if (PacketSize > 9 && *(u16*)&Packet[3] == 0x8284)
	{
		u8 Mode = Packet[9];

		switch (Mode)
		{
			case 6: // request list
				return (HandleRequestChars(Client, State, Packet));

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

				Socket->write(Answer, 10);
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
						char query[100];
						sprintf(query, "DELETE FROM characters WHERE c_id = %d LIMIT 1", CharID);
						if(MySQL->Query(query))
							Console->Print("Char %s not removed!", CharID);
						else
							Console->Print("Char %s removed!", CharID);
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

				Socket->write(Answer, 10);
				return (true);
			}
		}
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_CHARLIST): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}

	return true;
}

bool PGameServer::HandleGetStatus(PClient *Client, PGameState *State, const u8 *Packet)
{
	ConnectionTCP *Socket = Client->getTCPConn();

	if (*(u16*)&Packet[3]==0x3787)
	{
		static const u8 STATUS[9] = {0xfe, 0x06, 0x00, 0x87, 0x3a, 0x11, 0x00, 0x00, 0x00};

		Socket->write(STATUS, 9);
		State->TCP.mState = PGameState::TCP::GS_GAMEINFO;
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_GETSTATUS): invalid packet [%04x]", *(u16*)&Packet[3]);

		return (false);
	}

	return (true);
}

bool PGameServer::HandleGameInfo(PClient *Client, PGameState *State, const u8 *Packet)
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

	ConnectionTCP *Socket = Client->getTCPConn();

	if (*(u16*)&Packet[3] == 0x3c87)
	{
		int PortFix = Config->GetOptionInt("debug_mode");

		ConnectionUDP* udpConn = ServerSock->getUDPConnection(IPStringToDWord(Client->GetAddress()), Client->GetTMPUDPPort());
		Client->setUDPConnection(udpConn);

		u16 Port = Client->getUDPConn()->getPort();

		if (Port == 0)
			Console->Print(RED, BLACK, "Client->OpenUDP() failed");

		if(PortFix == 1)
		{
			Port = Config->GetOptionInt("useudpport");
			Console->Print(YELLOW, BLACK, "UDP Port set to non-standard for debugging!");
		};

		Console->Print("Using port %i for UDP connection", Port);

		*(u32*)&GameInfo[5] = Client->GetAccount()->GetID();
		*(u32*)&GameInfo[9] = Client->GetCharID(); // NEW : SOLVED ? was TODO : charid
		Console->Print("Serving char id :%d", Client->GetCharID()); //NEW

		u32 IP = IPStringToDWord(Config->GetOption("server_ip").c_str());
//Console->Print("IP-1 %d", IP);
		if (IP == 0)
			IP = 0x0100007f;

//Console->Print("IP-2 %d", IP);
		*(u32*)&GameInfo[13] = IP;
		*(u16*)&GameInfo[17] = Port;

		Socket->write(GameInfo, 31);
		Socket->flushSendBuffer();

		static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
		Socket->write(READY, 7);

		State->TCP.mState = PGameState::TCP::GS_INGAME;
		State->UDP.mState = PGameState::UDP::GUS_SYNC0;
        // Mark char as Online
        PChar *Char = Database->GetChar(Client->GetCharID());
        Char->SetOnlineStatus(true);

		//Console->Print("UDP Setup: %s", nlGetErrorStr(nlGetError()));
	}
	else
	{
		Console->Print(RED, BLACK, "Gameserver protocol error (GS_GAMEINFO): invalid packet [%04x]", *(u16*)&Packet[3]);
		return (false);
	}

	return (true);
}

bool PGameServer::HandleUDPType03(PGameState *State)
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
    PChar *Char = Database->GetChar(Client->GetCharID());
	u8 type = *(u8*)&Packet[1];

// ---------------------------------------------------------------------
//  UDP Game Packets
// ---------------------------------------------------------------------
	char DoubleDoorPacket[] = {0x13, 0x05, 0x00, 0x22, 0x94,
		0x0f,
		0x03, 0x27, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x05,
		0x00, 0x00, 0x00, 0x00, 0x15,
		0x0f,
		0x03, 0x28, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x05,
		0x00, 0x00, 0x00, 0x00, 0x15}; // 37
	char SingleDoorPacket[] = {0x13, 0x05, 0x00, 0x22, 0x94,
		0x0f,
		0x03, 0x27, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x00,
		0x00, 0xc8, 0x00, 0xff, 0x10,
		0x0f,
		0x03, 0x28, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x00,
		0x00, 0xc8, 0x00, 0xff, 0x10};  // 37

	char ZonePacket[] = {0x13, 0x05, 0x00, 0x23, 0x94,
		0x0c,
		0x03, 0x05, 0x00, 0x1f, 0x01, 0x00, 0x25, 0x13, 0x01, 0x00,
		0x0e, 0x02,
		0x17,
		0x03, 0x06, 0x00, 0x23, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x03, 0x02, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00
		};
    int ZonePacketSize = 42;

	char GenRepUse[] = {0x13, 0x1a, 0x00, 0x61, 0xb1,
		0x06,
		0x2d, 0x00, 0xc4, 0x00, 0x00, 0x0a,
	//				|Item ID |
		0x0d,
		0x03, 0x2d, 0x00, 0x1f, 0x01, 0x00, 0x2d, 0x02, 0x00, 0x00,
		0x00, 0x64, 0x00};//                      |    Location |
		//    |location| //26
    int GenRepUseSize = 26;

	char GenRepAdd[] = {0x13, 0x49, 0x00, 0x2d, 0xe1,
		0x11,
		0x03, 0x48, 0x00, 0x1f, 0x01, 0x00, 0x3d, 0x02, 0x00, 0x00,
		0x00, 0x02, 0x00, 0x00, 0x00, 0x64, 0x00};
    int GenRepAddSize = 23;

	char GenZone[] = {0x13, 0x3b, 0x00, 0x6b, 0xaf,
		0x2c,
		0x03, 0x3b, 0x00, 0x23, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xcc, 0x03, 0x00,
		0x00, 0x00, 0x00, 0x00};
    int GenZoneSize = 50;

	char WorldItemUse[] = {0x13, 0x1a, 0x00, 0x61, 0xb1,
		0x06,
		0x2d, 0x00, 0xc4, 0x00, 0x00, 0x0a,
	//				|Item ID |
		0x0b,
		0x03, 0x2d, 0x00, 0x1f, 0x01, 0x00, 0x17, 0x00, 0xc4, 0x00,
		0x00}; //                                       |Item ID | // 24
    int WorldItemUseSize = 24;

	char GogoUse[] = {0x13, 0x40, 0x00, 0x22, 0xed,
		0x0b,
		0x03, 0x40, 0x00, 0x1f, 0x01, 0x00, 0x3d, 0x0d, 0x00, 0x00,
		0x00}; // 17
    int GogoUseSize = 17;
		//This is for dungeons as it zones you right away.

	char DungeonUse[] = {0x13,
        0x49, 0x00,                             // UDP ID
        0x11, 0xd2,                             // Session ID
		0x06, 0x2d, 0x00,                       // Unknown
		0x00, 0x00,                             // Worlditem ID
		0x00, 0x0a,	0x0f, 0x03,                 // Unknown
		0x48, 0x00,                             // UDP ID
		0x1f, 0x01, 0x00, 0x38, 0x04,           // Unknown
		0x00,                                   // 0x00 for City<>Lv1, 0x01 for Lv1<>Lv2 (?)
		0x06, 0x04, 0x00, 0x00,                 // Destination WorldID
		0x09, 0x00};                            // Destination world-entry
		//0x07 0x04 0x00 0x00 = Location
    int DungeonUseSize = 28;

	char AptItemUse[] = {0x13, 0xc1, 0x00, 0x61, 0xf8,
		0x06,
		0x2d, 0x00, 0x38, 0x00, 0x00, 0x0a,
		0x11,
		0x03, 0xc1, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x01, 0x00, 0x38,
		0x00, 0x00, 0xf3, 0x00, 0x00, 0x00, 0x00};
		//          |Apt Place|
    int AptItemUseSize = 30;
    ///////////////////////////////////////////////
	char AptItemUse2[] = {0x13, 0xc3, 0x00, 0x63, 0xf8,
		0x0f,
		0x03, 0xc3, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x04, 0x00, 0xda,
		0xcf, 0x03, 0x00, 0x01, 0x00,
		0x15,
		0x1b, 0x22, 0x01, 0x00, 0x00, 0x1f, 0x49, 0x82, 0x81, 0x81,
		0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01, 0x00, 0x00, 0x00, 0x11,
		0x11};
    int AptItemUse2Size = 33;

	char AptItemFail[] = {0x13, 0x84, 0x00, 0x19, 0xec,
		0x08,
		0x03, 0x84, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x03};
    int AptItemFailSize = 14;

	char ChairUse[] = {0x13, 0x47, 0x00, 0x1a, 0xd6,
		0x0c,
		0x03, 0x46, 0x00, 0x1f, 0x01, 0x00, 0x21, 0x00, 0x68, 0x01,
		0x00, 0x00, //									|Item ID |
		0x0c,
		0x03, 0x47, 0x00, 0x1f, 0x01, 0x00, 0x21, 0x00, 0x68, 0x01,
		0x00, 0x00};//									|Item ID |
    int ChairUseSize = 31;

	char ExitChair[] = {0x13, 0x49, 0x00, 0x1c, 0xd6,
		0x10,
		0x03, 0x49, 0x00, 0x1f, 0x01, 0x00, 0x22, 0x18, 0x83, 0x01,
		0x80, 0x5f, 0x82, 0x80, 0x80, 0xc3};
    int ExitChairSize = 22;

	char VehicleUse[] = {0x13, 0x52, 0x00, 0x94, 0xae,
		0x13,
		0x03, 0x51, 0x00, 0x1f, 0x01, 0x00, 0x3d, 0x0e, 0x00, 0x00,
		0x00, 0xfc, 0x03, 0x00, 0x00, 0x05, 0x00, 0x0f, 0x00};
    int VehicleUseSize = 25;

	char VehicleDriver[] = {0x13, 0x58, 0x00, 0x9a, 0xae,
		0x0c,
		0x03, 0x58, 0x00, 0x1f, 0x01, 0x00, 0x21, 0xfc, 0x03, 0x00,
		//										  | Item ID |
		0x00, 0x00,
		0x19,
		0x32, 0xfb, 0x03, 0x03, 0xad, 0x80, 0xf7, 0x84, 0xc9, 0x91,
		0x90, 0xbf, 0xa1, 0x2e, 0x7f, 0x02, 0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x80, 0x00, 0x80};
    int VehicleDriverSize = 44;

	char DirectAdd[] = {0x13, 0x7e, 0x00, 0xf2, 0xad,
		0x0c,
		0x03, 0x7e, 0x00, 0x1f, 0x01, 0x00, 0x32, 0x01, 0xd5, 0x3d,
		0x02, 0x00};
    int DirectAddSize = 18;
// ---------------------------------------------------------------------
//  UDP Game Packets END
// ---------------------------------------------------------------------
	switch (type)
	{
		/******************************/
		case 0x03:
		{
			if ((Packet[4] == 0x22) && (Packet[5] == 0x0d)) {

				//Client->IncreaseUDP_ID();
				Client->SetUDP_ID(Client->GetUDP_ID()+1);
				*(u16*)&ZonePacket[7] = Client->GetUDP_ID();

				//Client->IncreaseUDP_ID();
				Client->SetUDP_ID(Client->GetUDP_ID()+1);
				*(u16*)&ZonePacket[20] = Client->GetUDP_ID();
				*(u16*)&ZonePacket[1] = Client->GetUDP_ID();
				*(u16*)&ZonePacket[3] = Client->GetSessionID();
				u32 value = *(u32*)&Packet[11];
				Char->SetLocation(value);

				int SizeZP = 42;
				//Console->Print("Zoning Stage 1: New location: %d", value);  <- Commented out, since it works now
				*(u32*)&ZonePacket[SizeZP - 8] = *(u32*)&Packet[15];
				ZonePacket[38] = ZonePacket[14];
				ZonePacket[39] = ZonePacket[15];
				Client->getUDPConn()->write(ZonePacket, SizeZP);
			} else if ((Packet[0] == 0x06) && (Packet[4] != 0x24)) {
				// I'm assuming this means out of order
				Console->Print("Out of Order!");
			} else if ((Packet[4] == 0x22) && (Packet[5] == 0x03)) {
				//Console->Print("Zoning Stage 2: Sending Zone information");  <- Commented out, since it works now

                int loc = Char->GetLocation();
                SendZone(Client, loc);
                u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
                Client->getUDPConn()->write(up, sizeof(up));
                State->UDP.mState = PGameState::UDP::GUS_SYNC1;//was GUS_SYNC1
// --------------------------------------------------------------------------------------------------------
			} else if ((Packet[4] == 0x1f) && (Packet[5] == 0x01) && (Packet[7] == 0x17)) { // USE Packet (Original written by MaxxJag)
// --------------------------------------------------------------------------------------------------------
                if(Packet[8] == 0x00) {  // 0x00 means Isnt a door
                    PAccount *Account = Client->GetAccount();
                    if(Account->IsAdminDebug() == true)
                    {
                        char debugmsg[100];
                        sprintf(debugmsg, "ID: %d LOC: %d", *(unsigned short*)&Packet[9], Char->GetLocation());
                        Chat->send(Client, CHAT_DIRECT, "System", debugmsg);
                    }
                    //Console->Print("Usage of non-door worlditem ID %d, SubClassID %d", Packet[9], Packet[10]);
                    char ErrorMsg[1024];
                    int itemID = *(unsigned short*)&Packet[9];
                    //int subClass = *(unsigned short*)&Packet[10];
                    int Location = Char->GetLocation();
                    //int worlditemtype = MySQL->GetWorldItemType(*(unsigned short*)&Packet[9], *(unsigned short*)&Packet[10], Char->GetLocation());
                    int worlditemtype = MySQL->GetWorldItemType(itemID, Location);
                    if(worlditemtype == -2)
                    {
                        sprintf(ErrorMsg, "Duplicate entry for WorlditemID %d, Location %d!", itemID, Location);
                        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
                        return(true);
                    }
                    if(worlditemtype == -1)
                    {
                        sprintf(ErrorMsg, "WorlditemID %d, Location %d is unknown", itemID, Location);
                        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
                        return(true);
                    }
                    if (worlditemtype == 1) // GenRep
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&GenRepUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&GenRepUse[3] = Client->GetSessionID();
                        *(unsigned short*)&GenRepUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&GenRepUse[8] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(GenRepUse, GenRepUseSize);
                        Console->Print("Genrep");
//                        Network_SendUDP (GenRepUse, sizeof(GenRepUse), ClientNum);
                    }
                    else if (worlditemtype == 2) // Appartment Station // Lift Access IF
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&AptItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&AptItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&AptItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&AptItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&AptItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(AptItemUse, AptItemUseSize);
                        Console->Print("Appartmentstation");
//                        Network_SendUDP (AptItemUse, sizeof(AptItemUse), ClientNum);
                    }
                    else if (worlditemtype == 3) // GoGuardian
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&GogoUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&GogoUse[3] = Client->GetSessionID();
                        *(unsigned short*)&GogoUse[7] = Client->GetUDP_ID();

                        Client->getUDPConn()->write(GogoUse, GogoUseSize);
                        Console->Print("Gogo");
//                        Network_SendUDP (GogoUse, sizeof(GogoUse), ClientNum);
                    }
                    else if (worlditemtype == 4) // Zoning Doors
                    {
                        int option1 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 1);
                        int option2 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 2);
                        int option3 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 3);
                        Console->Print("o1: %d  o2: %d  o3: %d  src: %d", option1, option2, option3, Char->GetLocation());

                        if(option1 == -1 || option2 == -1)
                        {
                            return(true);
                        }

                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&DungeonUse[14] = Client->GetUDP_ID();
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&DungeonUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&DungeonUse[3] = Client->GetSessionID();

                        *(unsigned short*)&DungeonUse[8] = *(unsigned short*)&Packet[9];

                        *(u8*)&DungeonUse[21] = option3;
                        *(unsigned int*)&DungeonUse[22] = option1;
                        *(unsigned short*)&DungeonUse[26] = option2;
                        //Console->Print("Warping player from %d to %d entry %d", Char->GetLocation(), option1, option2);

                        Client->getUDPConn()->write(DungeonUse, DungeonUseSize);
                    }
                    else if (worlditemtype == 5) // Chairs
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&ChairUse[7] = Client->GetUDP_ID();
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&ChairUse[20] = Client->GetUDP_ID();
                        *(unsigned short*)&ChairUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&ChairUse[3] = Client->GetSessionID();

                        *(unsigned short*)&ChairUse[14] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&ChairUse[27] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(ChairUse, ChairUseSize);
                        Console->Print("Chair");
//                        Network_SendUDP (WorldItemUse, sizeof(WorldItemUse), ClientNum);
                    }
                    else if (worlditemtype == 6) // Condition Locks
                    {
                        int option1 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 1);
                        int option2 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 2);
                        Console->Print("Condition lock");
                        switch (option1)
                        {
                            default:
                                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                                //Client->IncreaseUDP_ID();
                                *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                                *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                                *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                                *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                                *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                                Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                                Console->Print("default lock");
                                break;

                            case 2: // Appartment and Locked
                                if(MySQL->GetAptOwner(Char->GetLocation()) == (int)Char->GetID())
                                {
                                    char SendBuffer[] = {0x13, 0x00, 0x00, 0x00, 0x00,
												0x06,
												0x2d, 0x00, 0x00, 0x00, 0x00, 0x0a,
												0x0f,
												0x03, 0x28, 0x00, 0x1b, 0x81, 0x00, 0x00, 0x00, 0x20, 0x00,
												0x00, 0x00, 0x00, 0x00, 0x00,
												0x10,
												0x03, 0x3c, 0x00, 0x1f, 0x01, 0x00, 0x25, 0x13, 0x28, 0xb5,
												0x13, 0x00, 0x00, 0x60, 0x71, 0xc5};
                                    //Client->IncreaseUDP_ID();
                                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                                    *(unsigned short*)&SendBuffer[14] = Client->GetUDP_ID();
                                    //Client->IncreaseUDP_ID();
                                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                                    *(unsigned short*)&SendBuffer[30] = Client->GetUDP_ID();
                                    *(unsigned short*)&SendBuffer[1] = Client->GetUDP_ID();
                                    *(unsigned short*)&SendBuffer[3] = Client->GetSessionID();

                                    *(unsigned short*)&SendBuffer[8] = *(unsigned short*)&Packet[9];
                                    *(unsigned short*)&SendBuffer[17] = *(unsigned short*)&option2;
                                    Console->Print("Case2 lock");

                                    switch(option2)
                                    {
                                        case 3: // Single Locked
                                            SendBuffer[22] = 0x00;
                                            SendBuffer[24] = 0x64;
                                            SendBuffer[26] = 0xFF;
                                            SendBuffer[27] = 0x10;
                                            break;
                                        case 4: // Double Locked
                                            SendBuffer[22] = 0x05;
                                            SendBuffer[24] = 0x00;
                                            SendBuffer[26] = 0x00;
                                            SendBuffer[27] = 0x15;
                                            break;
                                        default:
                                            SendBuffer[22] = 0x00;
                                            SendBuffer[24] = 0x64;
                                            SendBuffer[26] = 0xFF;
                                            SendBuffer[27] = 0x10;
                                            break;
                                    }
                                    Client->getUDPConn()->write(SendBuffer, 45);
                                }
                                else // Not owner? Buzz!
                                {
                                    char SendBuffer[] = {0x13, 0x21, 0x00, 0xfc, 0xb1, 0x09, 0x03,
                                    0x21, 0x00, 0x2d, 0x00, 0x08, 0x00, 0x00, 0x05};

                                    Console->Print("Buzzing door");

                                    //Client->IncreaseUDP_ID();
                                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                                    *(unsigned short*)&SendBuffer[1] = Client->GetUDP_ID();
                                    *(unsigned short*)&SendBuffer[3] = Client->GetSessionID();
                                    *(unsigned short*)&SendBuffer[7] = Client->GetUDP_ID();

                                    *(unsigned short*)&SendBuffer[11] = *(unsigned short*)&Packet[9];

                                    //Network_SendUDP (SendBuffer, 15, ClientNum);
                                    Client->getUDPConn()->write(SendBuffer, 15);
                                }
                                break;
                        }
                    }
// *************************************************************************************************************
                    else if (worlditemtype == 7) // Venture Warp
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Venture");
                    }
                    else if (worlditemtype == 8) // Vehicle Depot
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("VehicDepot");
                    }
                    else if (worlditemtype == 9) // Holo Exit
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Holoexit");
                    }
                    else if (worlditemtype == 10) // Holo Weapon
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Holo weapon");
                    }
                    else if (worlditemtype == 11) // Holo Heal
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Holoheal");
                    }
                    else if (worlditemtype == 12) // CityCom
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Citycom");
                    }
                    else if (worlditemtype == 13) // OutFitters
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Outfitter");
                    }
                    else if (worlditemtype == 14) // Cabs
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Cab");
                    }
                    else if (worlditemtype == 15) // Containers
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Container");
                    }
                    else if (worlditemtype == 16) // Street Signs
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Street sign");
                    }
                    else if (worlditemtype == 17) // Street Signs
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Street sign 2");
                    }
                    else if (worlditemtype == 18) // Recreation Units
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Recreation unit");
                    }
                    else if (worlditemtype == 19) // Outpost hack
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
                        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

                        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
                        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

                        Client->getUDPConn()->write(WorldItemUse, WorldItemUseSize);
                        Console->Print("Outpost hack");
                    }
//*********************************************************************************************************
                    else
                    {
                        Chat->send(Client, CHAT_DIRECT, "System", "This kind of worlditem isnt coded yet");
                    }
                    //PrintPacket((u8*)Packet, PacketSize);
// --------------------------------------------------------------------------------------------------------
                } else {
                    //Console->Print("Usage of door worlditem");
                    PAccount *Account = Client->GetAccount();
                    if(Account->IsAdminDebug() == true)
                    {
                        char debugmsg[100];
                        sprintf(debugmsg, "ID: %d LOC: %d", *(unsigned short*)&Packet[8], Char->GetLocation());
                        Chat->send(Client, CHAT_DIRECT, "System", debugmsg);
                    }
                    char ErrorMsg[1024];
                    int i = MySQL->GetWorldDoorType(*(unsigned int*)&Packet[8], Char->GetLocation());
                    if(i == -2) {
                        if(Char->GetLocation() > 100000)
                            sprintf(ErrorMsg, "Duplicate entry for doorID %d, appartment %d", Packet[8], Char->GetLocation() - 100000);
                        else
                            sprintf(ErrorMsg, "Duplicate entry for doorID %d, location %d", Packet[8], Char->GetLocation());

                        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
                    }
                    else if(i == -1)
                    {
                        if(Char->GetLocation() > 100000)
                            sprintf(ErrorMsg, "Doortype not found: DoorID %d, appartment %d", Packet[8], Char->GetLocation() - 100000);
                        else
                            sprintf(ErrorMsg, "Doortype not found: DoorID %d, location %d", Packet[8], Char->GetLocation());

                        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
                    }
                    else if(i == 1)  // Single door
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&SingleDoorPacket[7] = Client->GetUDP_ID()-1;
                        *(unsigned short*)&SingleDoorPacket[23] = Client->GetUDP_ID();
                        *(unsigned short*)&SingleDoorPacket[1] = Client->GetUDP_ID();
                        *(unsigned short*)&SingleDoorPacket[3] = Client->GetSessionID();

                        *(unsigned int *)&SingleDoorPacket[10] = *(unsigned int *)&Packet[8];
                        *(unsigned int *)&SingleDoorPacket[26] = *(unsigned int *)&Packet[8];

                        int sizeDP = 37;
                        //Console->Print("Opening Singledoor id %d", Packet[8]);
                        Client->getUDPConn()->write(SingleDoorPacket, sizeDP);
                    }
                    else if(i == 2) // Double door
                    {
                        //Client->IncreaseUDP_ID();
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+2);
                        *(unsigned short*)&DoubleDoorPacket[7] = Client->GetUDP_ID()-1;
                        *(unsigned short*)&DoubleDoorPacket[23] = Client->GetUDP_ID();
                        *(unsigned short*)&DoubleDoorPacket[1] = Client->GetUDP_ID();
                        *(unsigned short*)&DoubleDoorPacket[3] = Client->GetSessionID();

                        *(unsigned int *)&DoubleDoorPacket[10] = *(unsigned int *)&Packet[8];
                        *(unsigned int *)&DoubleDoorPacket[26] = *(unsigned int *)&Packet[8];

                        int sizeDP = 37;
                        //Console->Print("Opening Doubledoor id %d", Packet[8]);
                        Client->getUDPConn()->write(DoubleDoorPacket, sizeDP);
                    }
                    else if (i == 3) //Locked Door (Opens by other means, usually access panel)
                    {
                        char SendBuffer[] = {0x13, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x007, 0x00, 0x1f, 0x01, 0x00, 0x31, 0x01, 0x81, 0x00, 0x00, 0x00};
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&SendBuffer[1] = Client->GetUDP_ID();
                        *(unsigned short*)&SendBuffer[3] = Client->GetSessionID();
                        *(unsigned short*)&SendBuffer[7] = Client->GetUDP_ID();

                        *(unsigned int *)&SendBuffer[14] = *(unsigned int *)&Packet[8];

                        Client->getUDPConn()->write(SendBuffer, 18);
                        Console->Print("locked door");
                    }
                    else if (*(unsigned int *)&Packet[8] > 1000)
                    {
                        //Client->IncreaseUDP_ID();
                        Client->SetUDP_ID(Client->GetUDP_ID()+1);
                        *(unsigned short*)&VehicleUse[1] = Client->GetUDP_ID();
                        *(unsigned short*)&VehicleUse[3] = Client->GetSessionID();
                        *(unsigned short*)&VehicleUse[7] = Client->GetUDP_ID();

                        *(unsigned int *)&VehicleUse[17] = *(unsigned int *)&Packet[8];

                        PSpawnedVehicles Vehic;
                        Vehicles->GetSpawnedVehicle (&Vehic, *(unsigned int *)&Packet[8]);
                        VehicleUse[21] = Vehic.Type;

                        Client->getUDPConn()->write(VehicleUse, VehicleUseSize);
                        Console->Print("Vehicleuse");
                    }
                    else // Wrong answer should never happen...
                    {
                        Console->Print(RED, BLACK, "Error in door function, illegal return value from GetWolrdDoorType: %d", i);
                    }
                }
// --------------------------------------------------------------------------------------------------------
			} else if ((Packet[4] == 0x1f) && (Packet[5] == 0x01) && (Packet[7] == 0x25) && (Packet[8] == 0x04)) {
			    // Player wants to increase subskill
			    // NC2 Version, isnt working here :(
			    Console->Print("Player wants to increase SubSkill ID %d",Packet[9]);

                u8 SubSkillPacket[] = {
                    0x13,
                    0x00, 0x00, 0x00, 0x00,                         // 2x UDP_ID Sequence
                    0x09, 0x03,
                    0x00, 0x00,                                     // 1x UDP_ID Sequence
                    0x1F, 0x01, 0x00, 0x25, 0x23, 0x28, 0x11, 0x03,
                    0x00, 0x00,                                     // 1x UDP_ID Sequence
                    0x1F, 0x01, 0x00, 0x25, 0x13,
                    0x00, 0x00,                                     // 1x UDP_ID Sequence
                    0x09,
                    0x00,                                           // Skill ID
                    0x00, 0x01,
                    0x00, 0x05,                                     // Remaining Skill PTS
                    0x00
                };

                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(u16*)&SubSkillPacket[1] = Client->GetUDP_ID();
                *(u16*)&SubSkillPacket[3] = Client->GetSessionID();
                *(u16*)&SubSkillPacket[7] = Client->GetUDP_ID();

                *(u8*)&SubSkillPacket[27] = Packet[9];

                Client->getUDPConn()->write(SubSkillPacket, sizeof(SubSkillPacket));
			} else if ((Packet[4] == 0x1f) && (Packet[7] == 0x1e)) {
			    // Inventory Moving
			} else if ((Packet[4] == 0x1f) && (Packet[7] == 0x22)) {  // Leaving Chair
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&ExitChair[1] = Client->GetUDP_ID();
                *(unsigned short*)&ExitChair[3] = Client->GetSessionID();
                *(unsigned short*)&ExitChair[7] = Client->GetUDP_ID();

                *(u16*)&ExitChair[13] = Char->Coords.mY + 768;
                *(u16*)&ExitChair[15] = Char->Coords.mZ + 768;
                *(u16*)&ExitChair[17] = Char->Coords.mX + 768;
                *(u8*)&ExitChair[19] = Char->Coords.mUD;
                *(u8*)&ExitChair[20] = Char->Coords.mLR;
                *(u8*)&ExitChair[21] = Char->Coords.mACT;

                Client->getUDPConn()->write(ExitChair, ExitChairSize);
                //Console->Print("%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x", ExitChair[0], ExitChair[1], ExitChair[2], ExitChair[3], ExitChair[4], ExitChair[5], ExitChair[6], ExitChair[7], ExitChair[8], ExitChair[9], ExitChair[10], ExitChair[11], ExitChair[12], ExitChair[13], ExitChair[14], ExitChair[15], ExitChair[16], ExitChair[17], ExitChair[18], ExitChair[19], ExitChair[20], ExitChair[21]);
                //Console->Print("leaving chair");
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x38)) { // Apt
                Console->Print("User %d, Access apt with name/pw %s", Client->GetCharID(), Packet+14);
                int i = MySQL->GetAptID(*(unsigned int*)&Packet[9], Packet+14);
                if(i != 0)
                {
                    //Client->IncreaseUDP_ID();
                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                    *(unsigned short*)&AptItemUse2[1] = Client->GetUDP_ID();
                    *(unsigned short*)&AptItemUse2[3] = Client->GetSessionID();
                    *(unsigned short*)&AptItemUse2[7] = Client->GetUDP_ID();
                    *(unsigned int*)&AptItemUse2[15] = i;

                    Char->SetLocation(i);

                    Client->getUDPConn()->write(AptItemUse2, AptItemUse2Size);
                }
                else // Wrong password or that entry doesnt have that apt
                {
                    //Client->IncreaseUDP_ID();
                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                    *(unsigned short*)&AptItemFail[1] = Client->GetUDP_ID();
                    *(unsigned short*)&AptItemFail[3] = Client->GetSessionID();
                    *(unsigned short*)&AptItemFail[7] = Client->GetUDP_ID();

                    Client->getUDPConn()->write(AptItemFail, AptItemFailSize);
                    Console->Print("Appartitemfail");
                }
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x03)) {
                /*Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&GenZone[1] = Client->GetUDP_ID();
                *(unsigned short*)&GenZone[3] = 37917+Client->GetUDP_ID();
                *(unsigned short*)&GenZone[7] = Client->GetUDP_ID();

                Client->getUDPConn()->write(GenZone, GenZoneSize);

                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&Zoning1Packet[7] = Client->GetUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&Zoning1Packet[20] = Client->GetUDP_ID();
                *(unsigned short*)&Zoning1Packet[1] = Client->GetUDP_ID();
                *(unsigned short*)&Zoning1Packet[3] = 37917+Client->GetUDP_ID();

                Char->SetLocation()

                TODO:
                - Add Char->GetAppartment(), read that from MySQL DB

                */
    //*******************************************************************************************
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x04)) {
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&GenZone[1] = Client->GetUDP_ID();
                *(unsigned short*)&GenZone[3] = Client->GetSessionID();
                *(unsigned short*)&GenZone[7] = Client->GetUDP_ID();

                Client->getUDPConn()->write(GenZone, GenZoneSize);

                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&ZonePacket[7] = Client->GetUDP_ID();
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&ZonePacket[20] = Client->GetUDP_ID();
                *(unsigned short*)&ZonePacket[1] = Client->GetUDP_ID();
                *(unsigned short*)&ZonePacket[3] = Client->GetSessionID();

                Char->SetLocation(*(u32*)&Packet[11]);
                *(unsigned int*)&ZonePacket[34] = *(unsigned int*)&Packet[15];
                Client->getUDPConn()->write(ZonePacket, ZonePacketSize);
                Console->Print("Genrepzoning");
    //*******************************************************************************************
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x02)) { // Add Genrep
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&GenRepAdd[1] = Client->GetUDP_ID();
                *(unsigned short*)&GenRepAdd[3] = Client->GetSessionID();
                *(unsigned short*)&GenRepAdd[7] = Client->GetUDP_ID();
                *(unsigned int*)&GenRepAdd[17] = *(unsigned int*)&Packet[12];
                *(unsigned short*)&GenRepAdd[21] = *(unsigned short*)&Packet[16];

                Client->getUDPConn()->write(GenRepAdd, GenRepAddSize);
                Console->Print("Add genrep");
    //*******************************************************************************************
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x0f)) { // Getting on Vehicle
                Console->Print("Someone trying to get into a vehicle. Seat Nr. %d", Packet[16]);
                if(Packet[16] == 0)
                {
                    //Client->IncreaseUDP_ID();
                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                    *(unsigned short*)&VehicleDriver[1] = Client->GetUDP_ID();
                    *(unsigned short*)&VehicleDriver[3] = Client->GetSessionID();
                    *(unsigned short*)&VehicleDriver[7] = Client->GetUDP_ID();
                    *(unsigned short*)&VehicleDriver[13] = *(unsigned short*)&Packet[13];

                    Client->getUDPConn()->write(VehicleDriver, VehicleDriverSize);
                    Console->Print("Getting on vehicle");
                }
    //*******************************************************************************************
            } else if ((Packet[4] == 0x1f) && (Packet[7] == 0x33) && (Packet[8] == 0x01)) { // Adding player to Direct
                    //Client->IncreaseUDP_ID();
                    Client->SetUDP_ID(Client->GetUDP_ID()+1);
                    *(unsigned short*)&DirectAdd[1] = Client->GetUDP_ID();
                    *(unsigned short*)&DirectAdd[3] = Client->GetSessionID();
                    *(unsigned short*)&DirectAdd[7] = Client->GetUDP_ID();

                    Client->getUDPConn()->write(DirectAdd, DirectAddSize);
                    Console->Print("Adding player to direct");
    //*******************************************************************************************
            //} else if ((Packet[4] == 0x1f) && (Packet[7] == 0x33) && (Packet[8] == 0x01)) { // Adding player to Direct
    //*******************************************************************************************
			} else if ((Packet[4] == 0x1f) && (Packet[5] == 0x01) && (Packet[6] == 0x00)) {
			    if(Packet[7] == 0x1B || Packet[7] == 0x3B)
			    {
                    // See chat.h and chat.cpp for the complete chat-handling
                    Chat->HandleGameChat(Client, Packet);
			    }
			    else if (Packet[7] == 0x02)
			    {
			        Console->Print("Jump!");
			    }
			    else if (Packet[7] == 0x4C)
			    {
			        // Triggers every 10 seconds... Maybe alive packet? Seems to be never changing...
			    }
			    else
			    {
			        Console->Print("Unknown 0x1F subpacket: %2X", Packet[7]);
			    }
			} else {
				//Console->Print("OTHER!");
				//PrintPacket((u8*)Packet, PacketSize);
			}

			break;
		}
		/******************************///END NEW
		case 0x0b:	// ping
		{
			//Console->Print("Inside: Ping");
			u32 Time = *(u32*)&Packet[2];
			u32 LocalTime = clock();
			u8 answer[15] = {0x13, 0x02, 0x00, 0x9f, 0x9c, 0x09, 0x0b, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00};
			*(u32*)&answer[7] = LocalTime;
			*(u32*)&answer[11] = Time;
			Client->getUDPConn()->write(answer, sizeof(answer));
			break;
		}

		case 0x0c:
		{
			if (State->UDP.mState == PGameState::UDP::GUS_SYNC2)
			{
				Console->Print("Sending BaseLine");
				SendBaseLine(Client);
				State->UDP.mState = PGameState::UDP::GUS_SYNC3;
			}
			break;
		}

		case 0x2a:
		{
			//Console->Print("Initialize: UDP_ID: %s", *(u16*)Client->GetUDP_ID()+1);
			u8 packet0[] = { 0x13, 0x01, 0x00, 0x1e, 0x94,
					 0x3d,
					 0x03, 0x01, 0x00, 0x2c, 0x01, 0x01, 0x45, 0xbf, 0x30, 0x4f, 0x00, 0x00, 0x90, 0x43,
					 0x33, 0xf3, 0x87, 0xc3, 0x00, 0x00, 0xac, 0xc3, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x02, 0x00, 0x07, 0x03, 0x06, 0xfa, 0xfa, 0xfa, 0xd2, 0xd2,
					 0xd2, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x0f,
					 0x01, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x32
				       };
			Client->SetUDP_ID(1);
			//Client->IncreaseUDP_ID();
			packet0[1] = Client->GetUDP_ID();
			packet0[3] = Client->GetSessionID();
			packet0[7] = Client->GetUDP_ID();
			Client->getUDPConn()->write(packet0, sizeof(packet0));
			break;
		}
		/************************/
		case 0x20:
		{
            if(Packet[4] == 0x7F)
            {
                //Playerposition update
                Char->Coords.mY = *(u16*)&Packet[5];
                Char->Coords.mZ = *(u16*)&Packet[7];
                Char->Coords.mX = *(u16*)&Packet[9];
                Char->Coords.mUD = Packet[11];
                Char->Coords.mLR = Packet[12];
                Char->Coords.mACT = Packet[13];
                // movement byte:
                // 0x00 NC has no focus (player alt+tab'ed out)
                // 0x20 Char does nothing
                // 0x22 kneeing
                // 0x28 left step
                // 0x30 right step
                // 0x40 walking (not running)
                // 0x60 forward
                // 0xA0 backward
                //Console->Print("Positionupdate: X(%d) Y(%d) Z(%d) U/D(%d) L/R(%d) Movement(%#.2X)", Char->Coords.mX, Char->Coords.mY, Char->Coords.mZ, Char->Coords.mUD, Char->Coords.mLR, Char->Coords.mACT);
            }
            else if (Packet[4] == 0x20)
            {
                Char->Coords.mACT = Packet[5];
                //Console->Print("Movementupdate: [%2x]", Packet[5]);
            }
            else
            {
            }
            break;
		}
        /************************/
        case 0x32:
        {
            // Vehicle updates
            if(Packet[19] == 0)
            {
                Console->Print("Vehicle going nowhere");
            }
            else
            {
                if(Packet[19] & 1)
                    Console->Print("Vehicle going left");
                if(Packet[19] & 2)
                    Console->Print("Vehicle going right");
                if(Packet[19] & 4)
                    Console->Print("Vehicle going forward");
                if(Packet[19] & 8)
                    Console->Print("Vehicle going back");
            }
            //Playerposition update
            Char->Coords.mY = *(u16*)&Packet[5];
            Char->Coords.mZ = *(u16*)&Packet[7];
            Char->Coords.mX = *(u16*)&Packet[9];
            Char->Coords.mUD = Packet[11];
            Char->Coords.mLR = Packet[12];
            Char->Coords.mACT = Packet[13];
            // movement byte:
            // 0x00 NC has no focus (player alt+tab'ed out)
            // 0x20 Char does nothing
            // 0x22 kneeing
            // 0x28 left step
            // 0x30 right step
            // 0x40 walking (not running)
            // 0x60 forward
            // 0xA0 backward
            break;
        }
        /************************/
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

	ConnectionUDP *Socket = Client->getUDPConn();

	u8 Header[] = {0x13,		// type
		       0x00, 0x00,	// sequence end
		       0x00, 0x00,	// packet
		       0x00,		// size
		      };

	*(u16*)&Header[1] = Client->GetUDP_ID();//State->UDP.mSequence;
	*(u16*)&Header[3] = Client->GetSessionID();
	Header[5] = (u8)PacketSize;

	//Client->IncreaseUDP_ID();
	Client->SetUDP_ID(Client->GetUDP_ID()+1);
	Socket->write(Header, sizeof(Header));
	Socket->write(Packet, PacketSize);

	Console->Print("Packet %04x, Seq %04x, Size %i", State->UDP.mServerPacketNum, State->UDP.mSequence, PacketSize);
}

void PGameServer::SendBaseLine(PClient *Client)
{
/*u8 packet0[] = {0x13,
0x02, 0x00,			// UDP_ID
0x1F, 0x94,			// UDP_ID
0xE5, 0x03, 0x02, 0x00, 0x07, 0x00, 0x00, 0x02, 0x00, 0x19,
0x22,				// Message ?
0x02, 0x01,			// UDP_ID
0x01,				//Section 1		0x12
0x0A, 0x00,			//Section 1 size (10)
0xFA,
0x0B,				// Subclass
0xA0, 0x8A,
0x19, 0xF4, 0x00, 0x00,		// Char id
0x10, 0x00,
0x02,				//Section 2
0x1C, 0x00,			//Section 2 size (28)
0x04, 0x04,

0x53, 0x02,			// Current HLT
0x53, 0x02,			// max HLT
0xaa, 0x01,			// current psi
0xaa, 0x01,			// max psi
0xee, 0x00,         // current stamina
0xee, 0x00,			// max stamina

0xFF, 0x00, 0xFF, 0x00,
//0xE1, 0x00, 0x47, 0x01, 0x47, 0x01, // max hlt +1 (3 times)
0x53, 0x02, 0x53, 0x02, 0x53, 0x02, // max hlt +1 (3 times)
0x64,				// Synaptic imp.
0x80, 0x00, 0x00,
0x03,				//Section 3
0x3C, 0x00,			//Section 3 size (60)
0x06, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,

0xFF,				// STR
0x00, 0x00,			// Skill PTS
0x00, 0x40, 0x1F, 0x44,		// Current STR XP (IEEE754 format)
0x00,				// Str grow rate
0xFF,				// Maximum str

0xFF,				// DEX
0x00, 0x00,			// Skill PTS
0x00, 0x40, 0x1F, 0x44,		// Current DEX XP (IEEE754 format)
0x00,				// Dex grow rate
0xFF,				// Maximum dex

0xFF,				// CON
0x00, 0x00,			// Skill PTS
0x00, 0x40, 0x1F, 0x44,		// Current CON XP (IEEE754 format)
0x00,				// CON grow rate
0xFF,				// Maximum CON

0xFF,				// INT
0x00, 0x00,			// Skill PTS
0x00, 0xA0, 0xFE, 0x44,		// Current INT XP (IEEE754 format)
0x00,				// INT grow rate
0xFF,				// Maximum INT

0xFF,				// PSI
0x00, 0x00,			// Skill PTS
0x00, 0x40, 0x1F, 0x44,		// Current PSI XP (IEEE754 format)
0x00,				// PSI grow rate
0xFF,				// Maximum PSI

0x00, 0x00, 0x00, 0x08,
0x04,				//Section 4
0x5E, 0x00,			//Section 4 size (94)
0x2E,				// Num subskills
0x02, 0x00, 0x01,
0xFF, 0x01,			// MC, cost to raise skill
0xFF, 0x01,			// HC, cost to raise skill
0xFF, 0x01,			// TRA, cost to raise skill
0x00, 0x01,			// Spare 4
0x00, 0x01,			// Spare 5
0x00, 0x01,			// Spare 6
0x00, 0x01,			// Spare 7
0x00, 0x01,			// Spare 8
0x00, 0x01,			// Spare 9
0xFF, 0x01,			// PC, cost to raise skill
0xFF, 0x01,			// RC, cost to raise skill
0xFF, 0x01,			// TC, cost to raise skill
0xFF, 0x01,			// VHC, cost to raise skill
0xFF, 0x01,			// AGL, cost to raise skill
0xFF, 0x01,			// REP, cost to raise skill
0xFF, 0x01,			// REC, cost to raise skill
0xFF, 0x01,			// RCL, cost to raise skill
0x00, 0x01,			// Spare 18
0x00, 0x01,			// Spare 19
0xFF, 0x01,			// ATL, cost to raise skill
0xFF, 0x01,			// END, cost to raise skill
0xFF, 0x01,			// FOR, cost to raise skill
0xFF, 0x01,			// FIR, cost to raise skill
0xFF, 0x01,			// ENR, cost to raise skill
0xFF, 0x01,			// XRR, cost to raise skill
0xFF, 0x01,			// POR, cost to raise skill
0xFF, 0x01,			// HLT, cost to raise skill
0x00, 0x01,			// Spare 28
0x00, 0x01,			// Spare 29
0xFF, 0x01,			// HCK, cost to raise skill
0xFF, 0x01,			// BRT, cost to raise skill
0xFF, 0x01,			// PSU, cost to raise skill
0xFF, 0x01,			// WEP, cost to raise skill
0xFF, 0x01,			// CST, cost to raise skill
0xFF, 0x01,			// RES, cost to raise skill
0xFF, 0x01,			// IMP, cost to raise skill
0x00, 0x01,			// Spare 37
0x00, 0x01,			// Spare 38
0x00, 0x01,			// Spare 39
0xFF, 0x01,			// PPU, cost to raise skill
0xFF, 0x01,			// APU, cost to raise skill
0xFF, 0x01,			// MST, cost to raise skill
0xFF, 0x01,			// PPW, cost to raise skill
0xFF, 0x01,			// PSR, cost to raise skill
0xFF, 0x01,			// WPW, cost to raise skill

0x05,				// Section 5
0xAE, 0x00,			// Section 5 size (174)
0x04, 0x00,			// Number of items (14)
0x07, 0x00,			// Item 1 size (7)
0x00,
0x04, 0x01,			// position of item
0xC2, 0x07,			// item id
0x01};*/
static u8 packet0[] = {0x13, 0x02, 0x00, 0x1f, 0x94,
/*5*/0xe5, 0x03, 0x02, 0x00, 0x07, 0x00, 0x00, 0x02, 0x00, 0x19,
/*15*/0x22,										//Message
0x02,
0x01,
0x01, 0x0a, 0x00,							//Packet Type 1, Size 0x00a0 (10)
/*21*/0xfa, 0x0b, 0xa0, 0x8a, 0x19, 0xf4, 0x00, 0x00, 0x10, 0x00, //Packet Type 1 Data
/*31*/0x02, 0x1c, 0x00,							//Packet Type 2, size 0x001c (28)
0x04, 0x04,
/*36*/0x18, 0x01,									//Current Health
/*38*/0x8c, 0x01,									//Max Health
/*40*/0x45, 0x00,									//Current PSI
/*42*/0x45, 0x00,									//Max PSI
/*44*/0x85, 0x00,									//Current Stamina
/*46*/0x85, 0x00,									//Max Stamina
/*48*/0xff, 0x00, 0xff, 0x00,
/*52*/0xe1, 0x00, 0x47, 0x01, 0x47, 0x01,			//?? Said to be Max health +1 *3
/*58*/0x64,										//Synaptic Impairment 100 - this
/*59*/0x80, 0x00, 0x00,
/*62*/0x03, 0x3c, 0x00,							//Skill Data size
0x06, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
/*76*/0x2f,										//STR level
/*77*/0x00, 0x00,									//STR points
/*79*/0x39, 0x59, 0x0e, 0x00,						//STR XP in float
/*83*/0x16,										//STR Growth Rate
/*84*/0x3c,										//Max STR Level
/*85*/0x3e,										//DEX level
/*86*/0x00, 0x00,									//DEX points
/*88*/0x9b, 0x6a, 0x43, 0x00,						//DEX XP in float
/*92*/0x0c,										//DEX Growth Rate
/*93*/0x50,										//Max DEX level
/*94*/0x2f,										//CON level
/*95*/0x00, 0x00,									//CON points
/*97*/0x6b, 0x18, 0x0f, 0x00,						//CON XP in float
/*101*/0x12,										//CON Growth Rate
/*102*/0x41,										//Max CON Level
/*103*/0x35,										//INT level
/*104*/0x01, 0x00,									//INT points
/*106*/0x7d, 0x67, 0x1c, 0x00,						//INT XP in float
/*110*/0x16,										//INT Growth Rate
/*111*/0x3c,										//Max INT level
/*112*/0x12,										//PSI level
/*113*/0x00, 0x00,									//PSI points
/*115*/0xdb, 0x88, 0x00, 0x00,						//PSI XP in float
/*119*/0x40,										//PSI Growth rate
/*120*/0x23,										//Max PSI Level
/*121*/0x00, 0x00, 0x00, 0x08,
/*125*/0x04, 0x5e, 0x00,							//Subskill Data Size
/*128*/0x2e,										//Subskill number
/*129*/0x02, 0x00, 0x01,
/*132*/0x19, 0x01,	//MC, Cost
/*134*/0x00, 0x01, //HC, Cost
/*136*/0x64, 0x05, //TRA, Cost
/*138*/0x00, 0x01, //Spare
/*140*/0x00, 0x01, //Spare
/*142*/0x00, 0x01, //Spare
/*144*/0x00, 0x01, //Spare
/*146*/0x00, 0x01, //Spare
/*148*/0x00, 0x01, //Spare
/*150*/0x52, 0x03, //PC, Cost
/*152*/0x00, 0x01, //RC, Cost
/*154*/0x49, 0x02, //TC, Cost
/*156*/0x3c, 0x02, //VHC, Cost
/*158*/0x1a, 0x01, //AGL, Cost
/*160*/0x37, 0x02, //REP, Cost
/*162*/0x00, 0x01, //REC, Cost
/*164*/0x00, 0x01, //RCL, Cost
/*166*/0x00, 0x01,	//Spare
/*168*/0x00, 0x01,	//Spare
/*170*/0x37, 0x02, //ATL, Cost
/*172*/0x1e, 0x01, //END, Cost
/*174*/0x37, 0x02, //FOR, Cost
/*176*/0x0a, 0x01, //FIR, Cost
/*178*/0x00, 0x01, //ENR, Cost
/*180*/0x0f, 0x01, //XRR, Cost
/*182*/0x00, 0x01, //PRO, Cost
/*184*/0x50, 0x03, //HTL, Cost
/*186*/0x00, 0x01, //Spare
/*188*/0x00, 0x01, //Spare
/*190*/0x00, 0x01, //HCK, Cost
/*192*/0x00, 0x01, //BRT, Cost
/*194*/0x00, 0x01, //PSU, Cost
/*196*/0x55, 0x03, //WEP, Cost
/*198*/0x00, 0x01, //CST, Cost
/*200*/0x58, 0x03, //RES, Cost
/*202*/0x00, 0x01, //IMP, Cost
/*204*/0x00, 0x01, //Spare
/*206*/0x00, 0x01, //Spare
/*208*/0x00, 0x01, //Spare
/*210*/0x23, 0x01, //PPU, Cost
/*212*/0x00, 0x01, //APU, Cost
/*214*/0x14, 0x01, //MST, Cost
/*216*/0x20, 0x01, //PPW, Cost
/*218*/0x00, 0x01, //PSR, Cost
/*220*/0x00, 0x01, // WPW ?
/*222*/0x05,
/*224*/0x0b, 0x00, //Inventory data size
/*226*/0x01, 0x00, //Number of Items
/*228*/0x07, 0x00, //data size
/*230*/0x00,
/*232*/0x04, 0x01, //Position
/*234*/0xc2, 0x07, //Item Id
/*236*/0x01 };

static u8 packet1[] = {0x13, 0x03, 0x00, 0x20, 0x94,
0xc4,
0x03, 0x03, 0x00, 0x07, 0x01, 0x00, 0x02, 0x00, 0x19,
0xff,
0x06, //Section 6 - QB/Processor/Implants/Armour
0x09, 0x00,
0x01,
0x06, 0x00, 0x00, 0x00, 0x51, 0x00, 0x01, 0x00, //torch in QB slot 0
0x07, //Section 7 - GoGo Contents
0x01, 0x00, 0x00, 0x0c, //Gogo header?
0x08, 0x00, //Size
0x01, //Item count
0x05, 0x00, //Item data size
0x00, 0x6d, 0x0c, 0x01, 0x04, //4bullets 8mm explosive?
0x08, //Section 8 - Buddies, GRs, etc
//New data, taken from a newly created character!
/*0x26, 0x00, 0x0a,
0xb0, 0x04, 0x00, 0x00, //cash
0x00, 0x00, //GRs tagged
//			0x04,
0x04, 0x04, 0x00,
0x00, //direct and buddies
0x00,
0x00, 0x0e,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x00, 0x00, 0x00, 0x00, //Class, Gender, and appearance
0x00,
0xff, 0xff, 0xff, 0xff, //Primary apartment - unassigned at start?
0x01, 0x00, 0x00, 0x00, 0x00,*/

0x26, 0x00,
0x0a,
0x1c, 0x04, 0x20, 0x00, //Cash
0x00, 0x00, //Number of GRs tagged
0x04, 0x04, 0x00,
0x00, //0x01 if anyone in direct chat followed by ID
0x00, //Number of buddies on list (followed by their IDs)
0x00,
0x0e, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x01, 0x00, 0x03, 0x07, 0x02, //Class/Gender, and appearance
0x00,
0x22, 0x00, 0x00, 0x00, //Primary Apartment (GR activated)
0x01, 0x00, 0x00, 0x00, 0x00,

0x09, //Section 9 - Faction Sympathies
0x62, 0x00, 0x15, 0x00,
0x0a, //Faction
0x00, 0x04,
0xd0, 0x5e, 0xac, 0x48, //Current SL (59)
//Sympathy for factions (100 = 0x00 24 74 49 reversed)
0x00, 0x40, 0x1c, 0x46, //CityAdmin (10)
0x00, 0x40, 0x1c, 0x46, //Diamond (10)
0x00, 0x24, 0x74, 0x49, //NeXT (100)
0x00, 0x40, 0x1c, 0x46, //Tangent (10)
0x00, 0x40, 0x4c, 0x46, //BioTech (11)
0xc2, 0xb4, 0xab, 0x47, //ProtoPharm (29)
0x00, 0x00, 0x80, 0x49, //Traders Union
0x00, 0x40, 0x1c, 0x46, //Tsunami Syndicate (10)
0x85, 0x5b, 0x7f, 0x47, //Black Dragon (25)
0x00, 0x24, 0x74, 0x49, //City Mercs (100)
0x00, 0x24, 0x74, 0x49, //Crahn Sect (100)
0x00, 0x00, 0x80, 0x49, //Dome Of York
0x00, 0x00, 0x80, 0xc9, //Anarchy Breed
0x00, 0x40, 0x1c, 0x46, //Fallen Angels (10)
0x0a, 0xd7, 0xd3, 0x46, //Twilight Guardians (16)
//Mobs!
0x00, 0x00, 0x80, 0xc9, //Regent's Legacy
0x00, 0x00, 0x80, 0xc9, //Regent's Mutants
0x00, 0x00, 0x80, 0xc9, //Insects
0x00, 0x00, 0x80, 0xc9, //monsters
0x00, 0x00, 0x00, 0x80, //Unknown
0x00, 0x24, 0x74, 0x49, //Highest SL character has had (100)
0x00, 0x00, 0x00, 0x00,
0x0a, //Faction
0x0a, //Section 0a - Clan Data?
0x00, 0x00,
0x0b, //Section 0b
0x01, 0x00, 0x00, 0x0d, 0x08, 0x00, 0xfa, 0x0b, 0xa0, 0x8a,
0x01, 0x00, 0x00, 0x00};


static u8 packet2[] = {0x13, 0x04, 0x00, 0x21, 0x94,
0x10,//                 \		Time		  /
0x03, 0x04, 0x00, 0x0D, 0x8B, 0xDD, 0x0B, 0x00, 0x47, 0xC0,
0x22, 0x00, 0xE5, 0x0A, 0xBB, 0x00};

//static u8 packet2[] = {0x13, 0x05, 0x00, 0xd6, 0xe3, 0x10, 0x03, 0x05,
//0x00, 0x0d, 0xac, 0x47, 0x0b, 0x00, 0xb3, 0x4c,
//0x68, 0x03, 0xe5, 0x0a, 0x5f, 0x01};


/*	u8 packet0[] = {0x13, 0x02, 0x00, 0x1f, 0x94,
			       0xe5, 0x03, 0x02, 0x00, 0x07, 0x00, 0x00, 0x02, 0x00, 0x19,
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
*/
    // Now fill our basepacket up with all playerspecific details
    PChar *Char = Database->GetChar(Client->GetCharID());
    Char->FillinCharDetails(packet0);

	//Client->IncreaseUDP_ID();
	Client->SetUDP_ID(Client->GetUDP_ID()+1);
	packet0[1] = Client->GetUDP_ID();
	packet0[3] = Client->GetSessionID();
	packet0[7] = Client->GetUDP_ID();
	Client->getUDPConn()->write(packet0, sizeof(packet0));
	Client->getUDPConn()->flushSendBuffer();


	//Client->IncreaseUDP_ID();
	Client->SetUDP_ID(Client->GetUDP_ID()+1);
	packet1[1] = Client->GetUDP_ID();
	packet1[3] = Client->GetSessionID();
	packet1[7] = Client->GetUDP_ID();
	Client->getUDPConn()->write(packet1, sizeof(packet1));
	Client->getUDPConn()->flushSendBuffer();


	//Client->IncreaseUDP_ID();
	Client->SetUDP_ID(Client->GetUDP_ID()+1);
	packet2[1] = Client->GetUDP_ID();
	packet2[3] = Client->GetSessionID();
	packet2[7] = Client->GetUDP_ID();
	*(u32*)&packet2[11] = mGameTime;
	Console->Print("GThex <%2X %2X %2X %2X> GTint <%d> GTfloat <%f>", packet2[11],  packet2[12],  packet2[13],  packet2[14], mGameTime, (float)mGameTime);
	Client->getUDPConn()->write(packet2, sizeof(packet2));
	Client->getUDPConn()->flushSendBuffer();
}

bool PGameServer::HandleGame(PClient *Client, PGameState *State)
{
	//PGameSocket *Socket = Client->GetGameSocket();
	ConnectionUDP *UDPSocket = Client->getUDPConn();
	//Console->Print("Clientadress %s", Client->GetAddress());

	int Size = 0;
	const u8 *Buf = UDPSocket->read(&Size);

	if (Size != 0)
	{
	    //Console->Print("%s: %2X %2X %2X %2X %2X %2X %2X %2X %2X %2X", Client->GetAddress(), Buf[0], Buf[1], Buf[2], Buf[3], Buf[4], Buf[5], Buf[6], Buf[7], Buf[8], Buf[9]);
		if (Buf[0] == 0x01 && Size==10)
		{
			/*******************************///NEW || Der fehler des nicht funzenden multi-logins liegt hier!
			PChar *Char = Database->GetChar(Client->GetCharID());
			int loc = Char->GetLocation();

			Console->Print("inside HandleGame : Location: %d", loc);

			SendZone(Client, loc);

			// "aliverep" ?
			u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
			UDPSocket->write(up, sizeof(up));
			//State->UDP.mSynced=true;
			State->UDP.mState = PGameState::UDP::GUS_SYNC1;//was GUS_SYNC1
			/*******************************///END NEW
		}

		switch (Buf[0])
		{
			case 0x01:
			{
				Console->Print("Case 01");   //<- Annoying...
				break;
			}
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
				//Console->Print("Case 3");  <- Annoying...
				HandleUDPType03(State);
				break;
			}
			default:
			{
				//Console->Print("Unknown: " + Buf[0]);
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

	ConnectionTCP *Socket = Client->getTCPConn();
	if (State->TCP.mWaitSend && Socket->getSendBufferSize() == 0)
		return (false);

	if (State->TCP.mState==PGameState::TCP::GS_CONNECTED)
	{
		Socket->write(HANDSHAKE0A, 6);
		State->TCP.mState = PGameState::TCP::GS_HANDSHAKE0;
	}

	if(State->TCP.mState == PGameState::TCP::GS_INGAME)
	{
		return (HandleGame(Client, State));
	}
	else
	{
		int PacketSize = 0;
		const u8 *Packet = Socket->read(&PacketSize);

		if (PacketSize > 0)
		{
			switch (State->TCP.mState)
			{
				case PGameState::TCP::GS_HANDSHAKE0:
					return (HandleHandshake(State, Packet, PacketSize));

				case PGameState::TCP::GS_AUTHENTICATE:
					return (HandleAuthenticate(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_GAMEDATA:
					return (HandleGameData(Client, State, Packet));

				case PGameState::TCP::GS_REQUESTCHARS:
					return (HandleRequestChars(Client, State, Packet));

				case PGameState::TCP::GS_CHARLIST:
					return (HandleCharList(Client, State, Packet, PacketSize));

				case PGameState::TCP::GS_GETSTATUS:
					return (HandleGetStatus(Client, State, Packet));

				case PGameState::TCP::GS_GAMEINFO:
					return (HandleGameInfo(Client, State, Packet));
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
	Console->Print(RED, BLACK, "Gameserver: client %s disconnected", Client->GetAddress());

    // Isnt working somehow. I assume that mID and mAccount is deleted already.
    // Mark char as Offline
	//PChar *Char = Database->GetChar(Client->GetCharID());
    //Char->SetOnlineStatus(false);

	Client->GameDisconnect();
	ClientStates.erase(Client);
	delete State;
	--mNumClients;


    // delete client from clientmanager list
	ClientManager->deleteClientFromList(Client);
}

void PGameServer::FinalizeClientDelayed(PClient *Client, PGameState *State)
{
	Console->Print("Gameserver: client %s is about to be disconnected", Client->GetAddress());
	State->TCP.mWaitSend = true;
}

