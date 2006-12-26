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

void PCommands::doCmdwarpto()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@warpto <charID or nickname>");
        return;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        char tmp_destNick[50];
        GetArgText(1, tmp_destNick, 50);
        target = GetClientByNick(tmp_destNick);
    }

    if(target == NULL) // If victim isnt found, return error
    {
        Chat->send(source, CHAT_DIRECT, "System", "No such player");
        return;
    }

    int destZone = 0;
    // Warp GM/Admin to target player
    destZone = Database->GetChar(target->GetCharID())->GetLocation();
    /*if (gDevDebug)*/ Console->Print("%s Warping %d to location of %d (> %d)", Console->ColorText(YELLOW, BLACK, "[Notice]"), source->GetCharID(), target->GetCharID(), destZone);
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    InitWarpCircle(source);
    InitCharVanish(source);
    if (source->ChangeCharLocation(destZone), true)
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (source, destZone, 0);
        source->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;

        u16 nNewX, nNewY, nNewZ;
        nNewX = target->GetChar()->Coords.mX;
        nNewY = target->GetChar()->Coords.mY;
        nNewZ = target->GetChar()->Coords.mZ;
        source->SetAwaitingWarpto(true, nNewX, nNewY, nNewZ);

        char tmpMsg_success[81];
        snprintf(tmpMsg_success, 80, "Successfully warped you to %s", target->GetChar()->GetName().c_str());
        tmpMsg_success[80] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
        return;
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), source->GetCharID(), destZone);
    }
}
