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

 npc.cpp - Management class for NPC worldactors

 CREATION: 04 Jan 2007 Namikon

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "npc.h"

///***********************************************************************
///***********************************************************************

bool PNPC::SQL_Load()
{
    if(gDevDebug) Console->Print("[DEBUG] Now loading NPC data for NPC id %d", mID);
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `npc_spawns` WHERE `npc_id` = %d", mID);
    if(gDevDebug) Console->Print("[DEBUG] Executing query %s", query);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PNPC::SQL_Load could not load NPC definition");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return false;
    }
    if(mysql_num_rows(result) == 0)
    {
        if(gDevDebug) Console->Print("[DEBUG] No NPCs found, returning false");
        MySQL->FreeGameSQLResult(result);
        return false;
    }
    else if(mysql_num_rows(result) > 1)
    {
        Console->Print(RED, BLACK, "PNPC::SQL_Load Duplicate entry for NPC %d", mID);
        MySQL->FreeGameSQLResult(result);
        return false;
    }
    if(gDevDebug) Console->Print("[DEBUG] One NPC found for my ID. Now grabbing data...");
    row = mysql_fetch_row(result);

    mWorldID = std::atoi(row[npc_worldid]);
    mNameID = std::atoi(row[npc_nameid]);
    mTypeID = std::atoi(row[npc_typeid]);
    mClothing = std::atoi(row[npc_clothing]);
    mPosX = std::atoi(row[npc_x]);
    mPosY = std::atoi(row[npc_y]);
    mPosZ = std::atoi(row[npc_z]);
    mAngle = std::atoi(row[npc_angle]);
    mLoot = std::atoi(row[npc_loot]);

    if(row[npc_name] != NULL)
        mName = row[npc_name];

    if(row[npc_customname] != NULL)
        mCustomName = row[npc_customname];

    if(row[npc_customscript] != NULL)
        mCustomLua = row[npc_customscript];

    if(gDevDebug) Console->Print("[DEBUG] NPC: WID:%d NID:%d TID:%d CL:%d PX:%d PY:%d PZ:%d ANG:%d LT:%d NAME:%s CNAME:%s SCRIPT:%s", mWorldID, mNameID, mTypeID, mClothing, mPosX, mPosY, mPosZ, mAngle, mLoot, mName.c_str(), mCustomName.c_str(), mCustomLua.c_str());
    MySQL->FreeGameSQLResult(result);
    return true;
}


void PNPC::Die()
{
    if(gDevDebug) Console->Print("[DEBUG] NPC dying now");
    mHealth = 0;
    mDeath = true;
    mRespawn = std::time(NULL) + NPC_RESPAWN_AFTER;
    mDirty = true;
}

void PNPC::Update()
{
    if(gDevDebug) Console->Print("[DEBUG] NPC Update: Respawn timer triggered! Setting NPC back to life");
    if(std::time(NULL) >= mRespawn && mDeath == true)
    {
        mHealth = 127;
        mDeath = false;
        mDirty = true;
    }
}

PNPC::PNPC(int nSQLID)
{
    // First make sure we start over with an emptry dataset
    mID = 0;
    mWorldID = 0;
    mNameID = 0;
    mTypeID = 0;
    mClothing = 0;
    mPosX = 0;
    mPosY = 0;
    mPosZ = 0;
    mAngle = 0;
    mLoot = 0;
    mName = "";
    mCustomName = "";
    mCustomLua = "";

    mAction = 20; // 0x14: Do nothing?
    mDeath = false;
    mHealth = 127; // Max health
    mTarget = 0;
    mDirty = false; // No need to send instand update

    //if(gDevDebug) Console->Print("[DEBUG] New NPC instance created. ID is %d", nSQLID);
    mID = nSQLID;
    if(SQL_Load() == false)  // Try to load NPC contents
        mSuccess = false;
    else
        mSuccess = true;

    mDirty = false;
}

PNPC::~PNPC()
{
    //if(gDevDebug) Console->Print("[DEBUG] NPC ID %d terminated", mID);
}

u8 PNPC::GetActionStatus()
{
    if(mDeath == true)
    {
        return 128; // 128 triggers "death" animation
    }
    else
    {
        return 3; // 3 is the value found in many packets. However, no idea what this does
    }
}

///***********************************************************************
///***********************************************************************

