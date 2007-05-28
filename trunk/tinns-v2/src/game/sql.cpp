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
	sql.cpp - handles all mysql interactions

	Authors:
	- Namikon
	- bakkdoor

        MODIFIED: 24 Dec 2005 Namikon
        REASON: - initial release by Namikon
        MODIFIED: 26 Dec 2005 Namikon
        REASON: - Fixed Connect and Query function
                - Added GetWorldDoorType() for door interaction
                - Added GetWorldItemType() for item interaction
                - Added GetWorldItemOption() for item interaction
        MODIFIED: 30 Dec 2005 bakkdoor
        REASON: - changed mysql_init() parameter to null to prevent segfault
                - added checking for dbHandle to prevent segfault
        MODIFIED: 01 Jan 2006 Namikon
        REASON: - Removed 2 Console outputs in ResQuery()
                - Added missing "return NULL" to ResQuery
        MODIFIED: 06 Jan 2006 Namikon
        REASON: - Added color to console outputs
                - Changed a few lines on the Get* functions (another std::atoi thing)
        MODIFIED: 26 Jul 2006 Hammag
        REASON: - Added CheckResCount() for DB Res memory leak tracking (to be done in the main loop)
                    rather than through Info/GameResQuery()
                - fixed InfoDBInuse and GameDBInuse updating
                - inhibited Info/GameDBInuse warning message in Info/GameResQuery()
        MODIFIED: 27 Sep 2006 Hammag
        REASON: - Added GetAptLocation() method
                - Modified Apt & Item info methods as they should work, but it doesn't match the DB
                  So commented out some changes
                  DB shouln't be used anymore for that soon anyway


*/
#include "main.h"

PMySQL::PMySQL()
{
    InfoDBInuse = 0;
    GameDBInuse = 0;
    info_port = Config->GetOptionInt("info_sql_port");
    strcpy(info_host, Config->GetOption("info_sql_host").c_str());
    strcpy(info_userName, Config->GetOption("info_sql_username").c_str());
    strcpy(info_password, Config->GetOption("info_sql_password").c_str());
    strcpy(info_database, Config->GetOption("info_sql_database").c_str());

    game_port = Config->GetOptionInt("game_sql_port");
    strcpy(game_host, Config->GetOption("game_sql_host").c_str());
    strcpy(game_userName, Config->GetOption("game_sql_username").c_str());
    strcpy(game_password, Config->GetOption("game_sql_password").c_str());
    strcpy(game_database, Config->GetOption("game_sql_database").c_str());

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
    mysql_close(info_dbHandle);
    mysql_close(game_dbHandle);
}

