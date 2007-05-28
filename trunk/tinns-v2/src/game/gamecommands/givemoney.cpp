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

void PCommands::doCmdgivemoney()
{
    u32 cashtoadd = 0;
    bool SyntaxError = false;
    if(ArgC < 1)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == false)
    {
        SyntaxError = true;
    }
    else
    {
        cashtoadd = (u32)GetArgInt(1);
    }
    if(cashtoadd == 0)
        return;

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@givemoney <amount> [<charID or nickname>]");
        return;
    }

    if(ArgC == 2)
    {
        if(IsArgNumeric(2) == true)
        {
            target = GetClientByID(GetArgInt(2));
        }
        else
        {
            char tmp_destNick[50];
            GetArgText(2, tmp_destNick, 50);
            target = GetClientByNick(tmp_destNick);
        }
        if(target == NULL)
        {
            Chat->send(source, CHAT_DIRECT, "System", "No such player");
            return;
        }
        if(source->GetAccountLevel() <= target->GetAccountLevel())
        {
            char tmpMsg[200];
            snprintf(tmpMsg, 199, "Cant manipulate %s's credits, target level is higher or equal to yours!", Chars->GetChar(target->GetCharID())->GetName().c_str());
            tmpMsg[199] = '\0';
            Chat->send(source, CHAT_DIRECT, "System", tmpMsg);
            return;
        }

        u32 oldcashval = target->GetChar()->GetCash();
        u32 newcashvalue = target->GetChar()->SetCash(oldcashval + cashtoadd);
        u32 diffcash = newcashvalue - oldcashval;

        PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg(target, newcashvalue);
        target->getUDPConn()->SendMessage(tmpMsg_cash);
        tmpMsg_cash = NULL;

        char tmpMsg_success[200];
        snprintf(tmpMsg_success, 199, "Added %d NC to %s's wallet (Has now %d NC)", diffcash, target->GetChar()->GetName().c_str(), newcashvalue);
        tmpMsg_success[199] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", tmpMsg_success);
    }
    else
    {
        u32 oldcashval = source->GetChar()->GetCash();
        u32 newcashvalue = source->GetChar()->SetCash(oldcashval + cashtoadd);

        PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg(source, newcashvalue);
        source->getUDPConn()->SendMessage(tmpMsg_cash);
        tmpMsg_cash = NULL;
        //Console->Print("oldcashval: %d  newcashvalue: %d", oldcashval, newcashvalue);
    }
}
