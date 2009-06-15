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



/*
	terminal.cpp - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)

	MODIFIED: 08 Jan 2007 Namikon
	REASON: - Created
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "include/terminal.h"


void PTerminal::SendNewMailNotice(PClient* nClient, u8 nNewMails)
{
    PMessage* tmpMsg = new PMessage(18);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u16)nClient->GetSessionID();
    *tmpMsg << (u8)0x0c;
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u8)0x1f;
    *tmpMsg << (u16)nClient->GetLocalID();
    *tmpMsg << (u8)0x3d;
    *tmpMsg << (u8)0x0c;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x00;
    *tmpMsg << nNewMails;

    nClient->SendUDPMessage(tmpMsg);
}

u8 PTerminal::GetNewEmailCount(PClient* nClient)
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT count(*) FROM emails WHERE e_toid = %d AND e_new = 1", nClient->GetCharID());
    if(gDevDebug) Console->Print("[DEBUG] Query is: %s", query);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PTerminal::GetNewEmailCount could not get emailcount");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    if(mysql_num_rows(result) == 0) // No Mails found
    {
        MySQL->FreeGameSQLResult(result);
        if(gDevDebug) Console->Print("[DEBUG] No new emails for this char");
        return 0;
    }
    if(gDevDebug) Console->Print("[DEBUG] Found new email, sending notice");

    row = mysql_fetch_row(result);
    u8 tRetVal = (u8)atoi(row[0]);
    MySQL->FreeGameSQLResult(result);

    SendNewMailNotice(nClient, tRetVal);
    return tRetVal;
}