void PMySQL::Update()
{
    CheckResCount(); // Check for MYSQL_RES mem leak

    // MySQL keepalive
    std::time_t t = std::time(NULL);
    if ((mKeepaliveDelay > 0) && ((t - mLastKeepaliveSent) > mKeepaliveDelay))
    {
      MYSQL_RES *result;
      char query[24];
      sprintf (query, "SELECT NOW()");

      result = GameResQuery(query);
      if(!result)
      {
          Console->Print("%s Can't send GameDB keepalive; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
          ShowGameSQLError();
          return;
      }
      else
        FreeGameSQLResult(result);

      result = InfoResQuery(query);
      if(!result)
      {
          Console->Print("%s Can't send InfoDB keepalive; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
          ShowInfoSQLError();
          return;
      }
      else
        FreeInfoSQLResult(result);

      mLastKeepaliveSent = std::time(NULL);
      if (gDevDebug) Console->Print("%s MySQL keepalive sent", Console->ColorText(GREEN, BLACK, "[Debug]"));
    }
}

void PMySQL::CheckResCount()
{
  static int MaxInfoDBCount = 0;
  static int MaxGameDBCount = 0;

  if (InfoDBInuse > MaxInfoDBCount)
  {
    Console->Print("%s Max In-use InfoDB Resources number increasing : %d (+%d)", Console->ColorText(YELLOW, BLACK, "[Notice]"), InfoDBInuse, InfoDBInuse-MaxInfoDBCount);
    MaxInfoDBCount = InfoDBInuse;
  }

  if (GameDBInuse > MaxGameDBCount)
  {
    Console->Print("%s Max In-use GameDB Resources number increasing : %d (+%d)", Console->ColorText(YELLOW, BLACK, "[Notice]"), GameDBInuse, GameDBInuse-MaxGameDBCount);
    MaxGameDBCount = GameDBInuse;
  }
}

bool PMySQL::Connect()
{
    Console->LPrint("Establishing link to Infoserver Database...");

    info_dbHandle = mysql_init(NULL);

    if(!info_dbHandle)
    {
        Console->LPrint(RED, BLACK, "[ERROR]");
        Console->LClose();
        Console->Print(" Unable to create MySQL-Handle!");
        exit(0);
    }

    if(!mysql_real_connect(info_dbHandle, info_host, info_userName, info_password, info_database, info_port, NULL, 0))
    {
        Console->LPrint(RED, BLACK, "[ERROR]");
        Console->LClose();
        Console->Print("Unable to connect to Infoserver Database. MySQL returned: %s", mysql_error(info_dbHandle));
    }
    else
    {
        Console->LPrint(GREEN, BLACK, "Success");
        Console->LClose();
    }
// <><><><><><><><> Gameserver DB <><><><><><><><>
    Console->LPrint("Establishing link to Gameserver Database...");

    game_dbHandle = mysql_init(NULL);

    if(!game_dbHandle)
    {
        Console->LPrint(RED, BLACK, "[ERROR]");
        Console->LClose();
        Console->Print(" Unable to create MySQL-Handle!");
        exit(0);
    }

    if(!mysql_real_connect(game_dbHandle, game_host, game_userName, game_password, game_database, game_port, NULL, 0))
    {
        Console->LPrint(RED, BLACK, "[ERROR]");
        Console->LClose();
        Console->Print("Unable to connect to Gameserver Database. MySQL returned: %s", mysql_error(game_dbHandle));
        return false;
    }
    else
    {
        Console->LPrint(GREEN, BLACK, "Success");
        Console->LClose();
        return true;
    }

}
// ----------------------------------------------------
MYSQL_RES *PMySQL::InfoResQuery(const char *query)
{
    int sql_result = 0;
    MYSQL_RES *result;

    sql_result = mysql_real_query(info_dbHandle, query, strlen(query));
    if(sql_result)
    {
        return NULL;
    }
    result = mysql_store_result(info_dbHandle);
    if(!result)
    {
        return NULL;
    }
    //if(InfoDBInuse == true)
    /*if(InfoDBInuse > 0)
    {
        Console->Print("%s another (%d) info_dbHandle result is still in use", Console->ColorText(YELLOW, BLACK, "[Warning]"), InfoDBInuse);
    }*/

    //InfoDBInuse = true;
    InfoDBInuse++;
    return result;
}

int PMySQL::InfoQuery(const char *query)
{
    int sql_result = 0;
    sql_result = mysql_real_query(info_dbHandle, query, strlen(query));

    return sql_result;
}

void PMySQL::ShowInfoSQLError()
{
    Console->Print(RED, BLACK, "MySQL Error: %s", mysql_error(info_dbHandle));
}
void PMySQL::FreeInfoSQLResult(MYSQL_RES *res)
{
    if(InfoDBInuse > 0)
    {
        mysql_free_result(res);
        InfoDBInuse--;
    }
    else
        Console->Print("PMySQL::FreeInfoSQLResult: Nothing to free...");
}
// ----------------------------------------------------
MYSQL_RES *PMySQL::GameResQuery(const char *query)
{
    int sql_result = 0;
    MYSQL_RES *result;

    sql_result = mysql_real_query(game_dbHandle, query, strlen(query));
    if(sql_result)
    {
        return NULL;
    }
    result = mysql_store_result(game_dbHandle);
    if(!result)
    {
        return NULL;
    }
    /*if(GameDBInuse > 0)
    {
        Console->Print("%s another (%d) game_dbHandle result is still in use", Console->ColorText(YELLOW, BLACK, "[Warning]"), GameDBInuse);
    }*/

    //GameDBInuse = true;
    GameDBInuse++;
    return result;
}

int PMySQL::GameQuery(const char *query)
{
    int sql_result = 0;
    sql_result = mysql_real_query(game_dbHandle, query, strlen(query));

    return sql_result;
}

void PMySQL::ShowGameSQLError()
{
    Console->Print(RED, BLACK, "MySQL Error: %s", mysql_error(game_dbHandle));
}

void PMySQL::FreeGameSQLResult(MYSQL_RES *res)
{
    if(GameDBInuse > 0)
    {
        mysql_free_result(res);
        GameDBInuse--;
    }
    else
        Console->Print("PMySQL::FreeGameSQLResult: Nothing to free...");
}
// ----------------------------------------------------
int PMySQL::GetWorldItemType(unsigned short ID, int Location)
{
    char query[2048];
    MYSQL_RES *result;
    MYSQL_ROW row;

    if (Location > 100000)
    {
      //int nAppLoc = GetAptLocation(Location);
      int nAppLoc = Location - 100000; // temp as DB doesn't link with App world ID, but with app ID
      if (nAppLoc)
        sprintf(query, "SELECT ai_type FROM apt_items WHERE ai_apt_id = %d AND ai_apt_map = %d", ID, nAppLoc);
      else
        return 0;
    }
    else
      sprintf(query, "SELECT wi_type FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", ID, Location);

    result = GameResQuery(query);
    if(!result)
    {
        Console->Print("%s Cannot get WorldItemType; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }
    else
    {
        if(mysql_num_rows(result) > 1)
        {
            FreeGameSQLResult(result);
            return -2;
        }
        else
        {
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                FreeGameSQLResult(result);
                return -1;
            }
            int ret_val = std::atoi(row[0]);
            FreeGameSQLResult(result);
            return ret_val;
        }
    }
/*    else
    {
        FreeGameSQLResult(result);
        return -1;
    }
*/
    return -1;
}

int PMySQL::GetWorldItemOption(unsigned short ID, int Location, int option)
{
    char query[2048];
    MYSQL_RES *result;
    MYSQL_ROW row;
    if(option != 1 && option != 2 && option != 3)
    {
        return -1;
    }

    if (Location > 100000)
    {
      //int nAppLoc = GetAptLocation(Location);
      int nAppLoc = Location - 100000; // temp as DB doesn't link with App world ID, but with app ID
      if (nAppLoc)
        sprintf(query, "SELECT ai_option%d FROM apt_items WHERE ai_apt_id = %d AND ai_apt_map = %d", option, ID, nAppLoc);
      else
        return 0;
    }
    else
      sprintf(query, "SELECT wi_option%d FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", option, ID, Location);

    result = GameResQuery(query);
    if(!result)
    {
        Console->Print("%s Cannot get WorldItemOption; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }
    else
    {
        if(mysql_num_rows(result) > 1)
        {
            FreeGameSQLResult(result);
            return -2;
        }
        else
        {
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                FreeGameSQLResult(result);
                return -1;
            }
            int ret_val = std::atoi(row[0]);
            FreeGameSQLResult(result);
            return ret_val;
        }
    }
/*
    else
    {
        FreeGameSQLResult(result);
        return -1;
    }
*/
    return -1;
}

int PMySQL::GetWorldDoorType(unsigned int ID, int Location) /* To be removed */
{
    char query[2048];
    MYSQL_RES *result;
    MYSQL_ROW row;

Console->Print(RED, BLACK, "PMySQL::GetWorldDoorType: DATABASE MUST NOT BE USED ANYMORE FOR DOORS INFO !!!");
    if (Location > 100000)
    {
      //int nAppLoc = GetAptLocation(Location);
      int nAppLoc = Location - 100000; // temp as DB doesn't link with App world ID, but with app ID
      if (nAppLoc)
        sprintf(query, "SELECT ad_type FROM apt_doors, apartments WHERE apt_doors.ad_apt_map = apartments.apt_type AND apt_doors.ad_apt_id = %i AND apartments.apt_id = %i", ID, nAppLoc);
      else
        return 0;
    }
    else
    {
        sprintf(query, "SELECT wd_type FROM world_doors WHERE wd_world_id = %d AND wd_world_map = %d", ID, Location);
    }
    result = GameResQuery(query);

    if(!result)
    {
        Console->Print("%s Cannot get WorldDoorType; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    } else {
        if(mysql_num_rows(result) > 1)
        {
            FreeGameSQLResult(result);
            return -2;
        }
        else
        {
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                FreeGameSQLResult(result);
                return -1;
            }
            int ret_val = std::atoi(row[0]);
            FreeGameSQLResult(result);
            return ret_val;
        }
    }
/*
    else
    {
        FreeGameSQLResult(result);
        return -1;
    }
*/
    return -1;
}

int PMySQL::GetAptID(unsigned int AptLoc, const u8 *pass)
{
    int type;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[255];

    if(!strcmp((const char*)pass, "Exit"))
        return 1;

    sprintf(query, "SELECT apt_id FROM apartments WHERE apt_location = %i AND apt_password = \"%s\"", AptLoc, pass);
    result = GameResQuery(query);

    if(!result)
    {
        Console->Print("%s Cannot get AppartmentID; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        FreeGameSQLResult(result);
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        type = std::atoi(row[0]);
        FreeGameSQLResult(result);
    }

    return (type + 100000);
}

int PMySQL::GetAptType(int AptID)
{
    int type;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[255];

    sprintf(query, "SELECT apt_type FROM apartments WHERE apt_id = %i", AptID - 100000);
    result = GameResQuery(query);

    if(!result)
    {
        Console->Print("%s Cannot get AppartmentType; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        FreeGameSQLResult(result);
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        type = std::atoi(row[0]);
        FreeGameSQLResult(result);
    }

    return type;
}

int PMySQL::GetAptOwner(int loc)
{
    int owner;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[255];

    sprintf (query, "SELECT apt_owner FROM apartments WHERE apt_id = %i", loc - 100000);

    result = GameResQuery(query);
    if(!result)
    {
        Console->Print("%s Cannot get AppartmentOwner; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        FreeGameSQLResult(result);
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        owner = std::atoi(row[0]);
        FreeGameSQLResult(result);
    }

    return owner;
}

int PMySQL::GetAptLocation(int loc)
{
    int Location;
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[255];

    if(loc > 100000)
        loc = loc - 100000;

    sprintf (query, "SELECT apt_location FROM apartments WHERE apt_id = %i", loc);

//Console->Print("Q: %s", query);
    result = GameResQuery(query);
    if(!result)
    {
        Console->Print("%s Cannot get AppartmentLocation; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        FreeGameSQLResult(result);
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        Location = std::atoi(row[0]);
        FreeGameSQLResult(result);
    }

    return Location;
}
