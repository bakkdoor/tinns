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

void PCommands::doCmdinfo()
{
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@info <charID or nickname>");
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
    // *************** Checks done, proceed with command
    /*
        DIRECT> System: PlayerInformation
        DIRECT> Info: CharID     : %d     // victim->GetCharID();
        DIRECT> Info: AccountID  : %d     // victim->GetAccount()->GetID();
        DIRECT> Info: LoginName  : %s     // victim->GetAccount()->GetName();
        DIRECT> Info: AccessLevel: %d     // victim->GetAccountLevel();
        DIRECT> Info: Current Loc: %d     // Chars->GetChar(source->GetCharID())->GetLocation();
        DIRECT> Info: IP address : %s     // victim->GetAddress():

        Maybe for future addons...
        DIRECT> System: CharInformation
        DIRECT> Info: Faction      : %d     // Chars->GetChar(source->GetCharID())->GetFaction();
        DIRECT> Info: Cash         : %d     // Chars->GetChar(source->GetCharID())->GetCash();
        DIRECT> Info: Soullight    : %d     // Chars->GetChar(source->GetCharID())->GetSoullight();
    */
    
    PAccount Acc(target->GetAccountID());
    
    // If source != target
    if(source->GetAccountID() != target->GetAccountID())
    {
        // Check if accesslevel is lower
        if(source->GetAccountLevel() <= target->GetAccountLevel())
        {
            char tmpMsg[200];
            snprintf(tmpMsg, 199, "Cant display info about %s, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
            tmpMsg[199] = '\0';
            Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
            return;
        }
    }

    char tmpInfo_head[151];
    char tmpInfo_cID[151];
    char tmpInfo_aID[151];
    char tmpInfo_Login[151];
    char tmpInfo_AxxLv[151];
    char tmpInfo_Loc[151];
    char tmpInfo_IP[151];

    snprintf(tmpInfo_head, 150,     "PlayerInformation");
    snprintf(tmpInfo_cID, 150,      "CharID     : %d",  target->GetCharID());
    snprintf(tmpInfo_aID, 150,      "AccountID  : %d",  Acc.GetID());
    snprintf(tmpInfo_Login, 150,    "LoginName  : %s",  Acc.GetName().c_str());
    snprintf(tmpInfo_AxxLv, 150,    "AccessLevel: %d",  Acc.GetLevel());
    snprintf(tmpInfo_Loc, 150,      "Current Loc: %d",  Chars->GetChar(target->GetCharID())->GetLocation());
    snprintf(tmpInfo_IP, 150,       "IP address : %s",  target->GetAddress());
    
    tmpInfo_head[150] = '\0';
    tmpInfo_cID[150] = '\0';
    tmpInfo_aID[150] = '\0';
    tmpInfo_Login[150] = '\0';
    tmpInfo_AxxLv[150] = '\0';
    tmpInfo_Loc[150] = '\0';
    tmpInfo_IP[150] = '\0';

    Chat->send(source, CHAT_DIRECT, "System", tmpInfo_head);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_cID);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_aID);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_Login);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_AxxLv);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_Loc);
    Chat->send(source, CHAT_DIRECT, "Info", tmpInfo_IP);
}
