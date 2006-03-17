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
*/
#include "tinns.h"

PMySQL::PMySQL()
{
    port = Config->GetOptionInt("sql_port");
    strcpy(host, Config->GetOption("sql_host").c_str());
    strcpy(userName, Config->GetOption("sql_username").c_str());
    strcpy(password, Config->GetOption("sql_password").c_str());
    strcpy(database, Config->GetOption("sql_database").c_str());
}

PMySQL::~PMySQL()
{
    Console->Print("Closing MySQL connection...");
    mysql_close(dbHandle);
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

int PMySQL::GetWorldItemType(unsigned short ID, int Location) {
    char query[2048];
    MYSQL_RES *result;
    MYSQL_ROW row;
    sprintf(query, "SELECT wi_type FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", ID, Location);

    if (mysql_real_query(dbHandle, query, strlen(query)))
    {
        return -1;
    }
    else
    {
        result = mysql_store_result(dbHandle);
        if (result)
        {
            if(mysql_num_rows(result) > 1)
            {
                return -2;
            }
            else
            {
                row = mysql_fetch_row(result);
                if(row == NULL) {
                    return -1;
                }

                return std::atoi(row[0]);
            }
        }
        else
        {
            return -1;
        }
    }
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
    sprintf(query, "SELECT wi_option%d FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", option, ID, Location);
    //Console->Print("MySQL Query: %s", query);
    //return -1;
    if (mysql_real_query(dbHandle, query, strlen(query)))
    {
        return -1;
    }
    else
    {
        result = mysql_store_result(dbHandle);
        if (result)
        {
            if(mysql_num_rows(result) > 1)
            {
                return -2;
            }
            else
            {
                row = mysql_fetch_row(result);
                if(row == NULL) {
                    return -1;
                }

                return std::atoi(row[0]);
            }
        }
        else
        {
            return -1;
        }
    }
    return -1;
}

int PMySQL::GetWorldDoorType(unsigned int ID, int Location) {
    char query[2048];
    MYSQL_RES *result;
    MYSQL_ROW row;

    if(Location > 100000)
    {
        sprintf(query, "SELECT ad_type FROM apt_doors, apartments WHERE apt_doors.ad_apt_map = apartments.apt_type AND apt_doors.ad_apt_id = %i AND apartments.apt_id = %i", ID, Location-100000);
    }
    else
    {
        sprintf(query, "SELECT wd_type FROM world_doors WHERE wd_world_id = %d AND wd_world_map = %d", ID, Location);
    }

    if(mysql_real_query(dbHandle, query, strlen(query)))
        return -1;

    result = mysql_store_result(dbHandle);
    if (result)
    {
        if(mysql_num_rows(result) > 1)
        {
            return -2;
        }
        else
        {
            row = mysql_fetch_row(result);
            if(row == NULL)
            {
                return -1;
            }

            return std::atoi(row[0]);
        }
    }
    else
    {
        return -1;
    }
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
    result = ResQuery(query);

    if(!result)
    {
        Console->Print("%s Cannot get AppartmentID; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        type = std::atoi(row[0]);
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
    result = ResQuery(query);

    if(!result)
    {
        Console->Print("%s Cannot get AppartmentType; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        type = std::atoi(row[0]);
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

    result = ResQuery(query);
    if(!result)
    {
        Console->Print("%s Cannot get AppartmentType; MySQL returned", Console->ColorText(RED, BLACK, "[Error]"));
        ShowSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        return 0;
    }
    else
    {
        row = mysql_fetch_row(result);
        owner = std::atoi(row[0]);
    }

    return owner;
}