void PNPCWorld::MSG_SendNPCs(PClient* nClient)
{
    PMessage* npc_initmsg = new PMessage(256);
    PNPC* tNPC;
    //if (gDevDebug) Console->Print("Starting to assemble NPC message");

    *npc_initmsg << (u8)0x13; // Begin UDP message
    *npc_initmsg << (u16)0x0000; // Placeholder for UDP ID
    *npc_initmsg << (u16)0x0000; // Placeholder for Session ID

    // Loop all NPCs in world
    for(PNPCMap::iterator it=mNPCs.begin(); it != mNPCs.end(); it++)
    {
        //if (gDevDebug) Console->Print("Got first NPC");
        if(!it->second) // Got an error and NPC doesnt exist? Skip this one
            continue;

        tNPC = it->second; // Get NPC

        // Get "string" for NPC angle
        std::string tAngleStr = Ssprintf("%d", tNPC->mAngle);

        // Calculate messagesize
        u8 tMsgLen = 29 + tNPC->mName.size() + tAngleStr.size();
        //if (gDevDebug) Console->Print("MsgLen will be %d bytes ", tMsgLen);
        // Check for customname
        bool tUseCustomName = false;
        if(tNPC->mCustomName.size() > 1)
        {
            //if (gDevDebug) Console->Print("NPC has an custom attached name, adding!");
            // NPC has customname, add to size
            tMsgLen += tNPC->mCustomName.size();
            tMsgLen++; // the 0x00 is not counted in .size() !
            tUseCustomName = true;
        }

        // Check if messagebuffer is full
        //if (gDevDebug) Console->Print("LEN: %d MAX: %d INUSE: %d", tMsgLen, npc_initmsg->GetMaxSize(), npc_initmsg->GetSize());
        if((int)tMsgLen > (npc_initmsg->GetMaxSize() - npc_initmsg->GetSize()))
        {
            //if (gDevDebug) Console->Print("The messagequeue is full. Sending it");
            // It is full. Now set final UDP/Session ID's and send packet
            npc_initmsg->U16Data(0x01) = nClient->GetUDP_ID();      // Set final UDP ID
            npc_initmsg->U16Data(0x03) = nClient->GetSessionID();   // Set final SessionID

            // Send message to client
            if(npc_initmsg->GetSize() > 5)
                nClient->getUDPConn()->SendMessage(npc_initmsg);
            else
                delete npc_initmsg;
            //if (gDevDebug) Console->Print("Message sent. Trying to dump");
            //if (gDevDebug) (*npc_initmsg).Dump();

            // Clear messagebuffer and ReInit for next NPCs
            //if (gDevDebug) Console->Print("Packet is in queue now, removing link to list");
            npc_initmsg = NULL;
            //if (gDevDebug) Console->Print("And getting a new msg");
            npc_initmsg = new PMessage(256);

            //if (gDevDebug) Console->Print("Preparing new message block");

            *npc_initmsg << (u8)0x13; // Begin UDP message
            *npc_initmsg << (u16)0x0000; // Placeholder for UDP ID
            *npc_initmsg << (u16)0x0000; // Placeholder for Session ID
            //if (gDevDebug) Console->Print("And starting over");
        }
        else
        {
            nClient->IncreaseUDP_ID();

            *npc_initmsg << (u8)tMsgLen;
            *npc_initmsg << (u8)0x03;
            *npc_initmsg << (u16)nClient->GetUDP_ID();
            *npc_initmsg << (u8)0x28;
            *npc_initmsg << (u8)0x00;
            *npc_initmsg << (u8)0x01;
            *npc_initmsg << (u32)tNPC->mWorldID;
            *npc_initmsg << (u16)tNPC->mTypeID;
            *npc_initmsg << (u16)tNPC->mClothing;
            *npc_initmsg << (u16)tNPC->mNameID;
            *npc_initmsg << (u16)tNPC->mPosY;
            *npc_initmsg << (u16)tNPC->mPosZ;
            *npc_initmsg << (u16)tNPC->mPosX;
            *npc_initmsg << (u8)0x00; // Was always 0x00 in all logs
            *npc_initmsg << (u8)0x57; // ??
            *npc_initmsg << (u8)0x00; // ??
            *npc_initmsg << (u8)0x6E; // Clan/FactionID
            *npc_initmsg << (u8)0x00; // Clan/FactionID
            *npc_initmsg << tNPC->mName.c_str();
            *npc_initmsg << tAngleStr.c_str();
            if(tUseCustomName == true)
                *npc_initmsg << tNPC->mCustomName.c_str();
        }

        // Remove link to NPC instance
        tNPC = NULL;
    }
    //if (gDevDebug) Console->Print("Done with mainloop. Now lets check if we have a message waiting...");
    if(npc_initmsg->GetSize() > 5) // Check if we really have an packet and not only the header
    {
        //if (gDevDebug) Console->Print("Message is waiting! Trying to send...");
        // It is full. Now set final UDP/Session ID's and send packet
        npc_initmsg->U16Data(0x01) = nClient->GetUDP_ID();      // Set final UDP ID
        npc_initmsg->U16Data(0x03) = nClient->GetSessionID();   // Set final SessionID

        // Send message to client
        if(npc_initmsg->GetSize() > 5)
            nClient->getUDPConn()->SendMessage(npc_initmsg);
        else
            delete npc_initmsg;

        //if (gDevDebug) Console->Print("Done. Now dumping packet");
        //if (gDevDebug) (*npc_initmsg).Dump();
    }
    else
    {
        //if (gDevDebug) Console->Print("No messages waiting to be send, removing message from memory");
        delete npc_initmsg;
    }
    // Free message
    //if (gDevDebug) Console->Print("Done. Removing link to list");
    npc_initmsg = NULL;
    //delete npc_initmsg;
    //if (gDevDebug) Console->Print("Allowing client to accept NPC updates");
    nClient->SetAcceptNPCUpdates(true);
}

