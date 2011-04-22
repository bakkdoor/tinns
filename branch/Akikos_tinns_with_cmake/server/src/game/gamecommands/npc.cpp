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

void PCommands::doNPC()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@npc <what> [<worldID>]");
        Chat->send(source, CHAT_DIRECT, "Usage", " <what> can be:");
        Chat->send(source, CHAT_DIRECT, "lua <id> 1/0", "Enables/Disables scripts");
        Chat->send(source, CHAT_DIRECT, "reloadlua <id>", "Reloads LUA script");
        Chat->send(source, CHAT_DIRECT, "spawn <id> [<name>]", "Spawns an NPC");
        Chat->send(source, CHAT_DIRECT, "remove <id>", "Removes an NPC");
        Chat->send(source, CHAT_DIRECT, "help <cmd>", "Displays detailed help for any command");

        /* more to add here... spawn, remove, ... */
        return;
    }
    // Get subcommand
    char tmp_npccommand[50];
    GetArgText(1, tmp_npccommand, 50);

    // Dont search for NPC ID when commands HELP and SPAWN are used
    PNPC* targetNPC = NULL;
    PNPCWorld* currentNPCWorld = NULL;
    if(strncmp(tmp_npccommand, "spawn", 5) != 0 && strncmp(tmp_npccommand, "help", 4) != 0)
    {
        currentNPCWorld = NPCManager->GetWorld( source->GetChar()->GetLocation() );
        if ( currentNPCWorld )
        {
            targetNPC = currentNPCWorld->GetNPC( GetArgInt(2) );
            if(!targetNPC)
            {
                // Search for DEF version of NPC (remember, def IDs are on 255 offset!
                // Note to myself: This is UGLY!!!! and BAD!!! but it works for now. CHANGE THIS!
                targetNPC = currentNPCWorld->GetNPC( GetArgInt(1) - 255 );
            }
        }
        if (!targetNPC)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Invalid NPC ID");
            return;
        }
    }

    if(strncmp(tmp_npccommand, "help", 4) == 0)
    {
        char tmp_help[50];
        GetArgText(2, tmp_help, 50);

        if(strncmp(tmp_help, "reloadlua", 9) == 0)
        {
            Chat->send(source, CHAT_DIRECT, "reloadlua", "Forces the server to reload any LUA script attached to the NPC");
            return;
        }
        else if(strncmp(tmp_help, "spawn", 5) == 0)
        {
            Chat->send(source, CHAT_DIRECT, "spawn", "Spawns an NPC on your current location");
            Chat->send(source, CHAT_DIRECT, "Params", "<spawnID> - See npc.def");
            Chat->send(source, CHAT_DIRECT, "Params", "[<Name>] - Optional name for NPC");
            return;
        }
        else if(strncmp(tmp_help, "remove", 6) == 0)
        {
            Chat->send(source, CHAT_DIRECT, "remove", "Removes an NPC");
            Chat->send(source, CHAT_DIRECT, "Params", "<worldID> - Use @debug it to get ID");
            Chat->send(source, CHAT_DIRECT, "Params", "[keep] - Dont delete NPC from SQL");
            return;
        }
        else if(strncmp(tmp_help, "help", 4) == 0)
        {
            Chat->send(source, CHAT_DIRECT, "help", "Yes... help...");
            return;
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "help", "No such command");
            return;
        }
    }
    else if(strncmp(tmp_npccommand, "spawn", 5) == 0)
    {
// ------------------------------
        PNPCWorld* tNPCWorld = NPCManager->GetWorld( source->GetChar()->GetLocation() );
        if (!tNPCWorld)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error NPC World is inactive");
            Console->Print("%s Error NPC World is inactive but players are there!", Console->ColorText(RED,BLACK, "[PANIC]"));
            return;
        }
        // Get requested spawn ID from Input
        // @npc spawn 12
        int tSpawnID = GetArgInt(2);
        // Check if ID is valid
        const PDefNpc* t_defNpc = GameDefs->Npcs()->GetDef(tSpawnID);
        if(!t_defNpc)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Invalid SpawnID");
            return;
        }
        char tCustomName[80];
        tCustomName[0] = '\0';
        if(ArgC == 3)
            GetArgText(3, tCustomName, 80);

        u16 tNPCPosX = source->GetChar()->Coords.mX + 768;
        u16 tNPCPosY = source->GetChar()->Coords.mY + 768;
        u16 tNPCPosZ = source->GetChar()->Coords.mZ + 768;
        u32 tLocation = source->GetChar()->GetLocation();

        u8 tAngle = source->GetChar()->Coords.mLR;
        string tNPCAngle = Ssprintf( "%d", tAngle );

        string tNPCScript = t_defNpc->GetStandardScript();
        u32 tNPCHealth = t_defNpc->GetHealth() * NPC_HEALTHFACTOR;

        /*-------------------------------------------------------*/
        // Get the highest NPC Id for current zone
        /*-------------------------------------------------------*/
        MYSQL_RES *result = NULL;
        char tSql[100];
        snprintf(tSql, 100, "SELECT IFNULL(MAX(npc_worldid)+1,0) FROM npc_spawns WHERE npc_location = %d", tLocation);
        result = MySQL->GameResQuery(tSql);
        if(!result)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB insert, NPC not added");
            MySQL->ShowGameSQLError();
            return;
        }

        int tNewWorldID = atoi(mysql_fetch_row(result)[0]);

        if(tNewWorldID == 0)
            tNewWorldID = NEW_NPC_ZONEID_START;

        /*-------------------------------------------------------*/
        // Insert NPC into DB
        /*-------------------------------------------------------*/
        char tSql2[500];
        snprintf(tSql2, 500, "INSERT INTO npc_spawns (npc_worldid, npc_nameid, npc_typeid, npc_name, npc_location, npc_x, npc_y, npc_z, npc_angle, npc_clothing, npc_loot, npc_unknown, npc_trader, npc_customname) VALUES (%d, %d, %d, \"%s\", %d, %d, %d, %d, %d, %d, %d, %d, %d, \"%s\")",
            tNewWorldID, tSpawnID, GetRandom(65000, 1000), tNPCScript.c_str(), tLocation, tNPCPosX, tNPCPosY, tNPCPosZ, atoi(tNPCAngle.c_str()),
            GetRandom(65000, 1000), t_defNpc->GetLoot(), tNPCHealth, 0, tCustomName);

        if(MySQL->GameQuery(tSql2))
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB insert, NPC not added");
            MySQL->ShowGameSQLError();
            return;
        }
        /*-------------------------------------------------------*/
        // Grab last insert ID to tell NPCWorld later
        /*-------------------------------------------------------*/
        int tNPC_SQLID = MySQL->GetLastGameInsertId();
        if(tNPC_SQLID == 0)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB insert, please check your DB");
            Console->Print("%s LastInsertID is 0 without SQL Error after query", Console->ColorText(RED,BLACK, "[PANIC]"));
            return;
        }
        /*-------------------------------------------------------*/
        // Tell NPCWorld to load NPC with given SQL
        /*-------------------------------------------------------*/
        tNPCWorld->AddNPC(tNPC_SQLID, tNewWorldID);

        char retMsg[100];
        snprintf(retMsg, 100, "NPC spawned. WorldID: %d", tNewWorldID);
        Chat->send(source, CHAT_DIRECT, "System", retMsg);
        return;
