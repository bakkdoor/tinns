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

outpost.cpp - TinNS outpost handling

CREATION: 24 Oct 2009 Namikon

*/

#include "main.h"
#include "msgbuilder.h"

POutpost::POutpost()
{
}

POutpost::~POutpost()
{
}

u32 POutpost::GetCurrentClan(u32 nOutpostID)
{
    MYSQL_RES *result = NULL;
    char tQuery[200];

    snprintf(tQuery, 200, "SELECT o_clan FROM outposts WHERE o_outnum = %d", nOutpostID);
    result = MySQL->GameResQuery(tQuery);

    if(!result)
    {
        Console->Print("%s [POutpost::GetCurrentClan] Cannot query SQL. Query was: %s", Console->ColorText(RED,BLACK,"Error"), tQuery);
        MySQL->ShowGameSQLError();
        return 0;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print("%s [POutpost::GetCurrentClan] Invalid or missing Outpost data for Outpost: %d", Console->ColorText(RED,BLACK,"Error"), nOutpostID);
        return 0;
    }

    u32 tClanID = atoi(mysql_fetch_row(result)[0]);

    MySQL->FreeGameSQLResult(result);
    return tClanID;
}

// Check if given nZoneID is an outpost area
bool POutpost::IsZoneOPArea(u32 nZoneID)
{
    // Outpost ID == WorldID
    // Outpost Underground = WorldID + 2000
    // Only process possible outpost zones and their undergrounds

    if((nZoneID > 2005 && nZoneID < 2213) || (nZoneID > 4005 && nZoneID < 4213))
    {
        const PDefWorld* tWorldDef = GameDefs->Worlds()->GetDef( nZoneID );
        if ( tWorldDef )
            return true;
    }
    return false;
}

// Sends Outpostinfo to client
void POutpost::SendOPAreaData(PClient* nClient)
{
    if(!nClient)
        return;

    MYSQL_RES *result = NULL;
    MYSQL_ROW row = NULL;
    char tQuery[200];
    int nZoneID = nClient->GetChar()->GetLocation();

    if(IsZoneOPArea(nZoneID) == false)
    {
        if (gDevDebug) Console->Print("ZoneID %d is not an outpost. Skipping", nZoneID);
        return;
    }
    // Fix underground ID
    if(nZoneID > 4000)
        nZoneID -= 2000;

    snprintf(tQuery, 200, "SELECT cl_id, cl_faction FROM clans INNER JOIN outposts ON (o_clan = cl_id) WHERE o_outnum = %d", nZoneID);
    result = MySQL->GameResQuery(tQuery);

    if(!result)
    {
        Console->Print("%s [POutpost::SendOPAreaData] Cannot query SQL. Query was: %s", Console->ColorText(RED,BLACK,"Error"), tQuery);
        MySQL->ShowGameSQLError();
        return;
    }

    if(mysql_num_rows(result) == 0)
    {
        Console->Print("%s [POutpost::SendOPAreaData] Invalid or missing Outpost data for ZoneID: %d", Console->ColorText(RED,BLACK,"Error"), nZoneID);
        return;
    }

    row = mysql_fetch_row(result);

    u32 tClanID = atoi(row[0]);
    u8 tFactionID = atoi(row[1]);

    MySQL->FreeGameSQLResult(result);

    Console->Print("ZoneID %d is an outpost. Sending Clan/Faction information... Clan: %d Faction: %d", nZoneID, tClanID, tFactionID);
    PMessage* tMsg = MsgBuilder->BuildOutpostClanInfoMsg( nClient, tClanID, tFactionID );
    nClient->SendUDPMessage( tMsg );
}