void PNPCWorld::MSG_SendAlive(PClient* nClient)
{
    if(gDevDebug) Console->Print("[PNPCWorld::MSG_SendAlive] Sending NPC alive msg");

    PMessage* tmpNPCUpdate = new PMessage(256);
    *tmpNPCUpdate << (u8)0x13;
    *tmpNPCUpdate << (u16)0x0000; // Placeholder
    *tmpNPCUpdate << (u16)0x0000; // Placeholder
    int tMsgLen = 12;
//    int tMsgLen = 22;

    for(PNPCMap::iterator it=mNPCs.begin(); it != mNPCs.end(); it++)
    {
        if(it->second)
        {
            PNPC* tNPC = it->second;
            // Build multiframe message
            //13 31 00 a5 ff 15 1b e7 03 00 00 1f 2c 84 80 7f 6b 84 03 77 02  01 00 00 00 00 14
            //NC UDP.| Sess  Ln Cs |..NPCID..| 1F |.Y.| |.Z.| |.X.| BM ?  Hlt TrgCh ?  ?  ?  Ac

            *tmpNPCUpdate << (u8)0x11;
            *tmpNPCUpdate << (u8)0x1B;
            *tmpNPCUpdate << (u32)tNPC->mWorldID;
            *tmpNPCUpdate << (u8)0x1F;
            *tmpNPCUpdate << (u16)tNPC->mPosY;
            *tmpNPCUpdate << (u16)tNPC->mPosZ;
            *tmpNPCUpdate << (u16)tNPC->mPosX;
            *tmpNPCUpdate << (u8)0x00; // ??
            *tmpNPCUpdate << (u8)0x00; // ??
            *tmpNPCUpdate << (u8)tNPC->mHealth;
            *tmpNPCUpdate << (u8)0x00; // ??
            *tmpNPCUpdate << (u8)0x00; // ??
// Weird is: The next packet that is commented out, lets the NPCs all look into one direction...

/*
            *tmpNPCUpdate << (u8)0x15; // Message length
            *tmpNPCUpdate << (u8)0x1b;
            *tmpNPCUpdate << (u32)tNPC->mWorldID;
            *tmpNPCUpdate << (u8)0x1F;
            *tmpNPCUpdate << (u16)tNPC->mPosY;
            *tmpNPCUpdate << (u16)tNPC->mPosZ;
            *tmpNPCUpdate << (u16)tNPC->mPosX;
            *tmpNPCUpdate << (u8)tNPC->GetActionStatus();
            *tmpNPCUpdate << (u8)0x77; // ?
            *tmpNPCUpdate << (u8)tNPC->mHealth;
            *tmpNPCUpdate << (u16)tNPC->mTarget;
            *tmpNPCUpdate << (u8)0x00; // ?
            *tmpNPCUpdate << (u8)0x00; // ?
            *tmpNPCUpdate << (u8)0x00; // ?
            *tmpNPCUpdate << (u8)tNPC->mAction;
*/
            if((int)tMsgLen > (tmpNPCUpdate->GetMaxSize() - tmpNPCUpdate->GetSize()))
            {
                //if (gDevDebug) Console->Print("DEBUG: Sending part-message");

                // Check if broad or unicast
                if(nClient == NULL)
                {
                    if(tmpNPCUpdate->GetSize() > 5)
                        ClientManager->UDPBroadcast(tmpNPCUpdate, mWorldID, 0, false, true);
                    else
                        delete tmpNPCUpdate;
                }
                else
                {
                    tmpNPCUpdate->U16Data(0x01) = nClient->GetUDP_ID();      // Set final UDP ID
                    tmpNPCUpdate->U16Data(0x03) = nClient->GetSessionID();   // Set final SessionID
                    //(*tmpNPCUpdate).Dump();
                    if(tmpNPCUpdate->GetSize() > 5)
                        nClient->getUDPConn()->SendMessage(tmpNPCUpdate);
                    else
                        delete tmpNPCUpdate;
                }

                tmpNPCUpdate = NULL;
                tmpNPCUpdate = new PMessage(256);

                // ReInit message
                *tmpNPCUpdate << (u8)0x13;
                *tmpNPCUpdate << (u16)0x0000; // Placeholder
                *tmpNPCUpdate << (u16)0x0000; // Placeholder
            }
        }
    }
    // Check if broad or unicast
    if(nClient == NULL)
    {
        if(tmpNPCUpdate->GetSize() > 5)
        {
            ClientManager->UDPBroadcast(tmpNPCUpdate, mWorldID, 0, false, true);
        }
        else
        {
            delete tmpNPCUpdate;
        }
    }
    else
    {
        if(tmpNPCUpdate->GetSize() > 5)
        {
            tmpNPCUpdate->U16Data(0x01) = nClient->GetUDP_ID();      // Set final UDP ID
            tmpNPCUpdate->U16Data(0x03) = nClient->GetSessionID();   // Set final SessionID
            //(*tmpNPCUpdate).Dump();
            nClient->getUDPConn()->SendMessage(tmpNPCUpdate);
        }
        else
        {
            delete tmpNPCUpdate;
        }
    }
    tmpNPCUpdate = NULL;
}

