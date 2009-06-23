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

void PCommands::doCmdSetSubSkill()
{
    u8 tNewLevel = 0;
    u8 tSubSkill = 0;

    bool SyntaxError = false;
    if (ArgC < 2)
    {
        SyntaxError = true;
    }

    if (IsArgNumeric(1) == false)
        SyntaxError = true;
    else
        tSubSkill = (u16)GetArgInt(1);

    if (IsArgNumeric(2) == false)
        SyntaxError = true;
    else
        tNewLevel = (u8)GetArgInt(2);

    if (tNewLevel == 0)
        SyntaxError = true;

    if ( source->GetChar()->Skill->IsValidSubSkill(tSubSkill) == false)
        SyntaxError = true;

    if (SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@setsubskill <1-45> <1-255> [<charID or nickname>]");
        Chat->send(source, CHAT_DIRECT, "Usage", "See the GameMaster reference for IDs");
        return;
    }

    PClient *temp_target = NULL;
    if ( ArgC == 3 )
    {
        if (IsArgNumeric(3) == true)
        {
            temp_target = GetClientByID(GetArgInt(3));
        }
        else
        {
            char tmp_destNick[50];
            GetArgText(3, tmp_destNick, 50);
            temp_target = GetClientByNick(tmp_destNick);
        }
        if (temp_target == NULL)
        {
            Chat->send(source, CHAT_DIRECT, "System", "No such player");
            return;
        }
    }
    else
        temp_target = source;


    // Now setting player's Skill tMainSkill to tNewLevel
    u8 tOldLevel = 0;
    int tLevelDiff = 0;

    // Grab old Level
    tOldLevel = temp_target->GetChar()->Skill->GetSubSkill(tSubSkill);
    tLevelDiff = tNewLevel - tOldLevel;
    if ( tLevelDiff == 0 )
    {
        Chat->send(source, CHAT_DIRECT, "System", "Not setting anything. No difference at all");
        return;
    }
    temp_target->GetChar()->Skill->SetSubSkill(tSubSkill, tNewLevel);

    char tmp[50];
    snprintf(tmp, 50, "Setting Level to %d", tNewLevel);
    Chat->send(source, CHAT_DIRECT, "System", tmp);

    MAIN_SKILLS tMainSkill = temp_target->GetChar()->Skill->GetMSfromSS(tSubSkill);
    PMessage* tmpMsg_setSLv = MsgBuilder->BuildSubskillIncMsg(temp_target, tSubSkill, temp_target->GetChar()->Skill->GetSP(tMainSkill));
    temp_target->SendUDPMessage(tmpMsg_setSLv);
    tmpMsg_setSLv = NULL;
    temp_target = NULL;
}
