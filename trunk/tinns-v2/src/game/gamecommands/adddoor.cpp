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

#include "main.h"

// Not required anymore, TinNS gets is worlditemdata from .dat files now

void PCommands::doCmdadddor()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    int doorID = 0, doorType = 0;

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        doorID = GetArgInt(1);
        doorType = GetArgInt(2);
    }

    if(doorID == 0 || doorType == 0 && (doorType == 1 || doorType == 2) || SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@adddoor <doorid> <1=singledoor / 2=doubledoor>");
        return;
    }
    else
    {
        char query[1024], answer[1024];
        PChar *Char = Database->GetChar(source->GetCharID());
        Console->Print("IngameCommand: Adding Worlddoor ID %d, Location %d to Database", doorID, Char->GetLocation());
        sprintf(query, "INSERT INTO world_doors (`wd_world_id`, `wd_type`, `wd_world_map`) VALUES ('%d', '%d', '%d');", doorID, doorType, Char->GetLocation());
        if(MySQL->GameQuery(query) > 0)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Failed to add door; Check console for MySQL error");
            Console->Print("Failed to add door! MySQL returned %s", mysql_error(MySQL->GetGameHandle()));
            return;
        }
        if(doorType == 2)
        {
            sprintf(answer, "Succesfully added doorID %d in location %d as doubledoor", doorID, Char->GetLocation());
        }
        else if(doorType == 1)
        {
            sprintf(answer, "Succesfully added doorID %d in location %d as singledoor", doorID, Char->GetLocation());
        }
        Chat->send(source, CHAT_DIRECT, "System", answer);
    }
}
*/
