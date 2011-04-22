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

npc_conversation.cpp - Split up file for NPC conversations (Trade and QuestStuff)

CREATION: 11 Oct 2009 Namikon

*/

#include "main.h"
#include "include/npc.h"
#include "include/worlds.h"
#include "include/npctemplate.h"
#include "include/worlddatatemplate.h"
#include "include/msgbuilder.h"

///***********************************************************************

bool PNPC::SetShopQuality(u8 nNewVal)
{
    // Ignore that setting if NPC is loaded from .def
    if(mFromDEF == true)
        return false;

    if(nNewVal == 0)
        return false;

    mItemQuality = nNewVal;
    return true;
}

bool PNPC::ReloadShopList()
{
    // Shop list will be reloaded uppon next click
    mVectItemsInShop.erase(mVectItemsInShop.begin(), mVectItemsInShop.end());
    return true;
}

void PNPC::AddToVectorList(u16 nItemID, u32 nPrice)
{
    stShopListEntry tEntry;
    tEntry.ItemID = nItemID;
    tEntry.Price = nPrice;
    Console->Print("New item in buffer: %d price %d", tEntry.ItemID, tEntry.Price);
    mVectItemsInShop.push_back(tEntry);
}

void PNPC::ContentListAddItem(PMessage* nContentList, u16 nItemID, u32 nBasePrice, bool nAddToList)
{
    // Items with baseprice = 0 are ignored!
    if(nBasePrice == 0)
        return;

    // Modify baseprice with Config value
    f32 tPerCent = Config->GetOptionInt("item_price");
    tPerCent = tPerCent / 100;
    u32 tPrice = nBasePrice * tPerCent;
    *nContentList << ( u16 ) nItemID;
    *nContentList << ( u32 )( tPrice );

    if(nAddToList)
        AddToVectorList(nItemID, tPrice);

    if (gDevDebug) Console->Print("[PNPC::ContentListAddItem] Adding item: ID: %d Price: %u", nItemID, tPrice);
}

void PNPC::ContentListAddItemGroup(PMessage* nContentList, u32 nItemGroupID)
{
    const PDefItems* tDefItems = NULL;
    std::map<int, PDefItems*>::const_iterator itStart = GameDefs->Items()->ConstIteratorBegin();
    std::map<int, PDefItems*>::const_iterator itEnd = GameDefs->Items()->ConstIteratorEnd();
    for ( std::map<int, PDefItems*>::const_iterator i = itStart; i != itEnd; i++ )
    {
        tDefItems = i->second;
        if((u32)tDefItems->GetItemGroupID() == nItemGroupID)
        {
            // Pricing: Use Baseprice
            // If Baseprice is 0, use Quality * Techlevel * 2
            u32 tPrice = tDefItems->GetBasePrice();
            if(tPrice == 0)
            {
                tPrice = tDefItems->GetTechlevel() * mItemQuality * 2;
            }

            ContentListAddItem(nContentList, i->first, tPrice);
        }
    }
}

