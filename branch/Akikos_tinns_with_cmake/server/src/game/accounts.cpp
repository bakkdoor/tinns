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
	accounts.cpp

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 26 Dec 2005 Namikon
	REASON: - Added SQLLoad to PAccounts. First step in adding MySQL support to accounts
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Changed FmtTxt() to sprintf(). It does... uhm, the same :D
	MODIFIED: 06 Jan 2006 Namikon
	REASON: - Removed the old XML loading functions, and changed the SQL ones to work with the Global Neopolis/TinNS Database
		- Added SetBannedStatus(<unix timestamp>) to ban/unban an account (use SetBannedStatus(0) to unban a player)
	MODIFIED: 03 Oct 2006 Hammag
	REASON: - Fixed an issue in PAccount::SetBannedStatus() that was causing the "can't update banned status" error message.

	MODIFIED: 27 May 2007 Hammag
	REASON: - Full changes for on-demand account access (no more memory-resident account data)
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new build system
		- clean ups
*/


/*
NOTE ABOUT ACCESS LEVELS IN THE MYSQL DATABASE:
a_priv:
0 = unregistered user
1 = Registered user
30 = volunteer
50 = GM
100 = Admin

a_status:
0 = Offline
1 = Online
2 = Banned
*/
#include "main.h"
#include "include/accounts.h"

/** Static members **/
RegEx* PAccount::mUsernameRegexFilter = NULL;
RegEx* PAccount::mPasswordRegexFilter = NULL;

bool PAccount::SetUsernameRegexFilter(const char* RegexStr)
{
  if(mUsernameRegexFilter)
  {
    delete mUsernameRegexFilter;
    mUsernameRegexFilter = NULL;
  }
  
  if(RegexStr)
  {
    try {
      mUsernameRegexFilter = new RegEx(RegexStr, PCRE_CASELESS);
    }
    catch (...) {
      return false;
    }
  }
  return true;
}

bool PAccount::SetPasswordRegexFilter(const char* RegexStr)
{
  if(mPasswordRegexFilter)
  {
    delete mPasswordRegexFilter;
    mPasswordRegexFilter = NULL;
  }
  
  if(RegexStr)
  {
    try {
      mPasswordRegexFilter = new RegEx(RegexStr, PCRE_CASELESS);
    }
    catch (...) {
      return false;
    }
  }
  return true;
}

bool PAccount::IsUsernameWellFormed(const char *Username)
{
  if(mUsernameRegexFilter)
  {
    return mUsernameRegexFilter->Search(Username);
  }
  else
    return true;
}

bool PAccount::IsPasswordWellFormed(const char *Password)
{
  if(mPasswordRegexFilter)
  {
    return mPasswordRegexFilter->Search(Password);
  }
  else
    return true;
}

/** Instance members **/	    
PAccount::PAccount()
{
	mID = 0;
	mLevel = PAL_BANNED;
  mStatus = PAS_OFFLINE;
  mBannedUntil = 0;
}

PAccount::PAccount(const u32 AccountId)
{
  char query[256];
  mID = 0;
  snprintf(query, 256, "SELECT * FROM accounts WHERE a_id = %d LIMIT 1;", AccountId);
  LoadFromQuery(query);
}

PAccount::PAccount(const char *Username)
{
  char query[256];
  mID = 0;
  if(IsUsernameWellFormed(Username)) {
    char escUsername[256];
    MySQL->EscapeString(Username, escUsername, 256);
    snprintf(query, 256, "SELECT * FROM accounts WHERE a_username = '%s' LIMIT 1;", escUsername);
    LoadFromQuery(query);
  }
}

bool PAccount::LoadFromQuery(char* query)
{
  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;
  
  bool FinalResult = false;

  result = MySQL->InfoResQuery(query);
  if(result == NULL)
  {
      Console->Print(RED, BLACK, "Failed to load AccountData from SQL");
      MySQL->ShowInfoSQLError();
      return false;
  }

  if((row = mysql_fetch_row(result)))
  {
    mID = atoi(row[a_id]);
    mName = row[a_username];
    mPassword = row[a_password];

    mBannedUntil = atoi(row[a_bandate]);
    if(mBannedUntil > time(NULL))
    {
      mStatus = PAS_BANNED;
      mLevel = PAL_BANNED;
    }
    else
    {
      mStatus = PAS_OFFLINE;
      mLevel = atoi(row[a_priv]);
    }

    FinalResult = true;
  }
  else
  {
Console->Print(YELLOW, BLACK, "Failed to load AccountData from SQL; Nothing to load...");
  }
  
  MySQL->FreeInfoSQLResult(result);
  return FinalResult;
}

