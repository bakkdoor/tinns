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

#include "main.h"

PAccounts::PAccounts()
{
}

PAccounts::~PAccounts()
{
}

int PAccounts::Authenticate(const char *User, const u8 *Password, int PassLen, const u8 *Key, int *accID)
{
    char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int RetVal = -99;
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
	char Pass[128];
	Pass[0]=0;
	if(PassLen < 128)
	{
		if(Key[0]>7) // TODO: >7 correct?
		{
			for(int i=0; i<PassLen; i+=2)
				Pass[i>>1] = (char)(((Password[i]&0xf0)>>4)
					+((Password[i+1]&0x0f)<<4)-Key[0]);
			Pass[PassLen>>1]=0;
		}
		else
		{
			for(int i=0; i<PassLen; i++)
				Pass[i] = (char)(Password[i]-Key[0]);
			Pass[PassLen]=0;
		}

    sprintf(query, "SELECT * FROM accounts WHERE a_username = '%s'", User);
    result = MySQL->ResQuery(query);
    if(result == NULL) // SQL Error
    {
        Console->Print("MySQL Error, unable to execute Query %s", query);
        MySQL->ShowSQLError();
        return -4;
    }
    if(mysql_num_rows(result) == 0) // No Account found. Autoaccount or Donothing
    {
        if(Config->GetOptionInt("auto_accounts")) // Autoaccount
        {
            if(std::strlen(User) < 3 && std::strlen(Pass) < 4)
            {
                RetVal = -8;
            }
            else if(std::strlen(User) < 3)
            {
                RetVal = -7;
            }
            else if(std::strlen(Pass) < 4)
            {
                RetVal = -6;
            }
            else
            {
              sprintf(query, "INSERT INTO accounts (a_username, a_password, a_priv, a_status) VALUES ('%s', '%s', %d, %d)", User, Pass, 0, 0);
              if(MySQL->Query(query))
              {
                  Console->Print("MySQL Error, unable to execute Query %s", query);
                  MySQL->ShowSQLError();
                  RetVal = -4;
              }
              else
              {
                  RetVal = -5;
              }
            }
        }
        else
        {
            RetVal = -1;
        }
    }
    else if(mysql_num_rows(result) > 1) // Duplicate userentry error
    {
        RetVal = -9;
    }
    else if(mysql_num_rows(result) == 1) // Found account
    {
        row = mysql_fetch_row(result);
        if(strcmp(row[a_password], Pass) != 0) // Username correct, password wrong
        {
            RetVal = -2;
        }
        else // Username & Password correct
        {
            if(atoi(row[a_status]) == 2)
            {
                RetVal = -10;
            }
            else
            {
                if(Config->GetOptionInt("minlevel") > atoi(row[a_priv])) // insufficient access rights
                {
                    RetVal = -11;
                }
                else if(Config->GetOptionInt("require_validation") == 1 && atoi(row[a_priv]) == 0)
                {
                    RetVal = -12;
                }
                else
                {
                    *accID = atoi(row[a_id]);
                    RetVal = 0;
                }
            }
        }
    }
    MySQL->FreeSQLResult(result);
	}
	else
	{
		Console->Print("Accounts: malformed auth data");
        RetVal = -3;
	}

	return RetVal;
}

int PAccounts::GetAccesslevel(int AccountID)
{
    MYSQL_RES *result = 0;
    MYSQL_ROW row = 0;
    char query[255];

    sprintf(query, "SELECT a_priv FROM accounts WHERE a_id = %d LIMIT 1", AccountID);
    result = MySQL->ResQuery(query);
    if(result == NULL)
        return -100;

    row = mysql_fetch_row(result);
    MySQL->FreeSQLResult(result);
    return atoi(row[0]);
}

std::string PAccounts::GetBannedTime(const char *username)
{
    MYSQL_RES *result;
    MYSQL_ROW row = 0;
    char query[255];
    long banneduntil = 0;
    unsigned long timediff = 0;

    sprintf(query, "SELECT a_bandate FROM accounts WHERE a_username = '%s' LIMIT 1", username);
    result = MySQL->ResQuery(query);
    if(result == NULL)
    {
        Console->Print("%s cant get bandate from sql!", Console->ColorText(RED, BLACK, "Error"));
        MySQL->ShowSQLError();
        return "DBERROR";
    }
    banneduntil = atol(row[0]);
    MySQL->FreeSQLResult(result);

    if(banneduntil < time(NULL))
    {
        return "0 more seconds. Please relog";
    }
    timediff = banneduntil - time(NULL);

    unsigned long tmpcalc = 0;
    int counter = 0;
    int type = 0;

    tmpcalc = timediff;

    if(timediff > 604800)       // weeks
    {
        while(tmpcalc > 604800)
        {
            tmpcalc -= 604800;
            counter++;
        }
        type = 5;
    }
    else if(timediff > 86400)   // days
    {
        while(tmpcalc > 86400)
        {
            tmpcalc -= 86400;
            counter++;
        }
        type = 4;
    }
    else if(timediff > 3600)    // hours
    {
        while(tmpcalc > 3600)
        {
            tmpcalc -= 3600;
            counter++;
        }
        type = 3;
    }
    else if(timediff > 60)      // Minutes
    {
        while(tmpcalc > 60)
        {
            tmpcalc -= 60;
            counter++;
        }
        type = 2;
    }
    else if(timediff < 61)      // Seconds
    {
        counter = timediff;
        type = 1;
    }

    char buffer[15];
    sprintf(buffer, "%d", counter);
    std::string rbuf(buffer);

    if(type == 1)
        return rbuf + " more seconds";
    else if(type == 2)
        return rbuf + " more minutes";
    else if(type == 3)
        return rbuf + " more hours";
    else if(type == 4)
        return rbuf + " more days";
    else if(type == 5)
        return rbuf + " more weeks";
    return "ERROR";
}
