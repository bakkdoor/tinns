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

void PCommands::doCmdkick()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@kick <charID or nickname>");
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

    // Make sure only people with a higher level than victim can kick victim
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant kick %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

// *************** Checks done, proceed with command
    int final_bantime = std::time(NULL) + 60;               // Ban 60 seconds (Anti-Rejoin)
    target->GetAccount()->SetBannedStatus(final_bantime);
    target->GetAccount()->SQLSave();

    InitCharVanish(target);
    GameServer->ClientDisconnected(target);                 // Kick

    Console->Print("%s %s (Lv %d) kicked %s (Lv %d)", Console->ColorText(YELLOW, BLACK, "[GameCommand]"), Database->GetChar(source->GetCharID())->GetName().c_str(), source->GetAccount()->GetLevel(), Database->GetChar(target->GetCharID())->GetName().c_str(), target->GetAccount()->GetLevel());
    char successmsg[100];
    snprintf(successmsg, 99, "Kicked %s", Database->GetChar(target->GetCharID())->GetName().c_str());
    successmsg[99] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", successmsg);
}
