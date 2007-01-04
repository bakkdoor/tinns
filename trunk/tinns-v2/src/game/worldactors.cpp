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

 worldactors.cpp - Management class for dynamic worldactors

 CREATION: 02 Jan 2007 Namikon

 MODIFIED:
 REASON: -


*/

#include "main.h"
#include "worldactors.h"

PWorldActors::PWorldActors()
{}

PWorldActors::~PWorldActors()
{}

void PWorldActors::SpawnWA(u32 nWorld, u16 nActorID, u16 nFunctionID, u32 nWOID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotZ)
{
    PMessage* tmpMsg = new PMessage(29);
    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)0x0000; // UDP placeholder
    *tmpMsg << (u16)0x0000; // Session placeholder
    *tmpMsg << (u8)0x16;    // Message length
    *tmpMsg << (u8)0x03;    // 0x03 commandset
    *tmpMsg << (u16)0x0000; // UDP placeholder
    *tmpMsg << (u8)0x1b;    // Subcommandset
    *tmpMsg << nWOID;  // WorldobjectID

    *tmpMsg << (u8)0x19;    // Positiondata follows
    *tmpMsg << nPosY;
    *tmpMsg << nPosZ;
    *tmpMsg << nPosX;

    *tmpMsg << nRotY;    // Rotation X
    *tmpMsg << nRotZ;    // Rotation Y
    *tmpMsg << nRotX;    // Rotation Z
    *tmpMsg << nActorID;
    *tmpMsg << nFunctionID;
    ClientManager->UDPBroadcast(tmpMsg, nWorld);
}

void PWorldActors::VanishWA(u32 nWorld, u32 nWAid)
{
    PMessage* tmpMsg = new PMessage(14);

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)0x0000; // UDP ID placeholder
    *tmpMsg << (u16)0x0000; // SessionID placeholder
    *tmpMsg << (u8)0x08;    // Len (static, always 0x08
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)0x0000; // Sub UDP ID placeholder
    *tmpMsg << (u8)0x26;    // Command FADE AWAY CHAR (kinda ^^)
    *tmpMsg << (u32)nWAid;  // WorldobjectID
    ClientManager->UDPBroadcast(tmpMsg, nWorld);
}

u32 PWorldActors::GetNextFreeWAID()
{
    if (gDevDebug) Console->Print("DEBUG: Getting next free worldactor ID...");
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    sprintf(query, "SELECT DISTINCT `wa_actor_id` FROM `world_actors` ORDER BY `wa_actor_id` DESC LIMIT 1");
    if (gDevDebug) Console->Print("DEBUG: Executing query %s", query);

    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        if (gDevDebug) Console->Print("DEBUG: Error in SQL query");
        Console->Print("%s Error in MySQL query: %s", Console->ColorText(RED, BLACK, "[Error]") ,query);
        MySQL->ShowGameSQLError();
        return 0;
    }

    row = mysql_fetch_row(result);
    u32 newWAid;
    if(mysql_num_rows(result) == 1)
        newWAid = std::atoi(row[wa_actor_id] + 1);
    else
        newWAid = DYNACTORIDSTART;

    if(newWAid < DYNACTORIDSTART)
        newWAid = DYNACTORIDSTART;

    MySQL->FreeGameSQLResult(result);

    return newWAid;
}

