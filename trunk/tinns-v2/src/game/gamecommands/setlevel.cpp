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

void PCommands::doCmdsetlevel()
{
    int destLevel = 0;
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(2) == false)
    {
        SyntaxError = true;
    }
    else
    {
        destLevel = GetArgInt(2);
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setlevel <charID or nickname> <newlevel 1-99>");
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
    if(source->GetAccountLevel() <= target->GetAccountLevel())
    {
        char tmpMsg[200];
        snprintf(tmpMsg, 199, "Cant set new level for %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
        tmpMsg[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
        return;
    }

    PAccount Acc(target->GetAccountID());
    Acc.SetLevel(destLevel);
    Acc.Save();
    
    char tmpMsg[60], tmpMsg2[60];
    snprintf(tmpMsg, 59, "Set level for player %s to %d", Chars->GetChar(target->GetCharID())->GetName().c_str(), destLevel);
    snprintf(tmpMsg2, 59, "**POOF** Your new accesslevel is now %d", destLevel);

    tmpMsg[59] = '\0';
    tmpMsg2[59] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
    Chat->send(target, CHAT_DIRECT, "System", tmpMsg2);
}