PNPCWorld::PNPCWorld(u16 nWorldID)
{
    //if(gDevDebug) Console->Print("[DEBUG] New world got initialized! Now starting to add NPCs. (WorldID %d)", nWorldID);
    mCreation = std::time(NULL);
    //mSuccessfullInit = false;
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    snprintf(query, 100, "SELECT * FROM `npc_spawns` WHERE `npc_location` = %d", nWorldID);
    //if(gDevDebug) Console->Print("[DEBUG] Query is: %s", query);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        Console->Print(RED, BLACK, "PNPCWorld::PNPCWorld could not load NPC definition");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return;
    }
    if(mysql_num_rows(result) == 0) // No NPCs found
    {
        MySQL->FreeGameSQLResult(result);
        //if(gDevDebug) Console->Print("[NPC] No NPCs for this world found");
        return;
    }
    //if(gDevDebug) Console->Print("[DEBUG] Found NPCs, now adding!");
    PNPC* tmpNpc = NULL;
    u32 tRawID = 0;
    int tSQLID = 0;
    while((row = mysql_fetch_row(result)))
    {
        tRawID = std::atoi(row[PNPC::npc_worldid]);
        tSQLID = std::atoi(row[PNPC::npc_id]);
        tmpNpc = new PNPC(tSQLID);
        if(tmpNpc->mSuccess == true)
        {
            //if(gDevDebug) Console->Print("[DEBUG] NPC init successfull, adding to list");
            mNPCs.insert(std::make_pair(tRawID, tmpNpc));
            tmpNpc = NULL;
        }
        else
        {
            //if(gDevDebug) Console->Print("[DEBUG] NPC init failed, removing link");
            delete tmpNpc;
        }
    }
    if(gDevDebug) Console->Print("[DEBUG] Init done");
    mWorldID = nWorldID;
    MySQL->FreeGameSQLResult(result);
    //mSuccessfullInit = true;
    mLastAliveMsg = 0;
}

PNPCWorld::~PNPCWorld()
{
    if(gDevDebug) Console->Print("[DEBUG] Erasing all NPCs");
    // Erase all NPCs
    for(PNPCMap::iterator it=mNPCs.begin(); it != mNPCs.end(); it++)
    {
        delete it->second;
        mNPCs.erase(it);
    }
}