void PWorldActors::InitWorld(PClient* nClient)
{
    if (gDevDebug) Console->Print("DEBUG: Initializing WorldActors for client...");
    u32 tZone = nClient->GetChar()->GetLocation();
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    sprintf(query, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d", tZone);
    if (gDevDebug) Console->Print("DEBUG: Executing query: %s", query);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        if (gDevDebug) Console->Print("DEBUG: SQL Error");
        Console->Print("%s Error in MySQL query: %s", Console->ColorText(RED, BLACK, "[Error]") ,query);
        MySQL->ShowGameSQLError();

        return;
    }
    if(mysql_num_rows(result) == 0)
    {
        if (gDevDebug) Console->Print("DEBUG: No dynamic actors found, skipping");
        return;
    }

    if (gDevDebug) Console->Print("DEBUG: We have dynamic actors, building message...");
    PMessage* tmpActorSpawn = new PMessage(512);
    *tmpActorSpawn << (u8)0x13;
    *tmpActorSpawn << (u16)0x0000; // Placeholder
    *tmpActorSpawn << (u16)0x0000; // Placeholder

    int framecounter = 0;
    while((row = mysql_fetch_row(result)))
    {
        // Build multiframe message
        nClient->IncreaseUDP_ID();
        *tmpActorSpawn << (u8)0x16;
        *tmpActorSpawn << (u8)0x03;
        *tmpActorSpawn << (u16)nClient->GetUDP_ID();
        *tmpActorSpawn << (u8)0x1b;
        *tmpActorSpawn << (u32)std::atoi(row[wa_actor_id]);
        *tmpActorSpawn << (u8)0x19;
        *tmpActorSpawn << (u16)std::atoi(row[wa_posY]);
        *tmpActorSpawn << (u16)std::atoi(row[wa_posZ]);
        *tmpActorSpawn << (u16)std::atoi(row[wa_posX]);
        *tmpActorSpawn << (u8)std::atoi(row[wa_rotY]);
        *tmpActorSpawn << (u8)std::atoi(row[wa_rotZ]);
        *tmpActorSpawn << (u8)std::atoi(row[wa_rotX]);
        *tmpActorSpawn << (u16)std::atoi(row[wa_actor_model]);
        *tmpActorSpawn << (u16)std::atoi(row[wa_actor_type]);
        framecounter++;
        if(framecounter == 23) // We reached 23 frames, send packet out, make it empty and continue
        {
            if (gDevDebug) Console->Print("DEBUG: Sending part-message (got more than 23 actors");
            tmpActorSpawn->U16Data(0x01) = nClient->GetUDP_ID();  // Set final UDP ID
            tmpActorSpawn->U16Data(0x03) = nClient->GetSessionID();  // Set final SessionID
            nClient->getUDPConn()->SendMessage(tmpActorSpawn);

            framecounter = 0; // Clear framecounter

            // ReInit message
            if (gDevDebug) Console->Print("DEBUG: Clearing message...");
            (*tmpActorSpawn).Clear();
            *tmpActorSpawn << (u8)0x13;
            *tmpActorSpawn << (u16)0x0000; // Placeholder
            *tmpActorSpawn << (u16)0x0000; // Placeholder
            if (gDevDebug) Console->Print("DEBUG: Done. Starting over!");
        }
    }
    tmpActorSpawn->U16Data(0x01) = nClient->GetUDP_ID();  // Set final UDP ID
    tmpActorSpawn->U16Data(0x03) = nClient->GetSessionID();  // Set final SessionID



    if (gDevDebug) Console->Print("DEBUG: Sending final message");
    nClient->getUDPConn()->SendMessage(tmpActorSpawn);
    if (gDevDebug) Console->Print("DEBUG: Done initializing zone");
}

u32 PWorldActors::AddWorldActor(PClient* nClient, u16 nActorID, u16 nFuncID, u16 nOpt1, u16 nOpt2, u16 nOpt3)
{
    PChar *tChar = nClient->GetChar();
    u16 tPosX = tChar->Coords.mX + 768;
    u16 tPosY = tChar->Coords.mY + 768;
    u16 tPosZ = tChar->Coords.mZ + 768;
    u16 tLoc = tChar->GetLocation();

    if (gDevDebug) Console->Print("DEBUG: Adding worldactor %d function %d to world %d", nActorID, nFuncID, tLoc);
    return AddWorldActor(tLoc, nActorID, nFuncID, tPosX, tPosY, tPosZ, (u8)194, (u8)128, (u8)128, nOpt1, nOpt2, nOpt3);
}

