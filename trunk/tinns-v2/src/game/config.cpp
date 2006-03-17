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

#include "tinns.h"


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

	ConfigFile = fopen("./database/config.cfg","r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file", Console->ColorText(RED, BLACK, "[Error]"));
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

	    if(!strcmp(Opt, "sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "server_name"))
            numOptions++;
	    else if(!strcmp(Opt, "server_ip"))
            numOptions++;
	    else if(!strcmp(Opt, "server_version"))
            numOptions++;
	    else if(!strcmp(Opt, "maxclients"))
            numOptions++;
	    else if(!strcmp(Opt, "gm_slots"))
            numOptions++;
	    else if(!strcmp(Opt, "client_path"))  // Is not used yet
            numOptions++;
	    else if(!strcmp(Opt, "defs_path"))
            numOptions++;
	    else if(!strcmp(Opt, "auto_accounts"))
            numOptions++;
	    else if(!strcmp(Opt, "patchserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "patches_path"))
            numOptions++;
	    else if(!strcmp(Opt, "file_path"))
            numOptions++;
	    else if(!strcmp(Opt, "max_file_xfers"))
            numOptions++;
	    else if(!strcmp(Opt, "patch_packet_size"))
            numOptions++;
	    else if(!strcmp(Opt, "infoserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "gameserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "debug_mode"))
            numOptions++;
	    else if(!strcmp(Opt, "useudpport"))
            numOptions++;
	    else if(!strcmp(Opt, "rconsole_enabled"))
            numOptions++;
	    else if(!strcmp(Opt, "rconsole_port"))
            numOptions++;

	    if(Opt && Val)
	    {
	        //Console->Print("#%d [%s] [%s]", numOptions, Opt, Val);
            mOptions.insert(std::make_pair(Opt, Val));
	    }
	}
	if(numOptions == 23)
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

/*bool PConfig::LoadOptions()
{
	Console->Print("Loading configuration file...");
	TiXmlDocument doc("./database/config.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *op = Root->FirstChildElement("option");
			while(op)
			{
				const char *name = op->Attribute("name");
				const char *value = op->Attribute("value");
				if(name && value)
					mOptions.insert(std::make_pair(name, value));

				op = op->NextSiblingElement("option");
			}

			return true;
			Console->Print(GREEN, BLACK, "Success");
		}
	}

	// no config file or no options
	Console->LPrint(RED, BLACK, "Failed");
	Console->LPrint(" (No config found)");
    Console->LClose();
	return false;
}*/

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
