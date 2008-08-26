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

 npc.h - Management class for NPC worldactors

 CREATION: 04 Jan 2007 Namikon

 MODIFIED:
 REASON: -

*/

#ifndef NPC_H
#define NPC_H

// How many seconds have to pass until the zone gets reset?
// Reset in: NPCs and PWorld object gets deleted and reloaded when
// next player syncs to that zone.
#define ZONE_RESET_AFTER 600

// How many seconds have to pass until the NPC is "revived"
#define NPC_RESPAWN_AFTER 300

// How many seconds have to pass until we need an NPC "keepalive" packet?
#define NPC_ALIVE_MSG 15

class PNPC;
class PNPCWorld;

typedef std::map<u32, PNPC*> PNPCMap;
typedef std::map<u16, PNPCWorld*> PNPCWorldMap;


class PNPC
{
    private:
        // SQL layout
        enum
        {
            npc_id,
            npc_worldid,
            npc_nameid,
            npc_typeid,
            npc_name,
            npc_location,
            npc_x,
            npc_y,
            npc_z,
            npc_angle,
            npc_clothing,
            npc_loot,
            npc_customname,
            npc_customscript
        };

        // SQL values
        u32 mID;
        u32 mWorldID;
        u16 mNameID;
        u16 mTypeID;
        u16 mClothing;
        u16 mPosX;
        u16 mPosY;
        u16 mPosZ;
        s8 mAngle;
        u16 mLoot;
        std::string mName;
        std::string mCustomName;
        std::string mCustomLua;

        std::time_t mRespawn;    // Respawn timer

        // Runtime values
        u8 mAction;         // Current action
        bool mDeath;        // Death...
        u8 mHealth;         // NPC Health
        u16 mTarget;        // Current focused player
        bool mDirty;        // Needs update to clients

        u8 GetActionStatus();
        // Looks like we have a bitmask. However, only 2 are 100% identified yet
        // 00000001 (  1): Stand normal (?)
        // 00000010 (  2): Stand normal (?)
        // 00000100 (  4): Stand normal (?)
        // 00001000 (  8): Stand normal (?)
        // 00010000 ( 16): Ducked
        // 00100000 ( 32): Stand normal (?)
        // 01000000 ( 64): Stand normal (?)
        // 10000000 (128): Death

        bool SQL_Load();
        bool mSuccess;

        PNPC(int nSQLID);
        ~PNPC();

    public:
        friend class PNPCWorld;
        inline void Attack(PClient* nClient) { mTarget = nClient->GetChar()->GetID(); mDirty = true; }
        inline void Attack(u16 nLocalCharID) { mTarget = nLocalCharID; mDirty = true; }
        inline void Move(u16 nNewX, u16 nNewY, u16 nNewZ) { mPosX = nNewX; mPosY = nNewY; mPosZ = nNewZ; mDirty = true; }

        void Die(); // ... die?
        void Update(); // Check respawn timer
};

// *****************************************

class PNPCWorld
{
    private:
        std::time_t mCreation;  // Creation time. (Required to check zone-reset timer
        std::time_t mLastAliveMsg;  // Time of last "ping" message to keep NPCs in world

        PNPCMap mNPCs;
        PNPCMap::iterator GetNPCListBegin() { return mNPCs.begin(); }
        PNPCMap::iterator GetNPCListEnd() { return mNPCs.end(); }

        bool mSuccessfullInit;

        u16 mWorldID;

        PNPCWorld(u16 nWorldID);
        ~PNPCWorld();

        void Update();

        // Send all NPCs to one player (Initial zone setup)
        void MSG_SendNPCs(PClient* nClient);

        // Send "alive" message for all NPCs as zone broadcast to everyone or as unicast if nClient is given
        void MSG_SendAlive(PClient* nClient = NULL);


    public:
        friend class PNPCManager;
        PNPC* GetNPC(u32 nNPCID);
};

// *****************************************

class PNPCManager
{
private:
    PNPCWorldMap mWorlds;
    PNPCWorldMap::iterator GetWorldListBegin() { return mWorlds.begin(); }
    PNPCWorldMap::iterator GetWorldListEnd() { return mWorlds.end(); }

public:
    PNPCManager();
    ~PNPCManager();

    void Update();
    PNPCWorld* InitWorld(u16 nWorldID);

    PNPCWorld* GetWorld(u16 nWorldID);
    void InitPlayer(PClient* nClient); // Player is entering zone
};

#endif
