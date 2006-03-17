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

PConfig::PConfig()
{
}

PConfig::~PConfig()
{
}

bool PConfig::LoadOptions()
{
	FILE *ConfigFile;
	char line[255];
	char *Opt, *Val;
	int numOptions = 0;

	ConfigFile = fopen("./conf/infoserver.conf","r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file", Console->ColorText(RED, BLACK, "[Error]"));
        return false;
	}

	Console->LPrint("Loading configuration file...");

	while(!feof(ConfigFile))
	{
	    fgets(line, 255, ConfigFile);
	    Opt = strtok(line, "=");
	    Val = strtok(NULL, "\n\r");

        if(Opt && Val)
        {

        if (line[0] == '/' && line[1] == '/')
            continue;

        Trim(Opt);
        Trim(Val);

        }
        else
        {
            continue;
        }

	    if(!strcmp(Opt, "sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "global_sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "auto_accounts"))
            numOptions++;
	    else if(!strcmp(Opt, "infoserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "gameserver_livecheck"))
            numOptions++;
	    else if(!strcmp(Opt, "minlevel"))
            numOptions++;

	    if(Opt && Val)
	    {
	        //Console->Print("#%d [%s] [%s]", numOptions, Opt, Val);
            mOptions.insert(std::make_pair(Opt, Val));
	    }
	}
	if(numOptions == 9)
	{
	    Console->LPrint(GREEN, BLACK, "Success");
	    Console->LClose();
        return true;
	}
    else
    {
        Console->LPrint(RED, BLACK, "Failed");
        Console->LClose();
        Console->Print("%s configuration file is incomplete!", Console->ColorText(RED, BLACK, "Error:"));
        return false;
    }
}

const std::string &PConfig::GetOption(const char *Name) const
{
	static std::string NullString = "";
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return i->second;

	return NullString;
}

int PConfig::GetOptionInt(const char *Name) const
{
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return std::atoi(i->second.c_str());

	return 0;
}

bool PConfig::VerifyValues(int *numErr, int *numWarn)
{
    std::string sqlhost = GetOption("sql_host");
    std::string sqlusername = GetOption("sql_username");
    std::string sqlpassword = GetOption("sql_password");
    std::string global_sql_database = GetOption("global_sql_database");
    int sqlport = GetOptionInt("sql_port");
    int auto_accounts = GetOptionInt("auto_accounts");
    int infoserver_port = GetOptionInt("infoserver_port");
    int gameserver_livecheck = GetOptionInt("gameserver_livecheck");
    int minlevel = GetOptionInt("minlevel");

    numErr = numWarn = 0;

    if(strcmp(sqlhost.c_str(), "") == 0)
    {
        Console->Print("%s sql_host is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(strcmp(sqlusername.c_str(), "") == 0)
    {
        Console->Print("%s sql_username is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }
    else if(strcmp(sqlusername.c_str(), "root") ==0)
    {
        Console->Print("%s sqlusername is root, which is pretty insecure.", Console->ColorText(YELLOW, BLACK, "Config warning:"));
        numWarn++;
    }

    if(strcmp(sqlpassword.c_str(), "") == 0);
    {
        Console->Print("%s sql_password is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(strcmp(global_sql_database.c_str(), "") == 0);
    {
        Console->Print("%s global_sql_database is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(sqlport < 1025 || sqlport > 65535)
    {
        Console->Print("%s sql_port has to be between 1025 and 65535!", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(auto_accounts != 0 && auto_accounts != 1)
    {
        Console->Print("%s auto_accounts has to be either 0 or 1!", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(infoserver_port < 1025 || infoserver_port > 65535)
    {
        Console->Print("%s infoserver_port has to be between 1025 and 65535!", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }
    else if(infoserver_port == sqlport)
    {
        Console->Print("%s infoserver_port has to be different from sql_port!", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(gameserver_livecheck == 0)
    {
        Console->Print("%s gameserver_livecheck cannot be 0", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }
    else if(gameserver_livecheck < 2)
    {
        Console->Print("%s gameserver_livecheck is pretty low, this may cause increased mysql load", Console->ColorText(YELLOW, BLACK, "Config warning:"));
        numWarn++;
    }
    else if(gameserver_livecheck > 120 && gameserver_livecheck < 600)
    {
        Console->Print("%s gameserver_livecheck is pretty high, this may cause offline servers shown as online and a wrong usercount", Console->ColorText(YELLOW, BLACK, "Config warning:"));
        numWarn++;
    }
    else if(gameserver_livecheck > 600)
    {
        Console->Print("%s gameserver_livecheck is too high. Use a value between 0 and 600", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(minlevel > 255)
    {
        Console->Print("%s minlevel has to be between 0 and 255", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }
    if(numErr == 0 && numWarn == 0)
        return true;
    else
        return false;
}
