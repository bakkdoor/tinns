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

PAccount::PAccount()
{
	mID = 0;
	for(int i=0; i<MAXCHARSPERACC; i++)
		mChars[i] = 0;
	mLevel = PAL_BANNED;
	mConsoleAllowed = false;
	mDirty = false;
	mAdminDebug = false;
}

bool PAccount::AddChar(u32 CharID)
{
	for(int i=0; i<MAXCHARSPERACC; i++)
	{
		if(mChars[i]==0)
		{
			mChars[i]=CharID;
if (gDevDebug) Console->Print("Added char %d to in-mem account %d", CharID, mID);
			return true;
		}
	}

	return false;
}

void PAccount::RemoveChar(u32 CharID)
{
	for(int i=0; i<MAXCHARSPERACC; i++)
	{
		if(mChars[i]==CharID)
		{
			mChars[i]=0;
			break;
		}
	}
}

void PAccount::RemoveChar(int CharIndex)
{
	if(CharIndex < 0 || CharIndex >= MAXCHARSPERACC)
		return;

	mChars[CharIndex]=0;
}

u32 PAccount::GetChar(int Num) const
{
	if(Num < 0 || Num >= MAXCHARSPERACC)
		return 0;

	return mChars[Num];
}

void PAccount::SetID(u32 ID)
{
	if(ID != mID)
		SetDirtyFlag();
	mID = ID;
}

void PAccount::SetName(const std::string &Name)
{
	if(/*std::*/strcasecmp(mName.c_str(), Name.c_str()))
		SetDirtyFlag();

	mName = Name;
}

void PAccount::SetPassword(const std::string &Pass)
{
	if(mPassword != Pass)
		SetDirtyFlag();
	mPassword = Pass;
}

void PAccount::SetLevel(PAccountLevel Level)
{
	if(mLevel != Level)
		SetDirtyFlag();

	mLevel = Level;
}

void PAccount::SetStatus(PAccountStatus Status)
{
    if(mStatus != Status)
        SetDirtyFlag();

    mStatus = Status;
}

void PAccount::SetConsoleAllowed(bool Allowed)
{
	if(Allowed != mConsoleAllowed)
		SetDirtyFlag();

	mConsoleAllowed = Allowed;
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

	return "banned";
}

/*void PAccount::Save(TiXmlElement *Node)
{
	if(!Node)
		return;

	Node->SetAttribute("name", GetName().c_str());
	Node->SetAttribute("password", GetPassword().c_str());
	Node->SetAttribute("id", GetID());
	Node->SetAttribute("level", GetLevelString().c_str());
	Node->SetAttribute("console", IsConsoleAllowed() ? "1" : "0");

	ClearDirtyFlag();
}*/

void PAccount::SQLSave()
{
    char query[1024];
    sprintf(query, "UPDATE accounts SET a_username = '%s', a_password = '%s', a_priv = %d, a_status = %d WHERE a_id = %d LIMIT 1;", GetName().c_str(), GetPassword().c_str(), mLevel, mStatus, GetID());
    if(MySQL->InfoQuery(query))
    {
        //Console->Print("Failed to update Account %d", GetID());
        MySQL->ShowInfoSQLError();
        return;
    }
}

void PAccount::SetBannedStatus(int banneduntil)
{
  int status;
  char query[255];
  
  if(banneduntil == 0)
  {
    status = 0;
  }
  else
  {
    status = 2;
  }

  sprintf(query, "UPDATE accounts SET a_status = %d, a_bandate = %d WHERE a_id = %d", status, banneduntil, mID);
  if(MySQL->InfoQuery(query))
  {
      Console->Print(RED, BLACK, "Error, cant update banned status for Account %d", mID);
  }
}

// ===========================================================================

PAccounts::PAccounts()
{
	mLastID = 1;
}

