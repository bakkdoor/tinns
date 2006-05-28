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
	client.h

	Authors:
	- v00d00
	- Akiko
	- Namikon
	- bakkdoor

	MODIFIED: 30 Nov 2005 Akiko
	REASON: - added GPL
		- added stuff from Namikon

    MODIFIED: 13 Dec 2005 bakkdoor
    REASON: - added multiuser chat
*/

#ifndef CLIENT_H
#define CLIENT_H

enum PClientConnection
{
	PCC_NONE = 0,
	PCC_GAME = 1
};

enum PClientLevel
{
	PCL_BANNED = -1,
	PCL_UNREGPLAYER = 0,
	PCL_REGPLAYER = 1,
	PCL_VOLUNTEER = 30,
	PCL_GM = 50,
	PCL_ADMIN = 100
};

class PClient
{
	private :
        ConnectionTCP* m_TCPConnection;
		ConnectionUDP* m_UDPConnection;

		PAccount *mAccount;
		int mIndex;
		u32 mCharID;
		u16 mUDP_ID;
		u16 mSessionID;
		PClientLevel mLevel;
		int mConnection;
		int TMP_UDP_PORT;

		// new multiuser-chat implementation //
        int m_ZoneID;
		//int[4] m_IP;

	protected :
	public :
		PClient(int Index);
		~PClient();

		inline int GetIndex() const { return mIndex; }
		inline u32 GetCharID() const { return mCharID; }
		inline u16 GetUDP_ID() const { return mUDP_ID; }
		inline u16 GetSessionID() const { return 37917 + mUDP_ID; }
		inline int GetTMPUDPPort() const { return TMP_UDP_PORT; } // Temp solution

		inline void SetTMPUDPPort(int port) { TMP_UDP_PORT = port; } // Temp solution
		inline void SetCharID(int id) { mCharID=id; }//NEW added
		inline void SetUDP_ID(int id) { if (mUDP_ID == 0xffff) { mUDP_ID = 0; } else { mUDP_ID = id; } }
		inline void IncreaseUDP_ID()
		{
		    mUDP_ID++;
		    mSessionID = 37917 + mUDP_ID;
        };

		inline PClientLevel GetLevel() const { return mLevel; }

        inline void setTCPConnection(ConnectionTCP* conn) { m_TCPConnection = conn; m_UDPConnection = 0; mConnection = PCC_GAME;  }
        inline void setUDPConnection(ConnectionUDP* conn) { m_UDPConnection = conn; }

        inline ConnectionTCP* getTCPConn() { return m_TCPConnection; }
        inline ConnectionUDP* getUDPConn() { return m_UDPConnection; }

		inline int GetConnection() const { return mConnection; }
		inline const char *GetAddress() const { return m_TCPConnection->getLocalAddress(); }
		inline PAccount *GetAccount() const { return mAccount; }

		void GameDisconnect();

		void LoggedIn(PAccount *Account);
		void Update();

		// new multiuser-chat implementation //
		inline int getZoneID() const { return m_ZoneID; } // example: canyon 650 (for local-channel...every client with same AreaID get the chatmsg)
		//inline int*	getIP() const { return (int*) m_IP; }
};

#endif

