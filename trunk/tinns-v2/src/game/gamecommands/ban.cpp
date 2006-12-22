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

void PCommands::doCmdban()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
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
    if(source->GetAccount()->GetLevel() <= target->GetAccount()->GetLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant ban %s, target level is higher or equal to yours!", Database->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }
    char tmpTimeVal[10];
    GetArgText(2, tmpTimeVal, 10);

    int loop_i = 0;
    char tmp_atoi[10];

    while(isdigit(tmpTimeVal[loop_i]) != 0 && loop_i < 10)
    {
        tmp_atoi[loop_i] = tmpTimeVal[loop_i];
        loop_i++;
    }
    char timefactor[1];
    timefactor[0] = tmpTimeVal[loop_i];
    int timevalue = atoi(tmp_atoi);
    int time_to_ban = 0;

    if(strcmp(timefactor, "s") == 0 || strcmp(timefactor, "S") == 0 )
    {
        time_to_ban = timevalue;
    }
    else if(strcmp(timefactor, "m") == 0 || strcmp(timefactor, "M") == 0 )
    {
        time_to_ban = timevalue * 60;
    }
    else if(strcmp(timefactor, "h") == 0 || strcmp(timefactor, "H") == 0 )
    {
        time_to_ban = timevalue * 60 * 60;
    }
    else if(strcmp(timefactor, "d") == 0 || strcmp(timefactor, "D") == 0 )
    {
        time_to_ban = timevalue * 60 * 60 * 24;
    }
    else
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@ban <charID or nickname> <xS(econds)>/<xM(inutes)>/<xH(ours)>/<xD(ays)>");
        return;
    }
    int final_bantime = std::time(NULL) + time_to_ban;
    target->GetAccount()->SetBannedStatus(final_bantime);
    target->GetAccount()->SQLSave();
    GameServer->ClientDisconnected(target);  // Now kick the player (Hes banned :) )
}
