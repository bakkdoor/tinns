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
  MODIFIED: 01 Jul 2006 hammag
	REASON: - added set timeout to 10 msec (for ReadSetTCP select) in Start()
	          to avoid useless 100% CPU use
  MODIFIED: 27 Aug 2006 hammag
	REASON: - Removed INFO_PORT use as value is available from config
  MODIFIED: 27 Aug 2006 hammag
	REASON: - Modified GSLiveCheck() to be independant of gameserver time (no time sync needed between servers)
  MODIFIED: 27 Aug 2006 hammag
	REASON: - Display the client id in Client connection message rather than the always increasing mNumClients
	        - Removed use of mNumClients in PInfoServer. the count is done in PServer.

    ToDo:
    - Take main loop timeout setting from config file
*/

#include "main.h"

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

PInfoServer::PInfoServer()
{
	//mNumClients = 1;
	mLivecheckInterval = Config->GetOptionInt("gameserver_livecheck");
}

PInfoServer::~PInfoServer()
{
    Console->Print("Closing Infoserver...");

    ServerSock->closeServer();

	for(InfoStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end(); i++)
		delete i->second;
}

void PInfoServer::Start()
{
	u16 Port = Config->GetOptionInt("infoserver_port");
	Console->LPrint("Starting Infoserver on port %i...", Port);

	if(ServerSock->open(Port))
	{
	    Console->LPrint(GREEN, BLACK, "Success");
	    Console->LClose();
	}
	else
	{
		Console->LPrint(RED, BLACK, "Failed");
		Console->LClose();
	}
	ServerSock->settimeout(0, 10000);
	GSLiveCheck();
}

