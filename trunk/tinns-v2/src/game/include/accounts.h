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
	accounts.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 26 Dec 2005 Namikon
	REASON: - Added load/save functions for SQL DB
	MODIFIED: 01 Jan 2005 Namikon
	REASON: - Added bool var for ingame debug outputs for administrators
	MODIFIED: 06 Jan 2005 Namikon
	REASON: - Added SetBannedStatus(<unix timestamp>) to ban/unban an account (use SetBannedStatus(0) to unban a player)
*/

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#ifdef _MSC_VER
	#pragma once
#endif
/*
0 = unregistered user
1 = Registered user
30 = volunteer
50 = GM
100 = Admin
*/

// New way of AccountLevel handling:
// Every level is possible, the following values are only edge values. We need a bit control about that
#define PAL_BANNED -1
#define PAL_UNREGPLAYER 0
#define PAL_REGPLAYER 1
#define PAL_VOLUNTEER 30
#define PAL_GM 50
#define PAL_ADMIN 100

/*enum PAccountLevel
{
	PAL_BANNED = -1,
	PAL_UNREGPLAYER = 0,
	PAL_REGPLAYER = 1,
	PAL_VOLUNTEER = 30,
	PAL_GM = 50,
	PAL_ADMIN = 100
};*/

/*
0 = Offline
1 = Online
2 = Banned
*/

enum PAccountStatus
{
    PAS_OFFLINE = 0,
    PAS_ONLINE = 1,
    PAS_BANNED = 2
};

class PAccount
{
	private :
        // SQL Layout
        enum {
            a_id,
            a_username,
            a_password,
            a_priv,
            a_status,
            a_bandate
        };
		enum { MAXCHARSPERACC = 4 };
		u32 mID;
		std::string mName;
		std::string mPassword;

		int mLevel;
		PAccountStatus mStatus;

		u32 mChars[MAXCHARSPERACC];
		bool mConsoleAllowed;
		bool mDirty;
		bool mAdminDebug;
		bool mShunned;

	protected :
		friend class PAccounts;
		void SetID(u32 ID);
		void SetName(const std::string &Name);
		void SetPassword(const std::string &Pass);

		//void SetLevel(PAccountLevel Level);
		void SetStatus(PAccountStatus Status);

		void SetConsoleAllowed(bool Allowed);
		inline void SetDirtyFlag() { mDirty = true; }
		inline void ClearDirtyFlag() { mDirty = false; }
	public :
		PAccount();

		inline u32 GetID() const { return mID; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetPassword() const { return mPassword; }

		inline void SetShun(bool val) { mShunned = val; };
		inline bool IsShunned() { return mShunned; };

		inline int GetLevel() const { return mLevel; }
		void SetLevel(int newLevel);

		inline PAccountStatus GetStatus() const { return mStatus; }

		std::string GetLevelString() const;
		inline bool IsConsoleAllowed() const { return mConsoleAllowed; }
		inline bool IsDirty() const { return mDirty; }

		//inline bool IsAdminDebug() const { return mAdminDebug; }
		//inline void SetAdminDebug( bool ShowDebugMessages ) { mAdminDebug = ShowDebugMessages; }

		void SetBannedStatus(int banneduntil);

		bool AddChar(u32 CharID);
		void RemoveChar(u32 CharID);
		void RemoveChar(int CharIndex);
		u32 GetChar(int Num) const;

		/*void Save(TiXmlElement *Node);*/
		void SQLSave();
};

class PAccounts
{
	private :
        // SQL Layout
        enum {
            a_id,
            a_username,
            a_password,
            a_priv,
            a_status,
            a_bandate
        };
		typedef std::map<u32, PAccount*> AccountMap;
		AccountMap mAccounts;
		u32 mLastID;
	public :
		PAccounts();
		~PAccounts();

		//PAccount *Authenticate(const char *User, const u8* Password, int PassLen, const u8 *Key, bool UseAutoAccounts=false);
		PAccount *Authenticate(const char *User, const u8* Password, int PassLen, const u8 *Key);
		PAccount *Authenticate(const char *User, const char *Password) const;
		//void Load();
		bool SQLLoad();
		void RehashAccountData();
		//void Update();
        void SQLUpdate();

		PAccount *GetAccount(u32 AccountID) const;
		PAccount *GetAccount(const std::string &Name) const;
		//PAccount *CreateAccount(const std::string &Name, const std::string &Password);
};

#endif

