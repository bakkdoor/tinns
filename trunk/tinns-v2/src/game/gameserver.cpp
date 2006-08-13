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
  MODIFIED: 01 Jul 2006 hammag
	REASON: - added set timeout to 10 msec (for ReadSetTCP select) in Start()
	          to avoid useless 100% CPU use
  MODIFIED: 30 Jul 2006 hammag
	REASON: - Fixed 0x13 0x03 0x1F message filters to enable other char than ID 1 to play ... :P

  MODIFIED: 12 Aug 2006 hammag
	REASON: - Fixed	BuildCharPosUpdateMsg() to send correct characters orientation to other characters
		          
    TODO:
    - Deny login if char is already online (More information about the login procedure is necessary to do that)
    - Take main loop timeout setting from config file
    - Add Regex control to new char name validation
    - Add Check of Char offline (and unloaded) before deleting (from the char choosing i/f) if multiple login allowed for the same account
    - Check if adding to Client to Client manager shouldn't be done only one UDP connection done ?
        => Risk of sending UDP chat on non-socket ????
    - put back management of default value for gameserver_port in config.cpp

*/


#include "main.h"

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
	ServerSock->settimeout(0, 10000);
	
	SetGameTime(0);	//Init time

  if (Config->GetOptionInt("gameserver_udpport_max") - Config->GetOptionInt("gameserver_udpport_min") + 1 < Config->GetOptionInt("maxclients"))
  {
    Console->Print("%s UDP port range setting doesn't allow for the simultaneous max clients set in config", Console->ColorText(YELLOW, BLACK, "[Warning]"));
  }
}

void PGameServer::SetGameTime(u32 newtime)
{
	if (clock_gettime(CLOCK_REALTIME, &mStartTime))
	{
	  Console->Print(RED, BLACK, "Horloge unavailable !!!");
	  perror("clock_gettime CLOCK_REALTIME");
	  mStartTime.tv_sec = 0;
    mStartTime.tv_nsec = 0;
	}  
  mBaseGameTime = newtime;    
}