bool PAccount::SetName(const std::string &Username)
{
  if(IsUsernameWellFormed(Username.c_str()))
  {
    mName = Username;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::SetPassword(const std::string &Password)
{
  if(IsPasswordWellFormed(Password.c_str()))
  {
    mPassword = Password;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::SetPasswordEncoded(const u8* PasswordData, int PassLen, const u8* Key)
{
	char Pass[128];
	
	if(DecodePassword(PasswordData, PassLen, Key, Pass))
  {
		return SetPassword((std::string)Pass);
	}
	else
	{
		Console->Print(RED, BLACK, "[Error]: user %s : malformed auth data (size=%d)", mName.c_str(), PassLen);
		return false;
	}
}

bool PAccount::SetLevel(int newLevel)
{
  if((newLevel >= PAL_BANNED) && (newLevel <= PAL_ADMIN))
  {
    mLevel = newLevel;
    return true;
  }
  else
  {
    return false;
  }
}

std::string PAccount::GetLevelString() const
{
	switch(mLevel)
	{
		case PAL_BANNED : return "banned";
		case PAL_UNREGPLAYER : return "unregplayer";
		case PAL_REGPLAYER : return "regplayer";
		case PAL_VOLUNTEER : return "volunteer";
		case PAL_GM : return "gm";
		case PAL_ADMIN : return "admin";
	}

	return "custom";
}

bool PAccount::SetStatus(PAccountStatus Status)
{
  mStatus = Status;
  return true;
}

bool PAccount::SetBannedUntilTime(std::time_t BannedUntil)
{
  if ((BannedUntil == 0) || (BannedUntil > std::time(NULL)))
  {
    mBannedUntil = BannedUntil;
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::DecodePassword(const u8* PasswordData, int PassLen, const u8 *Key, char* ClearPassword)
{
  ClearPassword[0] = 0;
  
  if(PassLen < 128)
	{
		if(Key[0]>7) // TODO: >7 correct?
		{
			for(int i=0; i<PassLen; i+=2)
				ClearPassword[i>>1] = (char)(((PasswordData[i]&0xf0)>>4)
					+((PasswordData[i+1]&0x0f)<<4)-Key[0]);
			ClearPassword[PassLen>>1]=0;
		}
		else
		{
			for(int i=0; i<PassLen; i++)
				ClearPassword[i] = (char)(PasswordData[i]-Key[0]);
			ClearPassword[PassLen]=0;
		}
		return true;
	}
	else
	  return false;
}

bool PAccount::Authenticate(const u8* PasswordData, int PassLen, const u8 *Key)
{
	char Pass[128];
	
	if(DecodePassword(PasswordData, PassLen, Key, Pass))
  {
		return Authenticate(Pass);
	}
	else
	{
		Console->Print(RED, BLACK, "[Error]: user %s : malformed auth data (size=%d)", mName.c_str(), PassLen);
		return false;
	}
}

bool PAccount::Authenticate(const char *Password) const
{
	if(mID == 0) // User doesn't exist and that hasn't been checked !
	{
	  Console->Print(RED, BLACK, "[Bug]: user %s doesn't exist and was not checked by code !", mName.c_str());
	  return false;
	}
	
	return(mPassword == Password);
}

bool PAccount::Create()
{
  if(Save(true)) {
    mID = MySQL->GetLastInfoInsertId();
    return true;
  }
  else
  {
    return false;
  }
}

bool PAccount::Save(bool CreateMode)
{
  char escUsername[256];
  char escPassword[256];
  MySQL->EscapeString(mName.c_str(), escUsername, 256);
  MySQL->EscapeString(mPassword.c_str(), escPassword, 256);
    
  std::string Query;
  Query = CreateMode ? "INSERT INTO" : "UPDATE";
  Query += "accounts SET ";
  Query += Ssprintf(" accounts SET a_username='%s', a_password = '%s'", escUsername, escPassword);
  Query += Ssprintf(", a_priv = %d, a_status = %d, a_bandate = %d", mLevel, mStatus, mBannedUntil);
  if(!CreateMode )
  {
    Query += Ssprintf(" WHERE a_id = %d LIMIT 1", mID);
  }

  if(MySQL->InfoQuery(Query.c_str()))
  {
      Console->Print(RED, BLACK, "[Error] Failed to %s account %s (id %d)", CreateMode ? "create" : "update", mName.c_str(), mID);
      MySQL->ShowInfoSQLError();
      return false;
  }
  return true;
}

u32 PAccount::GetCharIdBySlot(const u32 SlotId)
{
  char query[256];
  u32 CharId = 0;
  
  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;
  
  snprintf(query, 256, "SELECT c_id FROM characters WHERE a_id = %d AND c_slot = %d LIMIT 1;", mID, SlotId);  

  result = MySQL->GameResQuery(query);
  if(result == NULL)
  {
      Console->Print(RED, BLACK, "Failed to load CharacterData from SQL");
      MySQL->ShowGameSQLError();
      return 0;
  }

  if((row = mysql_fetch_row(result)))
  {
    CharId = atoi(row[0]);
  }
  
  MySQL->FreeGameSQLResult(result);
  
  /*** Temporary workaround to cope with DB where c_slot is not set ***/
  if(!CharId)
  {
    snprintf(query, 256, "SELECT c_id FROM characters WHERE a_id = %d ORDER BY c_slot ASC, c_id ASC LIMIT %d, 1;", mID, SlotId);
  
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "Failed to load CharacterData from SQL");
        MySQL->ShowGameSQLError();
        return 0;
    }
    
    if((row = mysql_fetch_row(result)))
    {
      CharId = atoi(row[0]);
    }
    
    MySQL->FreeGameSQLResult(result);    
  }
  /*** End of workaround ***/
  
  return CharId;
}


std::string PAccount::GetBannedTime() const
{
  const char* unit[5] = {"seconds", "minutes", "hours", "days", "weeks"};
  
  std::time_t timediff = mBannedUntil - std::time(NULL);
  if(timediff <=0)
  {
    return "0 more seconds. Please relog";
  }

  long counter;
  int type;

  if(timediff > 86400)   // days
  {
    counter = timediff / 86400;
    type = 3;
  }
  else if(timediff > 3600)    // hours
  {
    counter = timediff / 3600;
    type = 2;
  }
  else if(timediff > 60)      // Minutes
  {
    counter  = timediff / 60;
    type = 1;
  }
  else      // Seconds
  {
    counter = timediff;
    type = 0;
  }

  return Ssprintf("%d more %s", counter, unit[type]);
}