u32 PWorldActors::AddWorldActor(u32 nWorldID, u16 nActorID, u16 nFuncID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotZ, u16 nOpt1, u16 nOpt2, u16 nOpt3)
{
    if (gDevDebug) Console->Print("DEBUG: Adding worldactor: Getting next free WorldActorID");
    u32 tNextWAID = GetNextFreeWAID(); // Grab next free WorldActorID
    if (gDevDebug) Console->Print("DEBUG: Next ID will be: %d", tNextWAID);

    char query[512];
    snprintf(query, 512, "INSERT INTO `world_actors`(wa_actor_id,wa_actor_map,wa_actor_model,wa_actor_type,wa_posX,wa_posY,wa_posZ,wa_rotX,wa_rotY,wa_rotZ,wa_option1,wa_option2,wa_option3)VALUES('%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d','%d')",tNextWAID, nWorldID, nActorID, nFuncID, nPosX, nPosY, nPosZ, nRotX, nRotY, nRotZ, nOpt1, nOpt2, nOpt3);
    if (gDevDebug) Console->Print("DEBUG: Executing SQL query %s", query);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "WorldActors::AddWorldActor could not add worldactor to database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    if (gDevDebug) Console->Print("DEBUG: Successfully added WorldActor to Database");

    // Now spawn the actor right away!
    SpawnWA(nWorldID, nActorID, nFuncID, tNextWAID, nPosX, nPosY, nPosZ, nRotX, nRotY, nRotX);

    return tNextWAID;
}

void PWorldActors::DelWorldActor(PClient* nClient, u32 nWAid)
{
    char query[200];
    u16 tLoc = nClient->GetChar()->GetLocation();
    if (gDevDebug) Console->Print("DEBUG: Received request to remove worldactor %d from world %d", nWAid, tLoc);

    sprintf(query, "DELETE FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", tLoc, nWAid);
    if (gDevDebug) Console->Print("DEBUG: Executing query %s", query);
    if(MySQL->GameQuery(query))
        Console->Print("Error while deleting worldactorID %d in world %d", nWAid, tLoc);

    VanishWA(tLoc, nWAid);
}

void PWorldActors::GetWAoption(u32 nWAid, u16 nWorld, u16 &nValue1, u16 &nValue2, u16 &nValue3)
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[200];

    nValue1 = 0;
    nValue2 = 0;
    nValue3 = 0;
    sprintf(query, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", nWorld, nWAid);

    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetWAoption could not get WorldActor options");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0)
        return;

    row = mysql_fetch_row(result);
    nValue1 = std::atoi(row[wa_option1]);
    nValue2 = std::atoi(row[wa_option2]);
    nValue3 = std::atoi(row[wa_option3]);

    MySQL->FreeGameSQLResult(result);
}

int PWorldActors::GetWASQLID(u32 nWAid, u32 nWorld)
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[200];

    sprintf(query, "SELECT * FROM `world_actors` WHERE `wa_actor_map` = %d AND `wa_actor_id` = %d", nWorld, nWAid);

    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PWorldActors::GetWASQLID could not get WorldActor SQL ID");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return -1;
    }
    if(mysql_num_rows(result) == 0)
        return 0;

    row = mysql_fetch_row(result);
    int tWAid = std::atoi(row[wa_id]);
    MySQL->FreeGameSQLResult(result);

    return tWAid;
}

bool PWorldActors::IsDynamicActor(u32 nWAid)
{
    MYSQL_RES *result = NULL;
    char query[200];

    sprintf(query, "SELECT * FROM `world_actors` WHERE `wa_actor_id` = %d", nWAid);

    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PWorldActors::IsDynamicActor could not check if worldactor is dynamic");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    if(mysql_num_rows(result) == 0)
        return false;
    else if(mysql_num_rows(result) == 1)
        return true;
    else if(mysql_num_rows(result) > 1)
    {
        Console->Print("%s Duplicate entry for WorldActorID %d found", Console->ColorText(YELLOW, BLACK, "[Notice]"), nWAid);
        return true;
    }
    return false;
}