PAccounts::~PAccounts()
{
	SQLUpdate();	// just to be sure
	for(AccountMap::iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
		delete i->second;
}

bool PAccounts::SQLLoad()
{
  Console->Print("Loading accounts from MySQL database...");
    MYSQL_ROW row = 0;
    MYSQL_RES *result = 0;
    int nAcc=0;
    bool HasAdminAcc = false;

    result = MySQL->InfoResQuery("SELECT * FROM accounts");
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "Failed to load AccountData from SQL");
        MySQL->ShowInfoSQLError();
        return false;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print(RED, BLACK, "Failed to load AccountData from SQL; Nothing to load...");
        return false;
    }

    while((row = mysql_fetch_row(result)))
    {
        PAccount *info = new PAccount();
        info->SetID(std::atoi(row[a_id]));
        info->SetName(row[a_username]);
        info->SetPassword(row[a_password]);
//        info->SetLevel(row[3]);

        //int a_status = std::atoi(row[a_status]);
        int a_status = 0;
        bool IsStillBanned = false;

        //int banneduntil = std::atoi(row[a_bandate]);
        int banneduntil = 0;
        int now = time(0);

        if(now < banneduntil)
        {
            IsStillBanned = true;
        }
        else if(now >= banneduntil)
        {
            IsStillBanned = false;
            info->SetBannedStatus(0);
        }

        if(a_status == 2 && IsStillBanned == true)
        {
            info->SetLevel(PAL_BANNED);
            info->SetStatus(PAS_BANNED);
        }
        else
        {
            //int a_priv = std::atoi(row[a_priv]);
            int a_priv = 100;
            switch(a_priv)
            {
                case 0: info->SetLevel(PAL_UNREGPLAYER);
                case 1: info->SetLevel(PAL_REGPLAYER);
                case 30: info->SetLevel(PAL_VOLUNTEER);
                case 50: info->SetLevel(PAL_GM);
                case 100: info->SetLevel(PAL_ADMIN);
            }
        }
        info->SetStatus(PAS_OFFLINE);

        mAccounts.insert(std::make_pair(info->GetID(), info)).second;
        ++nAcc;
        mLastID = max(mLastID, info->GetID());
        if(info->GetLevel() == PAL_ADMIN)
        {
            HasAdminAcc = true;
        }
    }
    Console->Print("%s Loaded %i accounts", Console->ColorText(GREEN, BLACK, "[Success]"), nAcc);
	if(!HasAdminAcc)
	{
		Console->Print(YELLOW, BLACK, "Warning: No admin account in database");
	}

  return 0;
}

/*void PAccounts::Load()
{
	Console->Print("Loading accounts...");
	int nAcc=0;
	bool HasAdminAcc = false;
	TiXmlDocument doc("./database/accounts.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *Acc = Root->FirstChildElement("account");
			while(Acc)
			{
				PAccount *info = new PAccount();
				if(!info->Load(Acc))
				{
					Console->Print("Invalid account entry found in database. Ignored.");
					delete info;
				} else
				{
					if(!mAccounts.insert(std::make_pair(info->GetID(), info)).second)
					{
						Console->Print("Duplicate account id found: %s", info->GetName().c_str());
						delete info;
					} else
						++nAcc;
					mLastID = max(mLastID, info->GetID());
					if(info->GetLevel()==PAL_ADMIN)
						HasAdminAcc=true;
				}
				Acc=Acc->NextSiblingElement("account");
			}
		}
	}

	Console->Print("Loaded %i accounts", nAcc);
	if(!HasAdminAcc)
		Console->Print("Warning: no admin account in database");
}
*/
PAccount *PAccounts::Authenticate(const char *User, const u8 *Password, int PassLen, const u8 *Key, bool UseAutoAccounts)
{
	PAccount *Account = 0;
	char Pass[128];
	Pass[0]=0;
	PAccount *Acc = 0;
	if(PassLen < 128)
	{
		if(Key[0]>7) // TODO: >7 correct?
		{
			for(int i=0; i<PassLen; i+=2)
				Pass[i>>1] = (char)(((Password[i]&0xf0)>>4)
					+((Password[i+1]&0x0f)<<4)-Key[0]);
			Pass[PassLen>>1]=0;
		} else
		{
			for(int i=0; i<PassLen; i++)
				Pass[i] = (char)(Password[i]-Key[0]);
			Pass[PassLen]=0;
		}

		Acc = GetAccount(User);
		if(Acc)
		{
			if(Acc->GetPassword() == Pass)
				Account = Acc;
			else
				Console->Print("User %s entered wrong password", User);
		} else
			Console->Print("Unknown user %s", User);
	} else
		Console->Print("Accounts: malformed auth data");

	// auto accounts
	if(!Acc && UseAutoAccounts && Config->GetOptionInt("auto_accounts"))
	{
		if(std::strlen(User) >= 3 && std::strlen(Pass) >= 4)
		{
			Account = CreateAccount(User, Pass);
			Console->Print("Account %s created", User);
		} else
			Console->Print("Could not autocreate account: user name or password too short");
	}

	return Account;
}

