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

void PCommands::doCmddebug()
{
    PDebugMode nWhat = DBG_ALL;
    int nHow = -1;
    char* DbgTarget = "all";
    char DbgMessage[80];

    if(ArgC < 1)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@debug [loc[ation] | it[emid]] [0|1]");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        if(GetArgInt(1) == 0)
        {
            nHow = 0;
        }
        else
        {
            nHow = 1;
        }
    }
    else
    {
        char tmp[10];
        if(GetArgText(1, tmp, 10))
        {
            if(strncmp(tmp, "loc", 3) == 0)
            {
                nWhat = DBG_LOCATION;
                DbgTarget = "location";
            }
            else if(strncmp(tmp, "it", 2) == 0)
            {
                nWhat = DBG_ITEMID;
                DbgTarget = "itemid";
            }
        }
    }

    if (nWhat != DBG_ALL)
    {
        if(ArgC == 1)
        {
            nHow = (source->GetDebugMode(nWhat) ? 0 : 1); // toggle if no arg
        }
        else if(ArgC > 1 && GetArgInt(2) == 0)
        {
            nHow = 0;
        }
        else if(ArgC > 1 && GetArgInt(2) == 1)
        {
            nHow = 1;
        }
    }

    if (nHow != -1)
    {
        source->SetDebugMode(nWhat, nHow);
        snprintf(DbgMessage, 80, "Debug %s is now %s", DbgTarget, (nHow ? "ENABLED" : "DISABLED"));
        Chat->send(source, CHAT_DIRECT, "System", DbgMessage);
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@debug [loc[ation] | it[emid]] [0|1]");
    }
}
