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

void PCommands::doCmddelworlditem()
{
    bool SyntaxError = false;

    PChar *Char = Database->GetChar(source->GetCharID());
    char query[255], answer[255];
    int worlditemID = 0;
    int Location = Char->GetLocation();

    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == true)
    {
        worlditemID = GetArgInt(1);
        if(worlditemID == 0)
        {
            SyntaxError = true;
        }
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@delworlditem <ItemID>");
        return;
    }

    int checkval = MySQL->GetWorldItemType((unsigned short)worlditemID, Location);
    if(checkval < 1 || checkval > 5)
    {
        sprintf(answer, "Cannot delete WorlditemID %d, not in database!", worlditemID);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        return;
    }

    sprintf(query, "DELETE FROM world_items WHERE wi_worlditem_id = %d AND wi_worlditem_map = %d", worlditemID, Location);
    if(MySQL->GameQuery(query))
    {
        sprintf(answer, "Unable to delete worlditemID %d, see console for MySQL error", worlditemID);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        MySQL->ShowGameSQLError();
        return;
    }
    else
    {
        sprintf(answer, "Successfully deleted worlditemID %d, location %d from Database", worlditemID, Location);
        Chat->send(source, CHAT_DIRECT, "System", answer);
        return;
    }
}
*/