void PNPCWorld::Update()
{
    int tDirtyNPC = 0;
    // Check for dirty NPCs

    for(PNPCMap::iterator it=mNPCs.begin(); it != mNPCs.end(); it++)
        tDirtyNPC++;

    if(tDirtyNPC > 0)
    {
        PMessage* tmpNPCUpdate = new PMessage(256);
        *tmpNPCUpdate << (u8)0x13;
        *tmpNPCUpdate << (u16)0x0000; // Placeholder
        *tmpNPCUpdate << (u16)0x0000; // Placeholder

        int framecounter = 0;
        int tMsgLen = 22;

        for(PNPCMap::iterator it=mNPCs.begin(); it != mNPCs.end(); it++)
        {
            if(it->second)
            {
                if(it->second->mDirty == true)
                {
                    if(gDevDebug) Console->Print("[DEBUG] Found dirty NPC! Sending update");
                    PNPC* tNPC = it->second;
                    tNPC->mDirty = false;
                    // Build multiframe message

                    *tmpNPCUpdate << (u8)0x15; // Message length
                    *tmpNPCUpdate << (u8)0x1b;
                    *tmpNPCUpdate << (u32)tNPC->mWorldID;
                    *tmpNPCUpdate << (u8)0x1F;
                    *tmpNPCUpdate << (u16)tNPC->mPosY;
                    *tmpNPCUpdate << (u16)tNPC->mPosZ;
                    *tmpNPCUpdate << (u16)tNPC->mPosX;
                    *tmpNPCUpdate << (u8)tNPC->GetActionStatus();
                    *tmpNPCUpdate << (u8)0x77; // ?
                    *tmpNPCUpdate << (u8)tNPC->mHealth;
                    *tmpNPCUpdate << (u16)tNPC->mTarget;
                    *tmpNPCUpdate << (u8)0x00; // ?
                    *tmpNPCUpdate << (u8)0x00; // ?
                    *tmpNPCUpdate << (u8)0x00; // ?
                    *tmpNPCUpdate << (u8)tNPC->mAction;

                    if((int)tMsgLen > (tmpNPCUpdate->GetMaxSize() - tmpNPCUpdate->GetSize()))
                    {
                        if (gDevDebug) Console->Print("DEBUG: Sending part-message (got more than 30 npcs");
                        if(tmpNPCUpdate->GetSize() > 5)
                            ClientManager->UDPBroadcast(tmpNPCUpdate, mWorldID);
                        else
                            delete tmpNPCUpdate;

                        tmpNPCUpdate = NULL;
                        PMessage* tmpNPCUpdate = new PMessage(256);

                        framecounter = 0; // Clear framecounter

                        // ReInit message
                        *tmpNPCUpdate << (u8)0x13;
                        *tmpNPCUpdate << (u16)0x0000; // Placeholder
                        *tmpNPCUpdate << (u16)0x0000; // Placeholder
                        if (gDevDebug) Console->Print("DEBUG: Done. Starting over!");
                    }
                }
            }
        }
        if(tmpNPCUpdate->GetSize() > 5)
            ClientManager->UDPBroadcast(tmpNPCUpdate, mWorldID);
        else
            delete tmpNPCUpdate;

        tmpNPCUpdate = NULL;
    }
    if((mLastAliveMsg + NPC_ALIVE_MSG) <= std::time(NULL))
    {
        if (gDevDebug) Console->Print("DEBUG: Sending NPC alive messages");
        MSG_SendAlive();
        mLastAliveMsg = std::time(NULL);
    }
}

PNPC* PNPCWorld::GetNPC(u32 nNPCID)
{
    if(gDevDebug) Console->Print("[DEBUG] Searching for NPC %d in list", nNPCID);
    PNPCMap::const_iterator it = mNPCs.find(nNPCID);
	if(it == mNPCs.end()) // only if client not found in list
        return NULL;
	else
        return it->second;
}

///***********************************************************************
///***********************************************************************

PNPCManager::PNPCManager()
{
    if(gDevDebug) Console->Print("[DEBUG] NPCManager wakeup!");
    // Nothing yet
}

PNPCManager::~PNPCManager()
{
    if(gDevDebug) Console->Print("[DEBUG] NPCManager shutdown");
    for(PNPCWorldMap::iterator it=mWorlds.begin(); it != mWorlds.end(); it++)
    {
        delete it->second;
        mWorlds.erase(it);
    }
}

