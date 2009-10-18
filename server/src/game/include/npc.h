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

 MODIFIED: 28 Apr 2009 Hammag
 REASON: changed worlds Id from u16 (wrong !!!) to u32

*/

#ifndef NPC_H
#define NPC_H

// Healthfactor for NPCs (see old npc.def)
#define NPC_HEALTHFACTOR 15

// Minimum time in seconds that has to pass before an NPC
// gets his heartbeat send
#define NPC_HEARTBEAT_MIN 5
// Maximum time in seconds that is allowed to pass without
// an NPC heartbeat
#define NPC_HEARTBEAT_MAX 20

// If no custom NPC is set in this Zone, what ID to start with?
#define NEW_NPC_ZONEID_START 1000

// How many seconds have to pass until the zone gets reset?
// Reset in: NPCs and PWorld object gets deleted and reloaded when
// next player syncs to that zone.
#define ZONE_RESET_AFTER 600

// How many seconds have to pass until the NPC is "revived"
#define NPC_RESPAWN_AFTER 300

// How many seconds have to pass until we need an NPC "keepalive" packet?
#define NPC_ALIVE_MSG 15

// How often a NPC should check if an enemy is nearby?
#define NPC_ENEMYCHECK 5

#define NPC_ACTIONSTATE_SITGND  0x00
#define NPC_ACTIONSTATE_ATTACK  0x01
//#define NPC_ACTIONSTATE_?       0x02
//#define NPC_ACTIONSTATE_?       0x04
//#define NPC_ACTIONSTATE_?       0x08
#define NPC_ACTIONSTATE_KNEEL   0x10
#define NPC_ACTIONSTATE_PASSIVE 0x20
#define NPC_ACTIONSTATE_IDLE    0x40
#define NPC_ACTIONSTATE_DEATH   0x80

#define NPC_SHOOT_IDLE      15
#define NPC_SHOOT_SINGLE    16
#define NPC_SHOOT_AUTO1     17
#define NPC_SHOOT_AUTO2     18

class PNPC;
class PNPCWorld;

typedef std::map<u32, PNPC*> PNPCMap;
typedef std::map<u32, PNPCWorld*> PNPCWorldMap;

typedef struct
{
	u16 ItemID;
	u32 Price;
} stShopListEntry;

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
        npc_unknown,
        npc_trader,  // trader.def entry, or clan/faction data!
        npc_customname,
        npc_customscript,
        npc_shop_quality,
        npc_scripting
    };

    std::time_t mNextUpdate;    // Timestamp for next heartbeat
    std::time_t mNextEnemyCheck; // Timestamp for next enemycheck
    inline void PushUpdateTimer() { mNextUpdate = std::time(NULL) + GetRandom(NPC_HEARTBEAT_MAX, NPC_HEARTBEAT_MIN); };

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
    u16 mTrader;
    u8 mItemQuality; // Used for Shopping stuff
    u8 mUnknown;

    std::string mDialogScript;
    std::string mLUAFile; // Load File; Preloaded uppon NPC creation

    std::vector<stShopListEntry> mVectItemsInShop;    // We need to keep track of the itemorder for shopping
    void AddToVectorList(u16 nItemID, u32 nPrice);
    inline const stShopListEntry* GetItemNum(u32 nIdx) const { if(nIdx > mVectItemsInShop.size()) { return NULL; } else { return &mVectItemsInShop[nIdx]; }};

    bool mScripting;    // Manual override to disable scripting for an NPC TRUE: Scripts will be executed FALSE: Scripts will be ignored

    std::string mName;
    std::string mCustomName;
    std::string mCustomLua;

    std::time_t mRespawn;    // Respawn timer

    // Runtime values
    //bool mDeath;        // Death...
    u8 mFaction;        // NPC's faction

    u16 mHealth;         // NPC Current Health-Value
    u16 mMaxHealth;      // NPC Max Health value
    u32 mTarget;        // Current focused player
    bool mDirty;        // Needs update to clients

    // WorldID Fix 10.10.2009
    bool mFromDEF;      // to differ DEF NPCs from SQL NPCs
    bool mSuccess;      // NPC load successfull?


    u8 mAction;         // Current action
    inline u8 GetActionStatus() const { return mAction; };
    // 00000001 (  1) 0x01: Attack-Mode (Depends on WeaponStatus)
    // 00000010 (  2) 0x02: ?
    // 00000100 (  4) 0x04: ?
    // 00001000 (  8) 0x08: ?
    // 00010000 ( 16) 0x10: kneel
    // 00100000 ( 32) 0x20: Passive-Mode  (Depends on WeaponStatus. Difference between 0x01: NPC does NOT open fire)
    // 01000000 ( 64) 0x40: Idle
    // 10000000 (128) 0x80: Die

    u8 mWeaponStatus;
    inline u8 GetWeaponStatus() const { return mWeaponStatus; };
    // 00001111 (15) 0x0F: Follow given target with eyes / Put weapon away if pulled
    // 00010000 (16) 0x10: Pull weapon if not pulled / If pulled, attack
    // 00010001 (17) 0x11: Pull weapon and attack


    bool SQL_Load();
    bool DEF_Load(u32 nWorldID);

    PNPC( int nSQLID );
    PNPC( int nDEFID, u32 nWorldID );
    ~PNPC();

    void InitVars();
    void ContentListAddItem(PMessage* nContentList, u16 nItemID, u32 nBasePrice = 0, bool nAddToList = true);
    void ContentListAddItemGroup(PMessage* nContentList, u32 nItemGroupID);
    void StartDialog( PClient* nClient/*, string &nDialogscript */);

    bool DoSQLShoppingList( PClient* nClient, PMessage* nContentList );
    bool HasSQLShoppingList( PClient* nClient );
    bool IsAllbuyer( PClient* nClient );
    bool LoadLUAScript();

    inline u32 GetRealWorldID() { if(mFromDEF == true) return mWorldID+255; else return mWorldID; };

