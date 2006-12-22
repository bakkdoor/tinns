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

void PCommands::doCmdrecall()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@recall <charID or nickname>");
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
    // Check accountlevel (Only higher's can perform commands on lower's)
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant warp %s to you, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    // Warp target player to GM/Admin
    int destZone = Database->GetChar(source->GetCharID())->GetLocation();
    if (!Worlds->IsValidWorld(destZone))
    {
        Console->Print("%s Can't change location, destZone '%d' is invalid for some reason", Console->ColorText(RED, BLACK, "[PANIC]"), destZone);
        return;
    }
// *************** Checks done, proceed with command
    if (target->ChangeCharLocation(destZone))
    {
        PMessage* tmpMsg_zone = MsgBuilder->BuildAptLiftUseMsg (target, destZone, 0);
        target->getUDPConn()->SendMessage(tmpMsg_zone);
        tmpMsg_zone = NULL;

        // World changed, now place victim to position of GM/Admin
        PMessage* tmpMsg_posupdate;

        // Get position of GM/Admin
        u16 nNewX, nNewY, nNewZ;
        nNewX = source->GetChar()->Coords.mX;
        nNewY = source->GetChar()->Coords.mY;
        nNewZ = source->GetChar()->Coords.mZ;

        // And now built new message for Victim, with new X/Y/Z Coords
        tmpMsg_posupdate = MsgBuilder->BuildCharPosMoveMsg(target, nNewX, nNewY, nNewZ);

        // Finish packet (Add UDP_ID stuff etc)
        target->IncreaseUDP_ID();
        tmpMsg_posupdate->U16Data(0x01) = target->GetUDP_ID();
        tmpMsg_posupdate->U16Data(0x03) = target->GetSessionID();
        tmpMsg_posupdate->U16Data(0x07) = target->GetUDP_ID();

        // Send packet
        target->getUDPConn()->SendMessage(tmpMsg_posupdate);
    }
    else
    {
        Console->Print("%s Unable to change location for player %d to %d", Console->ColorText(RED, BLACK, "[PANIC]"), target->GetCharID(), destZone);
    }
}