u32 PGameServer::GetGameTime()
{
  //const u32 TimeFactor = 375;
  const u32 TimeFactor = 1000;
  const u32 nsTimeFactor = 1000000000 / TimeFactor;
  
  struct timespec tmpTime;
    
  if (mStartTime.tv_sec)
  {
  	clock_gettime(CLOCK_REALTIME, &tmpTime);
    return (mBaseGameTime + ((tmpTime.tv_sec - mStartTime.tv_sec) * TimeFactor) + (tmpTime.tv_nsec / nsTimeFactor) - (mStartTime.tv_nsec / nsTimeFactor));
	}
	else
	  return 0;
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
				if(Server->GetNumClients() > (Server->GetMaxClients() - Server->GetGMSlots()))
				{
					Console->Print("Server full, refusing connection from regular user '%s'", UserID);
					Failed = true;	// server full for non-GM users
				}
			}
			else if (Server->GetNumClients() > Server->GetMaxClients())
			{
					Console->Print("Server full, refusing connection from privileged user '%s'", UserID);
					Failed = true;	// server full even for GM users			  
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
			Client->SetRemoteUDPPort(*(int*)&Packet[9]);
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
			  CharEntry[i].CharID = Char->GetID();
				CharEntry[i].Type = Char->GetType();
				// CharEntry[i].Color0 = 0;
				CharEntry[i].Location = Char->GetLocation();
				// CharEntry[i].Unknown1 = 0;
				u32 tSkin, tHead, tTorso, tLegs;
				Char->GetRealLook(tSkin, tHead, tTorso, tLegs);
			  CharEntry[i].Head = tHead;
			  CharEntry[i].Torso = tTorso;
			  CharEntry[i].Legs = tLegs;
				CharEntry[i].NameLen = Char->GetName().length()+1;
				CharEntry[i].Name = Char->GetName();
				
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
						if(MySQL->GameQuery(query))
							Console->Print("Char %d not removed!", CharID);
						else
							Console->Print("Char %d removed!", CharID);
					}
				}
				return (true);
			}

			case 7: // create char
			{
				if (PacketSize < 64)
					return (false);

        u32 Slot =* (u32*)&Packet[30]; 
				//u32 nClass =* (u32*)&Packet[34]; // Not used - indirectly redundant with Profession
				u32 Profession =* (u32*)&Packet[38];
				u32 Gender =* (u32*)&Packet[42];
				u32 Head =* (u32*)&Packet[46];
				u32 Torso =* (u32*)&Packet[50];
				u32 Legs =* (u32*)&Packet[54];
				u32 Faction=* (u32*)&Packet[58];
				u8 NameLen = Packet[62];
				u8 NZSNb = Packet[63];

				char TempName[256];
				std::strncpy(TempName, (const char*)&Packet[64], NameLen);
				TempName[NameLen] = 0;

				Answer[5] = 2;	// return error if char creation fails

				// check for already used char name - should not happen though
				PChar *Char = Database->GetChar(TempName);
				if (!Char)
				{
					PAccount *Acc = Client->GetAccount();

					PChar *c = Database->CreateChar(Acc->GetID(), TempName, Gender, Profession, Faction,
					     Head, Torso, Legs, NZSNb, (const char*)&Packet[64+NameLen], Slot);
      
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
//	Console->Print("Inside HandleGameInfo");//NEW added

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
		//int PortFix = Config->GetOptionInt("debug_mode");

		ConnectionUDP* udpConn = ServerSock->getUDPConnection(IPStringToDWord(Client->GetAddress()), Client->GetRemoteUDPPort());
		Client->setUDPConnection(udpConn);
    if (!udpConn)
    {
      			Console->Print(RED, BLACK, "Client %d: UDP port setup failed", Client->GetID());
      			ClientDisconnected(Client);
    }	

		u16 Port = Client->getUDPConn()->getPort();

		if (Port == 0)
			Console->Print(RED, BLACK, "Client->OpenUDP() failed");
      

		/* if(PortFix == 1) // removed, no more use
		{
			Port = Config->GetOptionInt("useudpport");
			Console->Print(YELLOW, BLACK, "UDP Port set to non-standard for debugging!");
		}; */

u32 IP;
std::string IPServerString;
// use [server_nat_ip] for server if client is NOT on [no_nat_net] (and [no_nat_net]!=0)
if (strcmp(Config->GetOption("no_nat_net").c_str(),"0") && strncmp(Client->GetAddress(), Config->GetOption("no_nat_net").c_str(), strlen(Config->GetOption("no_nat_net").c_str())))
{
  IPServerString = Config->GetOption("server_nat_ip");
}
else // else client is "local" so use [server_ip]
{
		IPServerString = Config->GetOption("server_ip");
}
IP = IPStringToDWord(IPServerString.c_str());
//Console->Print("IP-1 %d", IP);
		if (IP == 0)
			IP = 0x0100007f;
//Console->Print("IP-2 %d", IP);
		*(u32*)&GameInfo[13] = IP;
		*(u16*)&GameInfo[17] = Port;
		Console->Print("Using UDP %s:%d on server", IPServerString.c_str(), Port);

		*(u32*)&GameInfo[5] = Client->GetAccount()->GetID();
		*(u32*)&GameInfo[9] = Client->GetCharID(); // NEW : SOLVED ? was TODO : charid
		Console->Print("Serving char id :%d", Client->GetCharID()); //NEW
		
		Socket->write(GameInfo, 31);
		Socket->flushSendBuffer();

		static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
		Socket->write(READY, 7);

		State->TCP.mState = PGameState::TCP::GS_INGAME;
		State->UDP.mState = PGameState::UDP::GUS_SYNC0;
//Console->Print("Sync Reset");
		Client->ResetTransactionID();
		  
    // Mark char as Online
    PChar *Char = Database->GetChar(Client->GetCharID());
    Char->SetOnlineStatus(true); //Also using this info to check if Char may have to be saved at client disconnect

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
//Console->Print("Synced 1");
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
  PMessage* outMsg;
  
// temp check on char localID
/* if ((Packet[4] == 0x1f) && ((*(u16*)&Packet[5]) != Client->GetLocalID()) && (State->UDP.mState >= PGameState::UDP::GUS_SYNC3) )
{
  Console->Print(YELLOW, BLACK, "Client %d: Bad Local ID received 0x%04hx, for 0x%04hx on server side",Client->GetIndex(), (*(u16*)&Packet[5]), Client->GetLocalID()); 
  Console->Print(YELLOW, BLACK, "Client %d: Packet 0x13 was type (1)0x%02hhx, (4)0x%02hhx, (7)0x%02hhx, (8)0x%02hhx",Client->GetIndex(), Packet[1], Packet[4], Packet[8]); 
} */

	switch (type)
	{
		/******************************/
		case 0x03:
		{
			if ((Packet[4] == 0x22) && ((Packet[5] == 0x0d) || (Packet[5] == 0x03) ))  // Zoning
			{
                #include "_inc/zoning.inc.cpp"
			}
			else if ((Packet[4] == 0x22) && (Packet[5] == 0x06))  // Char or Clan infoReq
      {
        outMsg = BuildCharClanInfoMsg(Client, *(u16*)&Packet[7], *(u16*)&Packet[9]);
        Client->getUDPConn()->SendMessage(outMsg);
      }
			else if ((Packet[0] == 0x06) && (Packet[4] != 0x24))  // Packets are out of order
			//else if (Packet[4] == 0x01) // NeoX's way of identifying OOO packets
			{
				Console->Print("Out of Order! (at 0x%04hx)", *(u16*)&Packet[5]);
			}
			else if ((Packet[4] == 0x1f) /*&& (Packet[5] == 0x01)*/ && (Packet[7] == 0x17)) // USE Packet (Original written by MaxxJag)
			{
                #include "_inc/use_main.inc.cpp"
			}
			else if ((Packet[4] == 0x1f) /*&& (Packet[5] == 0x01)*/ && (Packet[7] == 0x25) && (Packet[8] == 0x04))  // Change on subskills
			{
                #include "_inc/subskills.inc.cpp"
			}
			else if ((Packet[4] == 0x1f) && (Packet[7] == 0x1e))  // Inventory Moving
			{
			    //#include "_inc/inventory.inc.cpp" <- Someone has to write that :)
			}
			else if ((Packet[4] == 0x1f) && (Packet[7] == 0x22)) // Leaving Chair
			{
                #include "_inc/use_chair.inc.cpp"
            }
            else if ((Packet[4] == 0x1f) && (Packet[7] == 0x38))  // Entering Appartment with password
            {
                #include "_inc/use_aptaccess.inc.cpp"
            }
            else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x04))  // Zoning via GenRep
            {
                #include "_inc/zoning.inc.cpp"
            }
            else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x02))  // Add Genrep
            {
                #include "_inc/use_genrep.inc.cpp"
            }
            else if ((Packet[4] == 0x1f) && (Packet[7] == 0x3d) && (Packet[8] == 0x0f))  // Getting on Vehicle
            {
                #include "_inc/use_vehicle.inc.cpp"
            }
            else if ((Packet[4] == 0x1f) && (Packet[7] == 0x33) && (Packet[8] == 0x01))  // Adding player to Direct
            {
                #include "_inc/chat_direct.inc.cpp"
			}
			else if (Packet[4] == 0x1f) /*&& (Packet[5] == 0x01) && (Packet[6] == 0x00)) */
			{
			    if(Packet[7] == 0x1B || Packet[7] == 0x3B)
			    {
                    // See chat.h and chat.cpp for the complete chat-handling
                    Chat->HandleGameChat(Client, Packet);
			    }
			    else if (Packet[7] == 0x02)  // Player jumps
			    {
			        #include "_inc/player_jump.inc.cpp"
			    }
			    else if (Packet[7] == 0x4C)  // Triggers every 10 seconds... Maybe alive packet? Seems to be never changing...
			    {
			        #include "_inc/udp_0x4c.inc.cpp"
			    }
			    else
			    {
			        Console->Print("Unknown 0x1F subpacket: %2X", Packet[7]);
			    }
			}
			else
			{
				//Console->Print("OTHER!");
				//PrintPacket((u8*)Packet, PacketSize);
			}

			break;
		}
		/******************************///END NEW
		case 0x0b:	// ping // added UDP_ID & SessionID to message data
		{
			//Console->Print("Inside: Ping");
			u32 Time = *(u32*)&Packet[2];
			//u32 LocalTime = clock();
			u32 LocalTime = GameServer->GetGameTime();
			u8 answer[15] = {0x13, 0x02, 0x00, 0x9f, 0x9c, 0x09, 0x0b, 0x00, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00};
			answer[0] = 0x13;
		  *(u16*)&answer[1] = Client->GetUDP_ID();
		  *(u16*)&answer[3] = Client->GetSessionID();
		  answer[5] = 0x09;
		  answer[6] = 0x0b;
			*(u32*)&answer[7] = LocalTime;
			*(u32*)&answer[11] = Time;
			
			Client->getUDPConn()->write(answer, sizeof(answer));
      Client->getUDPConn()->flushSendBuffer();
			break;
		}

		case 0x0c:
		{
			if (State->UDP.mState == PGameState::UDP::GUS_SYNC2)
			{
//Console->Print("Synced 2");
//				Console->Print("Sending BaseLine");
				SendBaseLine(Client, State);
			}
			break;
		}

		case 0x2a:
		{
			//Console->Print("Initialize: UDP_ID: %s", *(u16*)Client->GetUDP_ID()+1);
			/* u8 packet0[] = { 0x13, 0x01, 0x00, 0x1e, 0x94,
					 0x3d,
					 0x03, 0x01, 0x00, 0x2c, 0x01, 0x01, 0x45, 0xbf, 0x30, 0x4f, 0x00, 0x00, 0x90, 0x43,
					 0x33, 0xf3, 0x87, 0xc3, 0x00, 0x00, 0xac, 0xc3, 0x00, 0x00,
					 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					 0x02, 0x00, 0x07, 0x03, 0x06, 0xfa, 0xfa, 0xfa, 0xd2, 0xd2,
					 0xd2, 0x00, 0x00, 0x00, 0x00, 0xb0, 0x00, 0x00, 0x00, 0x0f,
					 0x01, 0x00, 0x40, 0x42, 0x0f, 0x00, 0x32
				       } */
			u8 packet0[] = {0x13, 0x01, 0x00, 0xc3, 0x87,  // from NeoX
      		0x40,
      		0x03, 0x01, 0x00, 0x2c, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 
      		0x00, 0x80, 0xa4, 0x43, 0x66, 0xe6, 0x77, 0xc3, 0x00, 0x00,
      		0x03, 0xc3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      		0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x03, 0x07, 0x02, 0x00,
      		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
      		0x00, 0x00, 0x00, 0x00};

      PChar *nChar = Database->GetChar(Client->GetCharID());
      
			Client->IncreaseUDP_ID();
			*(u16*)&packet0[1] = (u16)Client->GetUDP_ID();
			*(u16*)&packet0[3] = (u16)Client->GetSessionID();
			*(u16*)&packet0[7] = (u16)Client->GetUDP_ID();
      *(f32*)&packet0[16] = (f32)((nChar->Coords).mY-32000);
			*(f32*)&packet0[20] = (f32)((nChar->Coords).mZ-32000);
			*(f32*)&packet0[24] = (f32)((nChar->Coords).mX-32000);			
			
			Client->getUDPConn()->write(packet0, sizeof(packet0));
			Client->getUDPConn()->flushSendBuffer();
			break;
		}
		/************************/
		case 0x20:
		case 0x32:
		{
            #include "_inc/player_movement.inc.cpp"
		}
        /************************/
		default:
		{
//			Console->Print("inside :HandleUDPType13");
//			PrintPacket((u8*)Packet,PacketSize);
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

void PGameServer::SendBaseLine(PClient *Client, PGameState *State)
{
    #include "_inc/baseline.inc.cpp"

	State->UDP.mState = PGameState::UDP::GUS_SYNC3;
  int nbSent;
  nbSent = ClientManager->SendUDPZoneWelcomeToClient(Client);
//Console->Print(GREEN, BLACK, " %d Welcome message were sent to client %d", nbSent, Client->GetIndex());

  PMessage* HelloMsg = BuildCharHelloMsg(Client);   
  nbSent = ClientManager->UDPBroadcast(HelloMsg, Client);
//Console->Print(GREEN, BLACK, "Client %d: Hello message sent to %d chars", Client->GetIndex(), nbSent);

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
		switch (Buf[0])
		{
			case 0x01:
			{
    		if (Size==10)
    		{
//Console->Print("Synced 0");
    		  if (State->UDP.mState != PGameState::UDP::GUS_SYNC1) // Added from NeoX
    		  {
      			/*******************************///NEW || Der fehler des nicht funzenden multi-logins liegt hier!
      			PChar *Char = Database->GetChar(Client->GetCharID());
      			u32 loc = Char->GetLocation();
      
//Console->Print("inside HandleGame : Location: %d", loc);
      
      			SendZone(Client, loc);
      
      			// "aliverep" ?
      			u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
      			*(u16*)&up[1] = Client->GetLocalID(); // Added from NeoX
      			*(u16*)&up[5] = (u16)(UDPSocket->getAddr()).sin_port; // Added from NeoX
      			UDPSocket->write(up, sizeof(up));
      			//State->UDP.mSynced=true;
      			State->UDP.mState = PGameState::UDP::GUS_SYNC1;//was GUS_SYNC1
//Console->Print("Initialize: UDP_ID");
      			Client->SetUDP_ID(0); // Added from NeoX
      			/*******************************///END NEW
    		  }
    		}
    		else
    		{			  
				  Console->Print("Other (non-sync0) Case 01");   //<- Annoying...
				}
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

    // delete client from clientmanager list => Do it before we remove network access
	ClientManager->deleteClientFromList(Client->GetLocalID());
	
	Client->GameDisconnect();
	ClientStates.erase(Client);
	delete State;
	--mNumClients;

}

void PGameServer::FinalizeClientDelayed(PClient *Client, PGameState *State)
{
	Console->Print("Gameserver: client %s is about to be disconnected", Client->GetAddress());
	State->TCP.mWaitSend = true;
}

/********************************************************************************/
/**** Packet building & sending function put here in wait for a better place ****/
/********************************************************************************/

PMessage* PGameServer::BuildCharHelloMsg(PClient* Client)
{ 
  PChar *nChar = Database->GetChar(Client->GetCharID());
  u32 nSkin, nHead, nTorso, nLegs;
    
  nChar->GetCurrentLook(nSkin, nHead, nTorso, nLegs);
    
  PMessage* tmpMsg = new PMessage(80);
  //tmpMsg->Fill(0);

  //Client->IncreaseUDP_ID(); // This must be done outside
    
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // size placeholder, set later in the function
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x25;
	*tmpMsg << (u16) Client->GetLocalID();
	*tmpMsg << (u32) nChar->GetID();

	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x06;
	*tmpMsg << (u8)0x08;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x01;
	*tmpMsg << (u8)0x01;
	*tmpMsg << (u8)0x9a;
	*tmpMsg << (u8)0x20;// Global Rank
	*tmpMsg << (u8)0x60;// Aggr rank

	*tmpMsg << (u8) nChar->GetFaction();

	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x0f;

	*tmpMsg << (u16) nSkin;
	*tmpMsg << (u8) nHead;
	*tmpMsg << (u8) nTorso;
	*tmpMsg << (u8) nLegs;

	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x4b;
	*tmpMsg << (u8)0x4b;
	*tmpMsg << (u8)0x4b;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
		//Name
	*tmpMsg << (u8) ((nChar->GetName()).length()+1);
	*tmpMsg << (nChar->GetName()).c_str();
	*tmpMsg << (u8)0x06;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u8)0x08;
	*tmpMsg << (u8)0x01;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;	

	(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
	
	return tmpMsg;
}


PMessage* PGameServer::BuildCharClanInfoMsg (PClient* nClient, u16 nReqType, u32 nCharId)
{
	int len;
  MYSQL_RES *result;
  MYSQL_ROW row;
  char query[255];
  PMessage* tmpMsg;

	switch (nReqType)
	{
	case 0: //Name Request
//Console->Print("Client %d (char %d): Character Name Request for CharID %i", nClient->GetIndex(), nClient->GetCharID(), nCharId);
		sprintf (query, "SELECT c_name FROM characters WHERE c_id = %i", nCharId);
		break;
	case 1: //Clan Long Name
//Console->Print("Client %d : Clan Long Name Request for ClanID %i",nClient->GetLocalID(), nCharId);
		sprintf (query, "SELECT cl_lname FROM clans WHERE cl_id = %i", nCharId);
		break;
	case 4: //Clan Short name
//Console->Print("Client %d : Clan Short Name Request for ClanID %i",nClient->GetLocalID(), nCharId);
		sprintf (query, "SELECT cl_sname FROM clans WHERE cl_id = %i", nCharId);
		break;
	case 5: //Clan Rank
//Console->Print("Client %d : Clan Short Name Request for ClanID %i",nClient->GetLocalID(), nCharId);
		//sprintf (query, "SELECT cl_sname FROM clans WHERE cl_id = %i", nCharId);
		return NULL;
		break;
	default:
		Console->Print(YELLOW, BLACK, "Client %d : Unhandled Info Request Type %i, Id %i",nClient->GetLocalID(), nReqType, nCharId);
		return NULL;
		break;
	}

  result = MySQL->GameResQuery(query);
  if(!result)
  {
      Console->Print(RED, BLACK, "%s Cannot get do SQL query %s ; MySQL returned", Console->ColorText(RED, BLACK, "[Error]", query));
      MySQL->ShowGameSQLError();
      return NULL;
  }

  if(mysql_num_rows(result) == 0)
  {
      MySQL->FreeGameSQLResult(result);
      return NULL;
  }

  row = mysql_fetch_row(result);
	len = (int)(strlen(row[0])+1);
	
  tmpMsg = new PMessage(32);
  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x23;
	*tmpMsg << (u8)0x06;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u16)nReqType; // wrong size here (u32) for buffer size u16 in NeoX
	*tmpMsg << (u32)nCharId;
	*tmpMsg << row[0];

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  
  MySQL->FreeGameSQLResult(result);
  
  return tmpMsg;
}

PMessage* PGameServer::BuildCharHealthUpdateMsg (PClient* nClient)
{
  PMessage* tmpMsg;

  tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)0x64; //Head Heath (% ?)
	*tmpMsg << (u8)0x64; //Body Heath 
	*tmpMsg << (u8)0x64; //Feet Heath 
	*tmpMsg << (u8)0x01;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  
  return tmpMsg;
}

PMessage* PGameServer::BuildCharPosUpdateMsg (PClient* nClient)
{
  PMessage* tmpMsg;
  PChar* nChar;
  
  tmpMsg = new PMessage(32);
  nChar = Database->GetChar(nClient->GetCharID());
  
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1b;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u16)0x0000; // pad to keep LocalID on u16
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)((nChar->Coords).mY);
	*tmpMsg << (u16)((nChar->Coords).mZ);
	*tmpMsg << (u16)((nChar->Coords).mX);
	*tmpMsg << (u16)(31910+(nChar->Coords).mUD-50);  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
	*tmpMsg << (u16)(31820+(nChar->Coords).mLR*2-179); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
	*tmpMsg << (u8)((nChar->Coords).mAct);
	*tmpMsg << (u8)0x00;
	
  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  
  return tmpMsg;
}

PMessage* PGameServer::BuildCharSittingMsg (PClient* nClient, u16 nData)
{
  PMessage* tmpMsg;
  
  tmpMsg = new PMessage(32);
  
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x32;
	*tmpMsg << (u16)nData; // ?
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u8)0xad; // ????
	*tmpMsg << (u8)0x80;
	*tmpMsg << (u8)0xf9;
	*tmpMsg << (u8)0x85;
	*tmpMsg << (u8)0x6a;
	*tmpMsg << (u8)0x98;
	*tmpMsg << (u8)0x7c;
	*tmpMsg << (u8)0x3f;
	*tmpMsg << (u8)0x8b;
	*tmpMsg << (u8)0x14;
	*tmpMsg << (u8)0x7e;
  *tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u16)0x0000;
  
  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  
  return tmpMsg;
}
