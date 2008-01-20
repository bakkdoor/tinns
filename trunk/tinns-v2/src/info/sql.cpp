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

PMySQL::PMySQL()
{
    port = Config->GetOptionInt("sql_port");
    strncpy(host, Config->GetOption("sql_host").c_str(), 100);
    strncpy(userName, Config->GetOption("sql_username").c_str(), 100);
    strncpy(password, Config->GetOption("sql_password").c_str(), 100);
    strncpy(database, Config->GetOption("global_sql_database").c_str(), 100);

    mKeepaliveDelay = (std::time_t) (Config->GetOptionInt("mysql_wait_timeout") * 0.9) ; // we take 90% of the wait_timeout to trigger keepalive
    if (mKeepaliveDelay == 0)
    {
      Console->Print("%s MySQL keepalive disabled by config", Console->ColorText(GREEN, BLACK, "[Info]"));
    }
    else if (mKeepaliveDelay < 60)
    {
      Console->Print("%s Configuration option 'mysql_wait_timeout' is too low (%d sec). Reset to 60 sec.", Console->ColorText(YELLOW, BLACK, "[Notice]"), mKeepaliveDelay);
      mKeepaliveDelay = 60;
    }
    mLastKeepaliveSent = 0;
}

PMySQL::~PMySQL()
{
    Console->Print("Closing MySQL connection...");
    mysql_close(dbHandle);
}

void PMySQL::Update()
{    
    // MySQL keepalive
    std::time_t t = std::time(NULL);
    if ((mKeepaliveDelay > 0) && ((t - mLastKeepaliveSent) > mKeepaliveDelay))
    {
      MYSQL_RES *result;
      char query[24];
      snprintf (query, 24, "SELECT NOW()");

      result = ResQuery(query);
      if(!result)
      {
          Console->Print("%s Can't send InfoDB keepalive; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
          ShowSQLError();
          return;
      }
      else
        FreeSQLResult(result);      
    
      mLastKeepaliveSent = std::time(NULL);
//Console->Print("%s MySQL keepalive sent", Console->ColorText(GREEN, BLACK, "[Debug]"));
    }
}

bool PMySQL::Connect()
{
    Console->LPrint("Establishing link to MySQL Database...");

	dbHandle = mysql_init(NULL);

	if(dbHandle)
	{
		//Console->Print("MySQL-Handle successfully initialized.");
	}
	else
	{
		Console->LPrint(RED, BLACK, "[ERROR]");
		Console->LClose();
		Console->Print(" MySQL-Handle couldn't be created!");
		exit(0);
	}

    if(!mysql_real_connect(dbHandle, host, userName, password, database, port, NULL, 0))
    {
		Console->LPrint(RED, BLACK, "[ERROR]");
		Console->LClose();
        Console->Print("Unable to connect to MySQL Database. MySQL returned: %s", mysql_error(dbHandle));
        return false;
    }
    else
    {
        Console->LPrint(GREEN, BLACK, "Success");
        Console->LClose();
        return true;
    }
}

MYSQL_RES *PMySQL::ResQuery(const char *query)
{
    int sql_result = 0;
    MYSQL_RES *result;

    sql_result = mysql_real_query(dbHandle, query, strlen(query));
    if(sql_result)
    {
        return NULL;
    }
    result = mysql_store_result(dbHandle);
    if(!result)
    {
        return NULL;
    }

    return result;
}

int PMySQL::Query(const char *query)
{
    int sql_result = 0;
    sql_result = mysql_real_query(dbHandle, query, strlen(query));

    return sql_result;
}

void PMySQL::ShowSQLError()
{
    Console->Print(RED, BLACK, "MySQL Error: %s", mysql_error(dbHandle));
}

void PMySQL::FreeSQLResult(MYSQL_RES *res)
{
  mysql_free_result(res);
}

u32 PMySQL::EscapeString(const char* nText, char* dText, u32 dMaxLength)
{
  u32 nLength = strlen(nText);
  u32 tMax = (dMaxLength - 1)/2;
  if(nLength > tMax)
  {
    nLength = tMax;
  }
  
  return mysql_real_escape_string(dbHandle, dText, nText, nLength);
}