void PInfoServer::Update()
{
	if(ServerSock->newConnection())
	{
		int clid = Server->NewClient();
		if(clid!=-1)
		{
			Console->Print(GREEN, BLACK, "Infoserver: client [%i] connected", clid);
			PClient *Client = Server->GetClient(clid);

			ConnectionTCP* tcpConn = ServerSock->getTCPConnection();
			Client->setTCPConnection(tcpConn);

			Console->Print("Client address: %s", Client->GetAddress());
			//++mNumClients;

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

void PInfoServer::GSLiveCheck()
{
    MYSQL_ROW row;
    MYSQL_RES *result;
    char query[256];
    snprintf (query, 256, "SELECT *, (NOW()< (`s_lastupdate` + INTERVAL %d SECOND)) FROM `server_list`", mLivecheckInterval);

    result = MySQL->ResQuery(query);
    if(result == NULL)
    {
        Console->Print("Livecheck: %s unable to read server list!", Console->ColorText(RED, BLACK, "[Warning]"));
        MySQL->ShowSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)
    {
        Console->Print("Livecheck: %s no gameserver found!", Console->ColorText(RED, BLACK, "[Warning]"));
        MySQL->FreeSQLResult(result);
        return;
    }

    ServerMap::iterator it;
    while((row = mysql_fetch_row(result)))
    {
        it = Serverlist.find(atoi(row[s_id]));
        if(it != Serverlist.end())
        {
            strncpy(it->second.mName, row[s_name], MAX_SERVER_NAME_LENGTH);
            it->second.mIp = IPStringToDWord(row[s_addr]);
            it->second.mPort = atoi(row[s_port]);
            it->second.mPlayers = atoi(row[s_players]);
            /* Prepared for future addon Servers by Accesslevel */
            // it->second.mMinLv = atoi(row[s_minlv]);
            /* ------------------------------------------------ */

            // ToDo: If statement correct? Maybe GSLiveCheck() has
            // to be called every mLinvecheckInterval seconds.... We'll
            // see when Gameserver has been rewritten

            if(row[s_timecheck] && (atoi(row[s_timecheck]) == 1))
            {
                it->second.mLasttimestamp = atol(row[s_lastupdate]);
                it->second.mOnline = true;
            }
            else
            {
                it->second.mOnline = false;
            }

            it->second.mUpdated = true;
        }
        else
        {
            GameServers tmpServer;

            strncpy(tmpServer.mName, row[s_name], MAX_SERVER_NAME_LENGTH);
            tmpServer.mIp = IPStringToDWord(row[s_addr]);
            tmpServer.mLasttimestamp = atol(row[s_lastupdate]);
            tmpServer.mPlayers = atoi(row[s_players]);
            tmpServer.mPort = atoi(row[s_port]);
            tmpServer.mOnline = true;
            tmpServer.mUpdated = true;
            Console->Print("Added GameServer %s", tmpServer.mName);
            /* Prepared for future addon Servers by Accesslevel */
            //    tmpServer.mMinLv = atoi(row[s_minlv]);
            /* ------------------------------------------------ */

            Serverlist.insert(std::make_pair(atoi(row[s_id]), tmpServer));
        }
    }
    MySQL->FreeSQLResult(result);

	for(ServerMap::iterator it = Serverlist.begin(); it != Serverlist.end(); it++)
    {
        if(it->second.mUpdated == false)
            Serverlist.erase(it);
        else
            it->second.mUpdated = false;
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

bool PInfoServer::HandleHandshake(PInfoState *State, const u8 *Packet, int PacketSize)
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
				Console->Print(RED, BLACK, "Infoserver protocol error (IS_HANDSHAKE0): invalid packet [%04x]", *(u16*)&Packet[3]);
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
  int returnval = 0;
    // ReturnValue values:
    // 0: No error
    // -1: Wrong/Unknown username
    // -2: Wrong Password
    // -3: Malformed Auth Data. Please relog
    // -4: Database error, contact admin
    // -5: No such account, Account created. Please relog
    // -6: Could not create autoaccount, PW too short
    // -7: Could not create autoaccount, Name too short
    // -8: Could not create autoaccount, PW and Name too short
    // -9: Duplicate entry for Username! Contact Admin
    // -10: User is banned
    // -11: Insufficient access rights
    // -12: Account is not yet activated (accesslevel = 0)
    // -99: General fault. Contact admin
	ConnectionTCP *Socket = Client->getTCPConn();
	PAccount* currentAccount = NULL;
	
	if(PacketSize > 20 && *(u16*)&Packet[3]==0x8084)
	{
		const u8 *Key = &Packet[5];			// password key
		u16 ULen = *(u16*)&Packet[16];		// username length
		u16 PLen = *(u16*)&Packet[18];		// password length
		char *UserName = (char*)&Packet[20];	// account name
		const u8 *PW = &Packet[20+ULen];	// encoded password

    if(UserName[ULen-1]) // Check that string is well terminated
    {
      Console->Print("Infoserver: Client [%d]: Username was not NULL-terminated !", Client->GetIndex());
      returnval = -1;
    }
    else
    {
      currentAccount = new PAccount(UserName);
      if(!currentAccount->GetID())
      {
        if(Config->GetOptionInt("auto_accounts")) // Autoaccount
        {
          delete currentAccount;
          currentAccount = new PAccount();
          
          if(!currentAccount->SetName(UserName)) // !!! len
          {
              returnval = -7;
          }
          if(!currentAccount->SetPasswordEncoded(PW, PLen, Key))
          {
              returnval = returnval ? -8 : -6;
          }
              
          if(!returnval)
          {
            if(currentAccount->Create())
            {
              returnval = -5;
            }
            else
            {
              returnval = -4;
            }
          }
        }
        else
        {
          returnval = -1;
        }
      }
      else
      {
        if(currentAccount->Authenticate(PW, PLen, Key))
        { // Username & Password correct
          if(currentAccount->IsBanned())
          {
              returnval = -10;
          }
          else if(currentAccount->GetLevel() < Config->GetOptionInt("minlevel")) // insufficient access rights
          {
              returnval = -11;
          }
          else if(Config->GetOptionInt("require_validation") == 1 && currentAccount->GetLevel() == PAL_UNREGPLAYER)
          {
              returnval = -12;
          }
          else
          {
              Client->setAccountID(currentAccount->GetID());
              returnval = 0;
          }
  
        }
        else
        {
          returnval = -2;
        }
      }
    }
    
		bool Failed = false;
		if(returnval == 0)
		{
			u8 AUTHOK[28]={0xfe, 0x19, 0x00, 0x83, 0x81, 0x00, 0x00, 0x00, 0x00, 0x10, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
							0x00, 0x00, 0x00, 0x00, 0x00 };
			*(u32*)&AUTHOK[5] = currentAccount->GetID();
			Socket->write(AUTHOK, 28);
			State->mState = PInfoState::IS_SERVERLIST;
		}
		else
		{
			Console->Print("Infoserver: User '%s': authentication failed. Errorcode %d", UserName, returnval);
			Failed = true;	// auth failed
		}
		if(Failed == true)
		{
			std::string errorReason;
			switch(returnval)
			{
			    // It seems that the client cuts the line off after 40 chars...
//                                 |1       |10       |20       |30       |40       |50       |60       |70       |80
			    case -99:
			    {
                    //errorReason = "General fault in processing your login request";
                    errorReason = "General fault while login request";
                    break;
			    }
			    case -12:
			    {
                    //errorReason = "Account not activated. Please check your EMails";
                    errorReason = "Error: Your account is not activated";
                    break;
			    }
			    case -11:
			    {
			        //errorReason = "Login rejected. You have to be " + GetAccessString(Config->GetOptionInt("minlevel")) + " or higher";
			        errorReason = "Level " + GetAccessString(Config->GetOptionInt("minlevel")) + " or higher required";
			        break;
			    }
			    case -10:
			    {
			        errorReason = "You are banned for " + currentAccount->GetBannedTime();
			        break;
			    }
			    case -9:
			    {
			        //errorReason = "Duplicate entry for this login. Contact Admin";
			        errorReason = "Duplicate entry found. Contact Admin";
			        break;
			    }
			    case -8:
			    {
			        //errorReason = "Autoaccount failed, name and password too short";
			        errorReason = "AutoAcc failed, name and pwd too short";
			        break;
			    }
			    case -7:
			    {
			        errorReason = "Autoaccount failed, name is too short";
			        break;
			    }
			    case -6:
			    {
			        //errorReason = "Autoaccount failed, password is too short";
			        errorReason = "Autoaccount failed, pwd is too short";
			        break;
			    }
			    case -5:
			    {
			        errorReason = "New Account created, please login again";
			        break;
			    }
			    case -4:
			    {
			        errorReason = "Database error, contact Administrator";
			        break;
			    }
			    case -3:
			    {
			        errorReason = "Malformed AuthData. Please login again";
			        break;
			    }
			    case -2:
			    {
			        errorReason = "Wrong password";
			        break;
			    }
			    case -1:
			    {
			        errorReason = "Unknown username";
			        break;
			    }
			}
			u8 AUTHFAILED_HEADER[] = {0xfe, 0x0c, 0x00, 0x83, 0x86, 0x05, 0x00, 0x06, 0x00};
			u8 AUTHFAILED_FOOTER[] = {0x00, 0x40};
			*(u16*)&AUTHFAILED_HEADER[1] = errorReason.size() + 8;
			*(u16*)&AUTHFAILED_HEADER[7] = errorReason.size() + 1;
			//*(u8*)&AUTHFAILED_FOOTER[1] = {0x40};

			Socket->write(AUTHFAILED_HEADER, sizeof(AUTHFAILED_HEADER));
			Socket->write(errorReason.c_str(), errorReason.size());
			Socket->write(AUTHFAILED_FOOTER, sizeof(AUTHFAILED_FOOTER));
			FinalizeClientDelayed(Client, State);
			State->mState=PInfoState::IS_UNKNOWN;
		}
	}
	else
	{
		Console->Print(RED, BLACK, "Infoserver protocol error (IS_AUTHENTICATE): invalid packet [%04x]", *(u16*)&Packet[3]);
		return false;
	}

	return true;
}

bool PInfoServer::HandleServerList(PClient *Client, const u8 *Packet, int PacketSize)
{
    u8 SERVERLIST_HEAD[] = {0xfe, 0x00, 0x00, 0x83, 0x83, 0x01, 0x00, 0x0d, 0x00};
    u8 SERVERLIST[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    u8 SERVERLIST_FOOTER[] = {0x00};

	ConnectionTCP *Socket = Client->getTCPConn();

	u32 tID = *(u32*)&Packet[5];
	PAccount* currentAccount = new PAccount(Client->getAccountID());
	u32 aID = currentAccount->GetID();

	if(!aID || (aID != tID))
	{
	    Console->Print("%s invalid userID %d (auth with id %d)", Console->ColorText(YELLOW, BLACK, "Warning:"), tID, aID);
	    delete currentAccount;
	    return false;
	}

	if(currentAccount->GetLevel() < Config->GetOptionInt("minlevel"))
	{
	    Console->Print("%s someone tried to bypass the login process! UserID %d", Console->ColorText(RED, BLACK, "Warning:"), aID);
	    delete currentAccount;
	    return false;
	}

  if(PacketSize == 31 && *(u16*)&Packet[3]==0x8284)
	{
    GSLiveCheck(); // Perform livecheck to have up-to-date data

    int len = 0;
    int num = 0;
    for(ServerMap::iterator it = Serverlist.begin(); it != Serverlist.end(); it++)
    {
        num++;
        len += 14 + strlen(it->second.mName);
    }

    *(u16*)&SERVERLIST_HEAD[1] = len;
    *(u8*)&SERVERLIST_HEAD[5] = num;
    Socket->write(SERVERLIST_HEAD, sizeof(SERVERLIST_HEAD));

    for(ServerMap::iterator it = Serverlist.begin(); it != Serverlist.end(); it++)
    {
            /* Prepared for future addon Servers by Accesslevel */
//            if(accesslevel >= it->second.mMinLv)
//            {
            /* ------------------------------------------------ */
      *(u32*)&SERVERLIST[0] = it->second.mIp;
      *(u16*)&SERVERLIST[4] = it->second.mPort;
      *(u8*)&SERVERLIST[8] = strlen(it->second.mName) + 1;
      *(u16*)&SERVERLIST[9] = it->second.mPlayers;
      if(it->second.mOnline == true)
      {
          Console->Print("Sending server name: %s ip: %s player: %d port: %d online: yes", it->second.mName, IPlongToString(it->second.mIp), it->second.mPlayers, it->second.mPort);
          *(u16*)&SERVERLIST[11] = 1;
      }
      else if(it->second.mOnline == false)
      {
          Console->Print("Sending server name: %s ip: %s player: %d port: %d online: no", it->second.mName, IPlongToString(it->second.mIp), it->second.mPlayers, it->second.mPort);
          *(u16*)&SERVERLIST[11] = 0;
      }
      Socket->write(SERVERLIST, sizeof(SERVERLIST));
      Socket->write(it->second.mName, strlen(it->second.mName));
      Socket->write(SERVERLIST_FOOTER, sizeof(SERVERLIST_FOOTER));
            /* Prepared for future addon Servers by Accesslevel */
//            }
            /* ------------------------------------------------ */
    }
	}
	else
	{
		Console->Print(RED, BLACK, "Infoserver protocol error (IS_SERVERLIST): invalid packet [%04x]", *(u16*)&Packet[3]);
		delete currentAccount;
		return false;
	}
	delete currentAccount;
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

	ConnectionTCP *Socket = Client->getTCPConn();

	if(State->mWaitSend && Socket->getSendBufferSize()==0)
		return false;

	if(State->mState==PInfoState::IS_CONNECTED)
	{
		Socket->write(HANDSHAKE0A, 6);
		State->mState = PInfoState::IS_HANDSHAKE0;
	}

	int PacketSize=0;
	const u8 *Packet = Socket->read(&PacketSize);
	if(PacketSize > 0)
	{
		switch(State->mState)
		{
			case PInfoState::IS_HANDSHAKE0:
				return HandleHandshake(State, Packet, PacketSize);

			case PInfoState::IS_AUTHENTICATE:
				return HandleAuthenticate(Client, State, Packet, PacketSize);

			case PInfoState::IS_SERVERLIST:
				return HandleServerList(Client, Packet, PacketSize);
			default:
				break;
		}
	}
	return true;
}

void PInfoServer::FinalizeClient(PClient *Client, PInfoState *State)
{
	Console->Print(RED, BLACK, "Infoserver: client %s disconnected", Client->GetAddress());
	Client->InfoDisconnect();
	ClientStates.erase(Client);
	delete State;
}

void PInfoServer::FinalizeClientDelayed(PClient *Client, PInfoState *State)
{
	Console->Print("Infoserver: client %i is about to be disconnected", Client->GetIndex());
	State->mWaitSend = true;
}