bool PNPC::DoSQLShoppingList( PClient* nClient, PMessage* nContentList )
{
    MYSQL_RES *result = NULL;
    MYSQL_ROW row;
    char query[100];

    snprintf( query, 100, "SELECT * FROM `npc_shop` WHERE `c_npc_id` = %d AND `c_zoneid` = %d", mWorldID, nClient->GetChar()->GetLocation());
    if (gDevDebug) Console->Print( "[PNPC::DoSQLShoppingList] Executing query %s", query );
    result = MySQL->GameResQuery( query );
    if ( result == NULL )
    {
        Console->Print( YELLOW, BLACK, "[PNPC::DoSQLShoppingList] could not load shoplist from SQL" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return false;
    }
    if ( mysql_num_rows( result ) == 0 )
    {
        if (gDevDebug) Console->Print( YELLOW, BLACK, "[PNPC::DoSQLShoppingList] No items found in SQL for NPC %d", mWorldID);
        MySQL->FreeGameSQLResult( result );
        return false;
    }

    f32 tPerCent = Config->GetOptionInt("item_price");
    tPerCent = tPerCent / 100;

    while((row = mysql_fetch_row(result)))
    {
        const PDefItems* t_item = GameDefs->Items()->GetDef(atoi( row[2] ));
        if(t_item)
        {
            // Pricing: If no value in DB is set, use Baseprice
            // If Baseprice is 0, use Quality * Techlevel * 2
            u32 tPrice = atoi(row[3]);
            if(tPrice == 0)
            {
                tPrice = t_item->GetBasePrice();
                if(tPrice == 0)
                {
                    tPrice = t_item->GetTechlevel() * mItemQuality * 2;
                }
            }

            // Modify price as told in config
            tPrice = tPrice * tPerCent;
            u16 tItemID = atoi(row[2]);

            *nContentList << ( u16 ) tItemID;
            *nContentList << ( u32 ) tPrice;

            // Store item position in list for later trade stuff
            AddToVectorList(tItemID, tPrice);

            if (gDevDebug) Console->Print("Adding item: ID: %d Price: %d",  tItemID, tPrice);
        }
    }
    MySQL->FreeGameSQLResult( result );
    return true;
}

bool PNPC::IsAllbuyer( PClient* nClient )
{
    MYSQL_RES *result = NULL;
    char query[100];

    snprintf( query, 100, "SELECT * FROM `npc_shop` WHERE `c_npc_id` = %d AND `c_zoneid` = %d and `c_itemid` = -1", mWorldID, nClient->GetChar()->GetLocation());
    Console->Print( "[PNPC::IsAllbuyer] Executing query %s", query );
    result = MySQL->GameResQuery( query );
    if ( result == NULL )
    {
        Console->Print( YELLOW, BLACK, "[PNPC::IsAllbuyer] could not check if npc is allbuyer" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return false;
    }
    int count = mysql_num_rows(result);
    MySQL->FreeGameSQLResult( result );
    if(count > 0)
        return true;
    else
        return false;
}

bool PNPC::HasSQLShoppingList( PClient* nClient )
{
    MYSQL_RES *result = NULL;
    char query[100];

    snprintf( query, 100, "SELECT * FROM `npc_shop` WHERE `c_npc_id` = %d AND `c_zoneid` = %d", mWorldID, nClient->GetChar()->GetLocation());
    Console->Print( "[PNPC::HasSQLShoppingList] Executing query %s", query );
    result = MySQL->GameResQuery( query );
    if ( result == NULL )
    {
        Console->Print( YELLOW, BLACK, "[PNPC::HasSQLShoppingList] could not load shoplist from SQL" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return false;
    }
    int count = mysql_num_rows(result);
    MySQL->FreeGameSQLResult( result );
    if(count > 0)
        return true;
    else
        return false;
}

void PNPC::StartDialog( PClient* nClient/*, string &nDialogscript*/ )
{
    std::string t_ScriptName = "";
    if(mFromDEF == false && mCustomLua.length() > 1)
    {
        char tmpnum[11];
        snprintf(tmpnum, 11, "%d", mID);
        t_ScriptName = tmpnum;
    }
    else
    {
        t_ScriptName = mDialogScript;
    }

    u32 tWorldID = GetRealWorldID();
    // Starts dialog with NPC
    // First, set required values in client's char
    nClient->GetChar()->SetDialogNPC(mWorldID);
    nClient->GetChar()->SetDialogNode(0);

    // Second generate start-dialog message
    PMessage* tmpMsg = MsgBuilder->BuildNPCStartDialogMsg(nClient, tWorldID, &t_ScriptName);
    nClient->SendUDPMessage(tmpMsg);

    Console->Print("[PNPC::StartDialog] Sending NPC DialogStart for Script %s", t_ScriptName.c_str());
    return;
}

void PNPC::StartConversation( PClient* nClient )
{
    // Set Offset to mWorldID
    // .def NPCs need this offset in order to work
    u32 tRealID = GetRealWorldID();

    // Check if NPC has script for talking
    // OR
    // Check if NPC is from SQL and has a custom LUA script stored in DB
    if(mScripting == true)
    {
        if(mDialogScript.length() > 0 || (mFromDEF == false && mCustomLua.length() > 1))
        {
            StartDialog(nClient);
            return;
        }
    }

    // Check if NPC has a TradeID
    Console->Print("[DEBUG] NPC WorldID %u  NPC TraderDefID %u", tRealID, mTrader);
    if(IsAllbuyer(nClient) == true)
    {
        PMessage* tmpMsg = MsgBuilder->BuildNPCBeginAllBuyerTradeMsg(nClient, tRealID);
        nClient->SendUDPMessage(tmpMsg);

        return;
    }

    if(mTrader > 0)
    {
        // NPC is a trader. Fetch trader template from def
        const PDefTrader* nTraderDef = GameDefs->Traders()->GetDef(mTrader);

        // Is the NPC a trading one? (Def-defined)
        if(nTraderDef)
        {
            // Preparing ItemList for shopping
            PMessage* ContentList = new PMessage();

            // Check if we already have our Vector filled with itemIDs
            if(mVectItemsInShop.size() > 0)
            {
                Console->Print("Using Vector shopping list");
                vector <stShopListEntry>::iterator it;
                for(it = mVectItemsInShop.begin(); it < mVectItemsInShop.end(); it++)
                    ContentListAddItem(ContentList, (*it).ItemID, (*it).Price, false);
            }
            else
            {
                int t_ItemGroupID = nTraderDef->GetType();
                int t_CurrItem = 0;
                int t_DefItemEnt = nTraderDef->GetItemId(t_CurrItem);
                while(t_DefItemEnt != 0)
                {
                    // Loop through all item(groups) in trader.def
                    if(t_DefItemEnt < 0)
                    {
                        // We got a ItemGroup
                        ContentListAddItemGroup(ContentList, (t_DefItemEnt * -1));
                    }
                    else
                    {
                        // We got a normal Item
                        ContentListAddItem(ContentList, t_DefItemEnt, 0, nTraderDef->GetQuality());
                    }

                    t_DefItemEnt = nTraderDef->GetItemId(++t_CurrItem);
                }
                if (gDevDebug) Console->Print("[PNPC::StartConversation] TraderTemplate: %d, Type: %d", mTrader, t_ItemGroupID);
            }


            // Step 2: Send Packet to start trade
            PMessage* tmpMsg = MsgBuilder->BuildNPCShoppingListMsg(nClient, ContentList, tRealID, nTraderDef->GetQuality());
            nClient->FragmentAndSendUDPMessage(tmpMsg, 0xac);

            delete ContentList;
        }
        else
        {
            Console->Print( YELLOW, BLACK, "[PNPC::StartConversation] unknown/invalid traderID ignoring traderequest" );
            return;
        }
    }
    // Not a regular trader? Check if we have some SQL Based shopping list
    else if(HasSQLShoppingList(nClient) == true)
    {
        PMessage* ContentList = new PMessage();
        if(DoSQLShoppingList(nClient, ContentList) == true)
        {
            PMessage* tmpMsg = MsgBuilder->BuildNPCShoppingListMsg(nClient, ContentList, tRealID, mItemQuality);
            nClient->FragmentAndSendUDPMessage(tmpMsg, 0xac);

            delete ContentList;
        }
        else
        {
            Console->Print( RED, BLACK, "[PNPC::StartConversation] Failed to generate SQL Shopping list" );
            delete ContentList;
        }
        return;
    }
    else
    {
        // No script found? No shopping list? Well, then this NPC looks.. inactive :)
        if (gDevDebug) Console->Print("[PNPC::StartConversation] Inactive NPC");
        return;
    }
}


// DoConversation:
// nClient : The client which startet the conversation / is doing it right now
// nAnswer: the Answer the player clicked
void PNPC::DoConversation( PClient* nClient, u8 nAnswer )
{
    // LUA Engine v3: Let the LuaEngine handle everything!
    LuaEngine->ProcessDialogScript(nClient, mLUAFile, nAnswer);

    return;
}