public:
    friend class PNPCWorld;

    inline void StopAttack() { mDirty = true; mAction = NPC_ACTIONSTATE_IDLE; mWeaponStatus = NPC_SHOOT_IDLE; };
    inline void Attack( PClient* nClient, u8 nType = NPC_SHOOT_SINGLE, u8 nUnknown = 90 ) { Attack(nClient->GetChar()->GetID(), nType, nUnknown); };
    void Attack( u32 nWorldID, u8 nType = NPC_SHOOT_SINGLE, u8 nUnknown = 90 );

    inline void Move( u16 nNewX, u16 nNewY, u16 nNewZ )
    {
        mPosX = nNewX;
        mPosY = nNewY;
        mPosZ = nNewZ;
        mDirty = true;
    }

    void Die(); // ... die?
    void Update(); // Check respawn timer
    void StartConversation( PClient* nClient );
    void DoConversation( PClient* nClient, u8 nAnswer ) ;

    // GameCommands
    bool ReloadLUAScript();
    bool ReloadShopList();
    bool SetShopQuality(u8 nNewVal);
    inline bool IsSQLNPC() const { return !mFromDEF; };
    inline int GetNPCID() const { return mWorldID; };
    inline int GetNPCSQLID() const { return mID; };
    inline void SetTrader( u16 nTraderDef ) { mTrader = nTraderDef; };
    inline void SetScripting(bool nVal) { mScripting = nVal; };
    inline u8 GetFaction() const { return mFaction; };
};

// *****************************************

class PNPCWorld
{
private:
    std::time_t mCreation;  // Creation time. (Required to check zone-reset timer
    std::time_t mLastAliveMsg;  // Time of last "ping" message to keep NPCs in world

    PNPCMap mNPCs;
    PNPCMap::iterator GetNPCListBegin()
    {
        return mNPCs.begin();
    }
    PNPCMap::iterator GetNPCListEnd()
    {
        return mNPCs.end();
    }

    bool mSuccessfullInit;

    u32 mWorldID;

    PNPCWorld( u32 nWorldID );
    ~PNPCWorld();

    void Update();

    // Send all NPCs to one player (Initial zone setup)
    void MSG_SendNPCs( PClient* nClient );

    // Send "alive" message for all NPCs as zone broadcast to everyone
    //void MSG_SendAlive();

    bool LoadNPCfromSQL();
    bool LoadNPCfromDEF();

    void BroadcastNewNPC(PNPC* nNpc);
    void CheckForEnemies(PNPC* nNPC);

public:
    friend class PNPCManager;
    PNPC* GetNPC( u32 nNPCID );

    // Functions to add/remove an NPC while server is running
    void SendSingleNPCInfo( PClient* nClient, PNPC* nNpc ); // Send
    bool AddNPC(u32 nSQL_ID, u32 nRaw_ID); // Load single SQL NPC from given SQL ID
    void DelNPC(u32 nWorldID); // Remove given NPC from list. Works for *all* npcs
                                                     // but uppon zone reset they're back.
};

// *****************************************

class PNPCManager
{
private:
    PNPCWorldMap mWorlds;
    PNPCWorldMap::iterator GetWorldListBegin()
    {
        return mWorlds.begin();
    }
    PNPCWorldMap::iterator GetWorldListEnd()
    {
        return mWorlds.end();
    }

public:
    PNPCManager();
    ~PNPCManager();

    void Update();
    PNPCWorld* InitWorld( u32 nWorldID );

    PNPCWorld* GetWorld( u32 nWorldID );
    void InitPlayer( PClient* nClient ); // Player is entering zone
};

#endif
