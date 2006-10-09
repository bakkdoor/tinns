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
  MODIFIED: 27 Jul 2006 Hammag
  REASON: - added mTransactionID member
          - re-added IncreaseUDP_ID() // but I don't understand well what SetUDP_ID() does ...
  MODIFIED: 05 Aug 2006 Hammag
  REASON: - changed TMP_UDP_PORT/SetTMPUDPPort()/GetTMPUDPPort() to mRemotePort/SetRemoteUDPPort()/GetRemoteUDPPort()
              which corresponds to the real purpose of these members
          - added GetID() as an alias os GetIndex() for better coherency with other classes
  MODIFIED: 09 Oct 2006 Hammag
  REASON: - added GetDebugMode() and SetDebugMode() methods

  
  TODO:   - check that SetUDP_ID, and the mSessionID(UDP_ID_HIGH) real use,
              and if UDP_ID and mSessionID must be synced (like in NeoX) or not

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

#define DEBUG_MODES 2
enum PDebugMode
{
  DBG_LOCATION = 0,
  DBG_ITEMID = 1,
  DBG_ALL = DEBUG_MODES // must always be last, with DEBUG_MODES updated as needed
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
		u16 mTransactionID;
		PClientLevel mLevel;
		int mConnection;
		int mRemotePort;
    
    bool mDebugMode[DEBUG_MODES];
		// new multiuser-chat implementation //
    int m_ZoneID;
		//int[4] m_IP;

	protected :
	public :
		PClient(int Index);
		~PClient();

    inline const bool GetDebugMode(PDebugMode nDebugID) { return mDebugMode[nDebugID]; }
    void SetDebugMode(PDebugMode nDebugID, bool nVal = true);
    
		inline int GetIndex() const { return mIndex; } // better use GetID()
		inline int GetID() const { return mIndex; } // for better coherency with other classes
		inline int GetLocalID() const { return mIndex + 1; }
		inline u32 GetCharID() const { return mCharID; }
		PChar* GetChar() const;
		inline u16 GetUDP_ID() const { return mUDP_ID; }
		inline u16 GetSessionID() const { return 37917 + mUDP_ID ; }
		inline u16 GetTransactionID() {return mTransactionID; }
		inline int GetRemoteUDPPort() const { return mRemotePort; } // Temp solution

		inline void SetRemoteUDPPort(int port) { mRemotePort = port; } // Temp solution
		inline void SetCharID(int id) { mCharID=id; }//NEW added
		inline void SetUDP_ID(int id) { if (mUDP_ID == 0xffff) { mUDP_ID = 0; } else { mUDP_ID = id; } }
		inline void IncreaseUDP_ID() { SetUDP_ID(mUDP_ID + 1); }
    inline void ResetTransactionID() { mTransactionID = 10170; }
    inline void IncreaseTransactionID(u8 nInc = 1) { mTransactionID += nInc; }
    
		inline PClientLevel GetLevel() const { return mLevel; }

    inline void setTCPConnection(ConnectionTCP* conn) { m_TCPConnection = conn; m_UDPConnection = 0; mConnection = PCC_GAME;  }
    inline void setUDPConnection(ConnectionUDP* conn) { m_UDPConnection = conn; }

    inline ConnectionTCP* getTCPConn() { return m_TCPConnection; }
    inline ConnectionUDP* getUDPConn() { return m_UDPConnection; }

		inline int GetConnection() const { return mConnection; }
		inline const char *GetAddress() const { return m_TCPConnection->getRemoteAddress(); }
		inline PAccount *GetAccount() const { return mAccount; }

		void GameDisconnect();

		void LoggedIn(PAccount *Account);
		void Update();

		// new multiuser-chat implementation //
		inline int getZoneID() const { return m_ZoneID; } // example: canyon 650 (for local-channel...every client with same AreaID get the chatmsg)
		//inline int*	getIP() const { return (int*) m_IP; }
};

#endif

