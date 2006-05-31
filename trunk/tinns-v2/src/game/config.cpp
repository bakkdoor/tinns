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
        config.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 23 Dec 2005 Namikon
        REASON: - Added GPL
        MODIFIED: 07 Jan 2006 Namikon
        REASON: - Started to replace XML with CFG files
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

	ConfigFile = fopen("./conf/gameserver.conf","r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file \"gameserver.conf\"", Console->ColorText(RED, BLACK, "[Error]"));
        return false;
	}

	Console->LPrint("Loading configuration file...");

	while(!feof(ConfigFile))
	{
	    fgets(line, 255, ConfigFile);
	    //Console->Print("LINE: [%s]", line);
	    Opt = strtok(line, "=");
	    Val = strtok(NULL, "\n\r");

        if(Opt && Val)
        {

        if (line[0] == '/' && line[1] == '/')
            continue;

          //  Console->Print("[%s] [%s]", Opt, Val);

        Trim(Opt);
        Trim(Val);

            //Console->Print("[%s] [%s]", Opt, Val);
        }
        else
        {
            continue;
        }

	    if(!strcmp(Opt, "info_sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "server_name"))
            numOptions++;
	    else if(!strcmp(Opt, "server_ip"))  // Siehe config file. Evtl weg
            numOptions++;
	    else if(!strcmp(Opt, "gameserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "server_version"))
            numOptions++;
	    else if(!strcmp(Opt, "maxclients"))
            numOptions++;
	    else if(!strcmp(Opt, "gm_slots"))
            numOptions++;
	    else if(!strcmp(Opt, "defs_path"))
            numOptions++;

	    if(Opt && Val)
	    {
	        //Console->Print("#%d [%s] [%s]", numOptions, Opt, Val);
            mOptions.insert(std::make_pair(Opt, Val));
	    }
	}
	if(numOptions == 17)
	{
	    Console->LPrint(GREEN, BLACK, "Success");
	    Console->LClose();
        return true;
	}
    else
    {
        Console->LPrint(YELLOW, BLACK, "Warning");
        Console->LClose();
        Console->Print("Configuration file is incomplete! %d", numOptions);
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
