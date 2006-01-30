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
	PCC_PATCH = 1,
	PCC_INFO = 2,
	PCC_GAME = 4
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
		NLaddress mAddress;
		char mAddrString[24];
		PAccount *mAccount;

		PGameSocket *mPatchSocket;
		PGameSocket *mInfoSocket;
		PGameSocket *mGameSocket;
		PSocket *mUDPSocket;
		NLaddress mLocalUDPAddr;
		NLaddress mRemoteUDPAddr;
		int mIndex;
		u32 mCharID;
		u16 mUDP_ID;
		u16 mSessionID;
		PClientLevel mLevel;
		int mConnection;

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

		inline void SetCharID(int id) { mCharID=id; }//NEW added
		inline void SetUDP_ID(int id) { if (mUDP_ID == 0xffff) { mUDP_ID = 0; } else { mUDP_ID = id; } }
		inline void IncreaseUDP_ID()
		{
		    mUDP_ID++;
		    mSessionID = 37917 + mUDP_ID;
        };

		inline PClientLevel GetLevel() const { return mLevel; }
		inline PGameSocket *GetPatchSocket() const { return mPatchSocket; }
		inline PGameSocket *GetInfoSocket() const { return mInfoSocket; }
		inline PGameSocket *GetGameSocket() const { return mGameSocket; }
		inline PSocket *GetUDPSocket() { return mUDPSocket; }
		inline int GetConnection() const { return mConnection; }
		inline const char *GetAddress() const { return mAddrString; }
		inline PAccount *GetAccount() const { return mAccount; }

		bool PatchConnect(const NLsocket &Socket);
		void PatchDisconnect();

		bool InfoConnect(const NLsocket &Socket);
		void InfoDisconnect();

		bool GameConnect(const NLsocket &Socket);
		void GameDisconnect();
		void SetRemoteUDPAddr(u32 Addr, u16 Port);
		u16 OpenUDP();

		void LoggedIn(PAccount *Account);
		void Update();

		// new multiuser-chat implementation //
		inline int getZoneID() const { return m_ZoneID; } // example: canyon 650 (for local-channel...every client with same AreaID get the chatmsg)
		//inline int*	getIP() const { return (int*) m_IP; }
};

#endif