// ------------------------------
    }
    else if(strncmp(tmp_npccommand, "remove", 6) == 0 && targetNPC && currentNPCWorld)
    {
        char tmp_option[50];
        if(ArgC == 3)
            GetArgText(3, tmp_option, 50);

        if(strncmp(tmp_option, "keep", 4) != 0)
        {
            if(targetNPC->IsSQLNPC() == true)
            {
                char sql[100];
                char sql2[100];
                snprintf(sql, 100, "DELETE FROM npc_spawns WHERE npc_id = %d", targetNPC->GetNPCSQLID());
                snprintf(sql2, 100, "DELETE FROM npc_shop WHERE c_npc_id = %d and c_zoneid = %d", targetNPC->GetNPCID(), source->GetChar()->GetLocation());
                if(MySQL->GameQuery(sql))
                {
                    Chat->send(source, CHAT_DIRECT, "System", "Unable to remove NPC from SQL");
                    Console->Print("%s Unable to remove NPC from SQL", Console->ColorText(RED, BLACK, "[Error]"));
                    MySQL->ShowGameSQLError();
                }
                if(MySQL->GameQuery(sql2))
                {
                    Chat->send(source, CHAT_DIRECT, "System", "Unable to remove NPC Shop settings from SQL");
                    Console->Print("%s Unable to remove NPC shop settings from SQL", Console->ColorText(RED, BLACK, "[Error]"));
                    MySQL->ShowGameSQLError();
                }
            }
        }
        Chat->send(source, CHAT_DIRECT, "System", "Removing NPC...");
        currentNPCWorld->DelNPC(targetNPC->GetNPCID());
        return;
    }
    else if(strncmp(tmp_npccommand, "reloadlua", 9) == 0 && targetNPC)
    {
        if(targetNPC->ReloadLUAScript() == true)
        {
            Chat->send(source, CHAT_DIRECT, "System", "LUA script reload successfull");
            return;
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "LUA script reload failed");
            return;
        }
    }
    else if(strncmp(tmp_npccommand, "lua", 3) == 0 && targetNPC)
    {
        if(ArgC < 3)
        {
            Chat->send(source, CHAT_DIRECT, "Syntax", "@npc <id> lua 1/0");
            return;
        }
        if(IsArgNumeric(3) == false)
        {
            Chat->send(source, CHAT_DIRECT, "Syntax", "@npc <id> lua 1/0");
            return;
        }

        int tVal = GetArgInt(3);
        targetNPC->SetScripting(tVal);


        if(targetNPC->IsSQLNPC() == true)
        {
            char sql[100];
            snprintf(sql, 100, "UPDATE npc_spawns SET npc_scripting = %d WHERE npc_id = %d", tVal, targetNPC->GetNPCSQLID());
            if(MySQL->GameQuery(sql))
            {
                Chat->send(source, CHAT_DIRECT, "System", "Unable to set scripting value in SQL");
                Console->Print("%s Unable to set scripting value in SQL", Console->ColorText(RED, BLACK, "[Error]"));
                MySQL->ShowGameSQLError();
            }
        }

        if(tVal == 1)
            Chat->send(source, CHAT_DIRECT, "System", "LUA scripting is now enabled");
        else
            Chat->send(source, CHAT_DIRECT, "Syntax", "LUA scripting is now disabled");

        return;
    }
    else
    {
        char buff[100];
        snprintf(buff, 100, "Unknown command: '%s'", tmp_npccommand);
        Chat->send(source, CHAT_DIRECT, "System", buff);
        return;
    }
}
