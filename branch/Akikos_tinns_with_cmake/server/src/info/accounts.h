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
	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#ifdef _MSC_VER
	#pragma once
#endif

#include "include/regex++.h"

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

// Max number of char slots per account
#define MAX_CHARS_PER_ACCOUNT  4

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
    
    // static members
	  static RegEx* mUsernameRegexFilter;
	  static RegEx* mPasswordRegexFilter;
	  
	  // instance members
	  u32 mID;
	  std::string mName;
	  std::string mPassword;
    int mLevel;
    PAccountStatus mStatus;
    std::time_t mBannedUntil;

    bool LoadFromQuery(char* query);
    bool DecodePassword(const u8* PasswordData, int PassLen, const u8 *Key, char* ClearPassword);
    
	public :
	  PAccount();
	  PAccount(const u32 AccountId);
	  PAccount(const char *Username);
	  
	  static bool SetUsernameRegexFilter(const char* RegexStr);
		static bool SetPasswordRegexFilter(const char* RegexStr);
		static bool IsUsernameWellFormed(const char *Username);
		static bool IsPasswordWellFormed(const char *Password);
		
		inline u32 GetID() const { return mID; }
		bool SetName(const std::string &Pass);
		inline const std::string &GetName() const { return mName; }
		bool SetPassword(const std::string &Pass);
		bool SetPasswordEncoded(const u8* PasswordData, int PassLen, const u8* Key);
		inline const std::string &GetPassword() const { return mPassword; }
    bool SetLevel(int newLevel);
		inline int GetLevel() const { return mLevel; }
		std::string GetLevelString() const;
    bool SetStatus(PAccountStatus Status);
		inline PAccountStatus GetStatus() const { return mStatus; }
		bool SetBannedUntilTime(std::time_t BannedUntil);
    inline bool IsBanned() const { return (mBannedUntil > std::time(NULL)); }
    std::string GetBannedTime() const;

		bool Authenticate(const u8* PasswordData, int PassLen, const u8 *Key);
		bool Authenticate(const char *Password) const;
		    
		bool Create();
    bool Save(bool CreateMode = false);

    //u32 GetCharIdBySlot(const u32 SlotId);
};

#endif

