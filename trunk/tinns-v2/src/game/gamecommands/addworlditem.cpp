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

void PCommands::doCmdaddworlditem()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    PChar *Char = Database->GetChar(source->GetCharID());
    char query[255], answer[255];
    int worlditemID = 0, worlditemType = 0, worlditemOp1 = 0, worlditemOp2 = 0;
    int Location = Char->GetLocation();

    if(IsArgNumeric(1) == false || IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        if(GetArgInt(1) == 0 || GetArgInt(2) == 0)
        {
            SyntaxError = true;
        }
        else
        {
            worlditemID = GetArgInt(1);
            worlditemType = GetArgInt(2);
        }
    }
    if(ArgC > 2)
    {
        worlditemOp1 = GetArgInt(3);
        if(ArgC > 3)
        {
            worlditemOp2 = GetArgInt(4);
        }
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@addworlditem <ItemID> <ItemType> [<Option1> <Option2>]");
        return;
    }

    if(worlditemType < 1 || worlditemType > 19)
    {
        Chat->send(source, CHAT_DIRECT, "System", "Invalid Worlditemtype (1-19)");
        return;
    }

    if (Location > 100000)
    {
        Chat->send(source, CHAT_DIRECT, "System", "You cant add worlditems in appartments at the moment");
        return;
    }
    else
    {
        int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
        if(checkval > 0 && checkval < 5)
        {
            sprintf(answer, "WorlditemID %d is already in the Database! Use @delworlditem <id> to remove it first", worlditemID);
            Chat->send(source, CHAT_DIRECT, "System", answer);
            return;
        }
        sprintf (query, "INSERT INTO world_items (wi_worlditem_id, wi_worlditem_map, wi_type, wi_option1, wi_option2) VALUES (%d, %d, %d, %d, %d)", worlditemID, Location, worlditemType, worlditemOp1, worlditemOp2);
        if(MySQL->GameQuery(query))
        {
            sprintf(answer, "Error while adding WorlditemID %d to MySQL DB. See console output for MySQL error", worlditemID);
            Chat->send(source, CHAT_DIRECT, "System", answer);
            MySQL->ShowGameSQLError();
            return;
        }
        sprintf(answer, "Added worlditemID %d, location %d as type %d", worlditemID, Location, worlditemType);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        Console->Print("IngameCommand: Added Worlditem ID %d, Location %d to Database", worlditemID, Char->GetLocation());
        return;
    }
}
