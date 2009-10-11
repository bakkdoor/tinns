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
#include "npc.h"
#include "worlds.h"
#include "npctemplate.h"
#include "worlddatatemplate.h"

///***********************************************************************

void PNPC::ContentListAddItem(PMessage* nContentList, u16 nItemID, u32 nQuality, u32 nBasePrice, f32 nPriceCoef)
{
    // TODO: What if BasePrice is 0? For now its techlevel * quality
    // TODO: nPriceCoef depends on faction standing!
    f32 tPriceCoef;
    if(nPriceCoef == 0.0f)
        tPriceCoef = 1 / 1.379942;
    else
        tPriceCoef = nPriceCoef;

    u32 tBasePrice = 0;
    if(nBasePrice == 0)
    {
        const PDefItems* t_item = GameDefs->Items()->GetDef(nItemID);
        tBasePrice = t_item->GetTechlevel() * nQuality ;
    }

    *nContentList << ( u16 ) nItemID;
    *nContentList << ( u32 )( tBasePrice * tPriceCoef );
    if (gDevDebug) Console->Print("[PNPC::ContentListAddItem] Adding item: ID: %d Price: %u", nItemID, tBasePrice);
}

void PNPC::ContentListAddItemGroup(PMessage* nContentList, u32 nItemGroupID, u32 nQuality)
{
    int maxitems = 16000;
    for( int i = 0; i < maxitems; i++ )
    {
        const PDefItems* t_item = GameDefs->Items()->GetDef(i);
        if(t_item)
        {
            if((u32)t_item->GetItemGroupID() == nItemGroupID)
            {
                int baseprice = t_item->GetBasePrice();
                if(baseprice == 0)
                {
                    // Set price (if base = 0) to techlevel * quality
                    baseprice = t_item->GetTechlevel() * nQuality;
                }
                ContentListAddItem(nContentList, i, baseprice);
                //Console->Print("[PNPC::ContentListAddItemGroup] Item: %d Price: %d (Group %d)", i, baseprice, nItemGroupID);
            }
        }
    }
    // This iterator loop stucks at its 3rd loop with 100% cpu load and i dont know why. thats
    // the reason why i used this simple 0 to 16000 loop. Would be nice if hammag could check this
    /*
    for ( std::map<int, PDefItems*>::const_iterator i = GameDefs->Items()->GetMap().begin(); i != GameDefs->Items()->GetMap().end(); i++ )
    {
        if(i->second)
        {
            if(i->second->GetItemGroupID() == nItemGroupID)
            {
                int itemid = i->first;
                int baseprice = i->second->GetBasePrice();
                Console->Print("[PNPC::ContentListAddItemGroup] Item: %d Price: %d (Group %d)", itemid, baseprice, nItemGroupID);
                //ContentListAddItem(nContentList, itemid, baseprice, coef);
            }
        }
        //do something..
    }
    */
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

    f32 PriceCoef = 1 / 1.379942;
    while((row = mysql_fetch_row(result)))
    {
        const PDefItems* t_item = GameDefs->Items()->GetDef(atoi( row[2] ));
        if(t_item)
        {
            *nContentList << ( u16 ) atoi(row[2]);
            *nContentList << ( u32 )( atoi(row[3]) * PriceCoef );
            if (gDevDebug) Console->Print("Adding item: ID: %d Price: %d", atoi(row[2]), t_item->GetBasePrice());
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

void PNPC::StartConversation( PClient* nClient )
{
    // Check if NPC has a TradeID
    Console->Print("[DEBUG] NPC WorldID %u  NPC TraderDefID %u", mWorldID, mTrader);
    if(IsAllbuyer(nClient) == true)
    {
        PMessage* tmpMsg = new PMessage();
        nClient->IncreaseUDP_ID();

        *tmpMsg << ( u8 )0x13;
        *tmpMsg << ( u16 )nClient->GetUDP_ID();
        *tmpMsg << ( u16 )nClient->GetSessionID();
        *tmpMsg << ( u8 )0x00; // Message length
        *tmpMsg << ( u8 )0x03;
        *tmpMsg << ( u16 )nClient->GetUDP_ID();
        *tmpMsg << ( u8 )0x1f;
        *tmpMsg << ( u16 )nClient->GetLocalID();
        *tmpMsg << ( u8 )0x26;
        if(mFromDEF == true)
            *tmpMsg << mWorldID + 255; // Dont forget the offset!!!
        else
            *tmpMsg << mWorldID; // Dont forget the offset!!!
        *tmpMsg << ( u8 )0x01; // Traders inventory
        *tmpMsg << ( u16 )0xFFFF; // Traders inventory

        ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
        nClient->FragmentAndSendUDPMessage(tmpMsg, 0xac);

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

            int t_ItemGroupID = nTraderDef->GetType();
            int t_CurrItem = 0;
            int t_DefItemEnt = nTraderDef->GetItemId(t_CurrItem);
            while(t_DefItemEnt != 0)
            {
                // Loop through all item(groups) in trader.def
                if(t_DefItemEnt < 0)
                {
                    // We got a ItemGroup
                    ContentListAddItemGroup(ContentList, (t_DefItemEnt * -1), nTraderDef->GetQuality());
                }
                else
                {
                    // We got a normal Item
                    ContentListAddItem(ContentList, t_DefItemEnt, 0, nTraderDef->GetItemPriceScale(t_CurrItem), nTraderDef->GetQuality());
                }

                t_DefItemEnt = nTraderDef->GetItemId(++t_CurrItem);
            }
            if (gDevDebug) Console->Print("[PNPC::StartConversation] TraderTemplate: %d, Type: %d", mTrader, t_ItemGroupID);

            // Step 2: Send Packet to start trade
            PMessage* tmpMsg = new PMessage();
            nClient->IncreaseUDP_ID();

            *tmpMsg << ( u8 )0x13;
            *tmpMsg << ( u16 )nClient->GetUDP_ID();
            *tmpMsg << ( u16 )nClient->GetSessionID();
            *tmpMsg << ( u8 )0x00; // Message length
            *tmpMsg << ( u8 )0x03;
            *tmpMsg << ( u16 )nClient->GetUDP_ID();
            *tmpMsg << ( u8 )0x1f;
            *tmpMsg << ( u16 )nClient->GetLocalID();
            *tmpMsg << ( u8 )0x26;
            if(mFromDEF == true)
                *tmpMsg << mWorldID + 255; // Dont forget the offset!!!
            else
                *tmpMsg << mWorldID; // Dont forget the offset!!!
            *tmpMsg << ( u8 )0x01; // Traders inventory
            *tmpMsg << ( u16 )( ContentList->GetSize() / 6 ); // List entries
            *tmpMsg << ( u8 )nTraderDef->GetQuality(); // Items quality
            *tmpMsg << *ContentList;

            ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

            delete ContentList;

            nClient->FragmentAndSendUDPMessage(tmpMsg, 0xac);
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
            PMessage* tmpMsg = new PMessage();
            nClient->IncreaseUDP_ID();

            *tmpMsg << ( u8 )0x13;
            *tmpMsg << ( u16 )nClient->GetUDP_ID();
            *tmpMsg << ( u16 )nClient->GetSessionID();
            *tmpMsg << ( u8 )0x00; // Message length
            *tmpMsg << ( u8 )0x03;
            *tmpMsg << ( u16 )nClient->GetUDP_ID();
            *tmpMsg << ( u8 )0x1f;
            *tmpMsg << ( u16 )nClient->GetLocalID();
            *tmpMsg << ( u8 )0x26;
            if(mFromDEF == true)
                *tmpMsg << mWorldID + 255; // Dont forget the offset!!!
            else
                *tmpMsg << mWorldID; // Dont forget the offset!!!
            *tmpMsg << ( u8 )0x01; // Traders inventory
            *tmpMsg << ( u16 )( ContentList->GetSize() / 6 ); // List entries
            *tmpMsg << ( u8 )mItemQuality; // Items quality
            *tmpMsg << *ContentList;

            ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

            delete ContentList;

            nClient->FragmentAndSendUDPMessage(tmpMsg, 0xac);
        }
        else
        {
            Console->Print( RED, BLACK, "[PNPC::StartConversation] Failed to generate SQL Shopping list" );
            delete ContentList;
        }
        return;
    }
    // No sql shopping list and no def-based one? Then
    // TODO: Add npc script management
    else
    {
        if (gDevDebug) Console->Print("[PNPC::StartConversation] No npc script handling yet");
        return;
    }
}
