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

	ConfigFile = fopen("./conf/patchserver.conf","r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file \"patchserver.conf\"", Console->ColorText(RED, BLACK, "[Error]"));
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

            if(!strcmp(Opt, "patchserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "patches_path"))
            numOptions++;
	    else if(!strcmp(Opt, "file_path"))
            numOptions++;
	    else if(!strcmp(Opt, "max_file_xfers"))
            numOptions++;
	    else if(!strcmp(Opt, "patch_packet_size"))
            numOptions++;

	    if(Opt && Val)
                 mOptions.insert(std::make_pair(Opt, Val));
	}
	if(numOptions == 5)
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