PAccount *PAccounts::Authenticate(const char *User, const char *Password) const
{
	PAccount *Account = 0;
	PAccount *Acc = GetAccount(User);
	if(Acc)
	{
		if(Acc->GetPassword() == Password)
			Account = Acc;
		else
			Console->Print("User %s entered wrong password", User);
	} else
		Console->Print("Unknown user %s", User);

	return Account;
}

PAccount *PAccounts::GetAccount(u32 AccountID) const
{
	PAccount *Result = 0;
	AccountMap::const_iterator i = mAccounts.find(AccountID);
	if(i != mAccounts.end())
		Result = i->second;

	return Result;
}

PAccount *PAccounts::GetAccount(const std::string &Name) const
{
	PAccount *Result = 0;
	for(AccountMap::const_iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
	{
		if(!/*std::*/strcasecmp(i->second->GetName().c_str(), Name.c_str()))
		{
			Result = i->second;
			break;
		}
	}
	return Result;
}

void PAccounts::SQLUpdate()
{
    Console->Print("Updating account database...");
    MYSQL_ROW row;
    MYSQL_RES *result;
    int NumUpd=0;
//    int nAcc=0;
//    bool HasAdminAcc = false;
    int id = 0;

    result = MySQL->InfoResQuery("SELECT * FROM accounts");
    if(result == NULL) {
        Console->Print(RED, BLACK, "Failed to update AccountData from SQL");
        return;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print(RED, BLACK, "Failed to update AccountData from SQL; No Account Data!");
        return;
    }

    while((row = mysql_fetch_row(result)))
    {
        id = std::atoi(row[a_id]);
        AccountMap::const_iterator i = mAccounts.find(id);
        if(i!=mAccounts.end() && i->second->IsDirty())
        {
            i->second->SQLSave();
            ++NumUpd;
        }
    }
    Console->Print("Update done, %i accounts updated", NumUpd);
}

/*void PAccounts::Update()
{
	Console->Print("Updating account database...");
	TiXmlDocument doc("./database/accounts.xml");
	int NumUpd=0;
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			// update already exisiting accounts first
			TiXmlElement *e = Root->FirstChildElement("account");
			while(e)
			{
				int id;
				e->Attribute("id", &id);
				u32 id32 = static_cast<u32>(id);

				// WARNING: do not supply functions to change an account id from outside
				// because this piece of code won't work anymore!
				AccountMap::const_iterator i = mAccounts.find(id32);
				if(i!=mAccounts.end() && i->second->IsDirty())
				{
					i->second->Save(e);
					++NumUpd;
				}

				e = e->NextSiblingElement("account");
			}

			// now add unsaved accounts
			for(AccountMap::iterator i=mAccounts.begin(); i!=mAccounts.end(); i++)
			{
				PAccount *acc = i->second;
				if(acc->IsDirty())
				{
					TiXmlElement elem("account");
					acc->Save(&elem);
					Root->InsertEndChild(elem);
					++NumUpd;
				}
			}

			if(!doc.SaveFile())
				Console->Print("PAccounts::Update: could not save account database");
		}
	} else
	{
		Console->Print("PAccounts::Update: could not load account database");
	}

	Console->Print("Update done, %i accounts updated", NumUpd);
}
*/

PAccount *PAccounts::CreateAccount(const std::string &Name, const std::string &Password)
{
	PAccount *Acc = GetAccount(Name);
	if(Acc)
	{
		Console->Print("Accounts: account name %s already exists", Name.c_str());
		return 0;
	}

	Acc = new PAccount();
	Acc->SetID(++mLastID);
	Acc->SetName(Name);
	Acc->SetPassword(Password);
	Acc->SetLevel(PAL_UNREGPLAYER);
	Acc->SetConsoleAllowed(false);

	mAccounts.insert(std::make_pair(Acc->GetID(), Acc));

	SQLUpdate();
	return Acc;
}
