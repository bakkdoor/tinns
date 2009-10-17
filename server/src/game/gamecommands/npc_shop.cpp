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

void PCommands::doNPC_Shop()
{
    bool SyntaxError = false;
    if(ArgC < 2)
    {
        SyntaxError = true;
    }

    if(IsArgNumeric(1) == true)
    {
        target = GetClientByID(GetArgInt(1));
    }
    else
    {
        SyntaxError = true;
    }

    if(SyntaxError == true)
    {
        Chat->send(source, CHAT_DIRECT, "Usage", "@npcshop <worldID> <what>");
        Chat->send(source, CHAT_DIRECT, "Usage", " <what> can be:");
        Chat->send(source, CHAT_DIRECT, "setquality <1-255>", "Sets itemquality in shop");
        Chat->send(source, CHAT_DIRECT, "additem <id> <price>", "Adds item <ID> with <Price>");
        Chat->send(source, CHAT_DIRECT, "delitem <id>", "Removes item <ID>");
        Chat->send(source, CHAT_DIRECT, "setdefshop <id>", "ID from trader.def; Use 0 to unset");
        Chat->send(source, CHAT_DIRECT, "reload", "Reloads shop list");
        Chat->send(source, CHAT_DIRECT, "remove", "Disables the shop");
        Chat->send(source, CHAT_DIRECT, "setallbuyer", "Makes NPC an allbuyer (Yo's)");

        /* more to add here... spawn, remove, ... */
        return;
    }

    PNPC* targetNPC = NULL;
    PNPCWorld* currentNPCWorld = NPCManager->GetWorld( source->GetChar()->GetLocation() );
    if ( currentNPCWorld )
    {
        targetNPC = currentNPCWorld->GetNPC( GetArgInt(1) );
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

    // Get subcommand
    char tmp_npccommand[50];
    GetArgText(2, tmp_npccommand, 50);

    if(strncmp(tmp_npccommand, "setallbuyer", 11) == 0)
    {
        int tItemID = -1;
        int tPrice = 0;
        int tNPCID = targetNPC->GetNPCID();
        int tZoneID = source->GetChar()->GetLocation();

        char sqlq[200];
        snprintf(sqlq, 200, "INSERT INTO npc_shop (`c_npc_id`, `c_zoneid`, `c_itemid`, `c_itemprice`) VALUES (%d, %d, %d, %d)", tNPCID, tZoneID, tItemID, tPrice);
        if(MySQL->GameQuery(sqlq))
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB insert, nothing changed");
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "NPC is now an allbuyer");
        }
        return;

    }
    else if(strncmp(tmp_npccommand, "additem", 7) == 0)
    {
        int tItemID = GetArgInt(3);
        int tPrice = GetArgInt(4);
        int tNPCID = targetNPC->GetNPCID();
        int tZoneID = source->GetChar()->GetLocation();
        const PDefItems* t_item = GameDefs->Items()->GetDef(tItemID);
        if(!t_item)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Invalid ItemID");
            return;
        }
        char sqlq[200];
        snprintf(sqlq, 200, "INSERT INTO npc_shop (`c_npc_id`, `c_zoneid`, `c_itemid`, `c_itemprice`) VALUES (%d, %d, %d, %d)", tNPCID, tZoneID, tItemID, tPrice);
        if(MySQL->GameQuery(sqlq))
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB insert, item not added");
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Item added to shop");
        }
        return;
    }
    else if(strncmp(tmp_npccommand, "delitem", 7) == 0)
    {
        int tItemID = GetArgInt(3);
        int tNPCID = targetNPC->GetNPCID();
        int tZoneID = source->GetChar()->GetLocation();
        char sqlq[200];
        snprintf(sqlq, 200, "DELETE FROM npc_shop WHERE `c_npc_id` = %d and `c_zoneid` = %d and `c_itemid` = %d LIMIT 1", tNPCID, tZoneID, tItemID);
        if(MySQL->GameQuery(sqlq))
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error on DB update, item not removed");
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Item removed from shop");
        }
        return;
    }
    else if(strncmp(tmp_npccommand, "setdefshop", 10) == 0)
    {
        int tNewTraderDef = GetArgInt(3);
        const PDefTrader* nTraderDef = GameDefs->Traders()->GetDef(tNewTraderDef);

        // Valid trader definition?
        if(!nTraderDef)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Invalid Traderdef ID");
            return;
        }

        char sqlq[200];
        char sqlq2[200];
        bool success = false;
        snprintf(sqlq, 200, "UPDATE npc_spawns SET `npc_trader` = %d WHERE `npc_id` = %d", tNewTraderDef, targetNPC->GetNPCSQLID());
        if(!MySQL->GameQuery(sqlq))
        {
            snprintf(sqlq2, 200, "UPDATE npc_spawns SET `npc_trader` = 0 WHERE `npc_id` = %d", targetNPC->GetNPCSQLID());
            if(!MySQL->GameQuery(sqlq2))
                success = true;
        }

        if(success == true)
        {
            // Set new trader and remove existing shoplist
            targetNPC->SetTrader(tNewTraderDef);
            targetNPC->ReloadShopList();
            Chat->send(source, CHAT_DIRECT, "System", "NPC acts now as defined TraderShop");
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error while removing shop");
        }


        return;
    }
    else if(strncmp(tmp_npccommand, "remove", 6) == 0)
    {
        int tZoneID = source->GetChar()->GetLocation();
        int tNPCID = targetNPC->GetNPCID();
        char sqlq[200];
        bool success = false;
        snprintf(sqlq, 200, "DELETE FROM npc_shop WHERE `c_npc_id` = %d and `c_zoneid` = %d", tNPCID, tZoneID);
        if(MySQL->GameQuery(sqlq))
        {
            snprintf(sqlq, 200, "UPDATE npc_spawns SET `npc_trader` = 0 WHERE `npc_id` = %d", targetNPC->GetNPCSQLID());
            if(MySQL->GameQuery(sqlq))
                success = false;
            else
                success = true;
        }
        if(success == true)
        {
            // Remove shoplist
            targetNPC->ReloadShopList();
            Chat->send(source, CHAT_DIRECT, "System", "Shop removed from NPC");
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Error while removing shop");
        }


        return;
    }
    else if(strncmp(tmp_npccommand, "reload", 6) == 0)
    {
        if(targetNPC->ReloadShopList() == true)
        {
            Chat->send(source, CHAT_DIRECT, "System", "Shoplist reload successfull");
            return;
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Shoplist reload failed");
            return;
        }
    }
    else if(strncmp(tmp_npccommand, "setquality", 10) == 0)
    {
        u8 tNewLv = (u8)GetArgInt(3);
        if(targetNPC->IsSQLNPC() == true)
        {
            char sqlq[200];
            snprintf(sqlq, 200, "UPDATE npc_spawns SET `npc_shop_quality` = %d WHERE `npc_id` = %d", tNewLv, targetNPC->GetNPCSQLID());
            if(MySQL->GameQuery(sqlq))
            {
                Chat->send(source, CHAT_DIRECT, "System", "Error while updating SQL. New quality will be lost on reload");
            }
        }

        if(targetNPC->SetShopQuality(tNewLv) == true)
        {
            char tBuff[100];
            snprintf(tBuff, 100, "Itemquality of this NPC is now %u", tNewLv);
            Chat->send(source, CHAT_DIRECT, "System", tBuff);
            return;
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "System", "Cannot set ItemQuality. (Make sure you entered 1-255 and NPC is loaded from SQL)");
            return;
        }
    }
    else
    {
        char buff[100];
        snprintf(buff, 100, "Unknown command: '%s'", tmp_npccommand);
        Chat->send(source, CHAT_DIRECT, "System", buff);
        return;
    }
}