void PNPCManager::InitPlayer(PClient* nClient)
{
    if(gDevDebug) Console->Print("[DEBUG] Player entered zone, doing init");
    // Ok, player entered zone. First, get zone!
    u16 nZone = nClient->GetChar()->GetLocation();
if(gDevDebug) Console->Print("[DEBUG] Now searching for zone %d in list", nZone);
    // Search worldmanager for this zone
    PNPCWorld* tmpWorld = GetWorld(nZone);

    if(tmpWorld != NULL)
    {
        if(gDevDebug) Console->Print("[DEBUG] World found, poking MSG_SendNPCs");
        // World found? Fine. Then poke the class to send its content to the client
        tmpWorld->MSG_SendNPCs(nClient);
    }
    else
    {
        if(gDevDebug) Console->Print("[DEBUG] World not found, creating....");
        // World not found! OMG! Create it!
        tmpWorld = InitWorld(nZone);
        if(tmpWorld == NULL)
        {
            Console->Print("%s Unable to init NPCs for world %d", Console->ColorText(RED, BLACK, "[ERROR]"), nZone);
            return;
        }
        if(gDevDebug) Console->Print("[DEBUG] Done. Poking MSG_SendNPCs");
        // now we have the world, poke it to send its content
        tmpWorld->MSG_SendNPCs(nClient);
        tmpWorld->MSG_SendAlive(nClient); // Force instand-update of NPCs for this client
    }
}

PNPCWorld* PNPCManager::InitWorld(u16 nWorldID)
{
    if(gDevDebug) Console->Print("[DEBUG] InitWorld triggered: ID %d", nWorldID);
    PNPCWorld* tmpWorld = NULL;
    tmpWorld = new PNPCWorld(nWorldID);
    if(tmpWorld != NULL)
    {
        if(gDevDebug) Console->Print("[DEBUG] World created. Adding to list...");
        mWorlds.insert(std::make_pair(nWorldID, tmpWorld));
        if(gDevDebug) Console->Print("[DEBUG] ... and returning instance");
        return tmpWorld;
    }
    else
    {
        if(gDevDebug) Console->Print("[DEBUG] Failed to init world. Returning NULL");
        return NULL;
    }
}

void PNPCManager::Update()
{
    static std::time_t lastdebug = std::time(NULL);
    // Loop all worlds
//    if(lastdebug < std::time(NULL))
//        if(gDevDebug) Console->Print("[DEBUG] WorldLoop still running...");

    for(PNPCWorldMap::iterator it=mWorlds.begin(); it != mWorlds.end(); it++)
    {
        // Make sure target still exists
        if(it->second)
        {
            // Getworld, and check if zone is in use
            PNPCWorld* tWorld = it->second;
            if(ClientManager->IsWorldInUse(tWorld->mWorldID) == true)
            {
                //if(lastdebug < std::time(NULL))
                //    if(gDevDebug) Console->Print("[DEBUG] World is in use, poking management class to update itself");
                // World is in use. Now send "dirty" npcs or alive messages
                tWorld->Update();
            }
            else
            {
                //if(lastdebug < std::time(NULL))
                //    if(gDevDebug) Console->Print("[DEBUG] World not in use. Checking lifetimer...");

                if((tWorld->mCreation + ZONE_RESET_AFTER) <= std::time(NULL))
                //if(tWorld->mCreation <= std::time(NULL))
                {
                    if(gDevDebug) Console->Print("[DEBUG] World reached ZONE_RESET timeout. Erasing...");
                    // World is unused sine ZONE_RESET_AFTER minutes, deleting now
                    delete it->second;
                    mWorlds.erase(it);
                }
                else
                {
                    // Keep LastAliveMessage up to date until someone enters zone
                    tWorld->mLastAliveMsg = std::time(NULL);
                  //  if(lastdebug < std::time(NULL))
                  //      if(gDevDebug) Console->Print("[DEBUG] World still within ZONE_RESET timeout");
                }
            }
        }
    }
    if(lastdebug < std::time(NULL))
    {
        lastdebug = std::time(NULL) + 3;
        //if(gDevDebug) Console->Print("[DEBUG] next updateloopmsg in 3 seconds");
    }
}

PNPCWorld* PNPCManager::GetWorld(u16 nWorldID)
{
    if(gDevDebug) Console->Print("[DEBUG] Trying to get instance for worldid %d", nWorldID);
    PNPCWorldMap::const_iterator it = mWorlds.find(nWorldID);
	if(it == mWorlds.end())
	{
	    if(gDevDebug) Console->Print("[DEBUG] Not found, returning NULL");
        return NULL;
	}
	else
	{
	    if(gDevDebug) Console->Print("[DEBUG] Found. Returning address");
        return it->second;
	}
}
