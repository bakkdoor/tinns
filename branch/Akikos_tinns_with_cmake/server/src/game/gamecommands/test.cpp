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

void PCommands::doCmdtest()
{
    if (GetArgInt(1) == 1)^M
    {^M
        if(ArgC < 5)^M
            return;^M
^M
        PNPC* targetNPC = NULL;^M
        PNPCWorld* currentNPCWorld = NULL;^M
        currentNPCWorld = NPCManager->GetWorld( source->GetChar()->GetLocation() );^M
        if ( currentNPCWorld )^M
        {^M
            targetNPC = currentNPCWorld->GetNPC( GetArgInt(2) );^M
            if(!targetNPC)^M
                return;^M
            // @test 15 <npcID> <target_to_attack> <unknown u8 value>^M
            targetNPC->Attack(GetArgInt(3), (u8)GetArgInt(5), (u8)GetArgInt(4));^M
        }^M
    }^M
    else if (GetArgInt(1) == 2)^M
    {^M
        if(ArgC < 3)^M
            return;^M
^M
        int tF1 = GetArgInt(2);^M
        int tF2 = GetArgInt(3);^M
        const PDefFaction* tFactionA = NULL;^M
        const PDefFaction* tFactionB = NULL;^M
^M
        if(tF1 > tF2)^M
        {^M
            tFactionA = GameDefs->Factions()->GetDef(tF1);^M
            tFactionB = GameDefs->Factions()->GetDef(tF2);^M
        }^M
        else^M
        {^M
            tFactionA = GameDefs->Factions()->GetDef(tF2);^M
            tFactionB = GameDefs->Factions()->GetDef(tF1);^M
        }^M
^M
        if(tFactionA && tFactionB)^M
        {^M
            int tRel = -99;^M
            if(tF1 > tF2)^M
                tRel = tFactionA->GetRelation(tF2);^M
            else
                tRel = tFactionA->GetRelation(tF1);^M
^M
            char buff[150];^M
            snprintf(buff, 150, "Relation between <%s> and <%s> is: %d", tFactionA->GetName().c_str(), tFactionB->GetName().c_str(), tRel);^M
            Chat->send(source, CHAT_DIRECT, "Relations", buff);^M
        }^M
        else^M
            Chat->send(source, CHAT_DIRECT, "Relations", "Invalid faction");^M
    }
    else if(GetArgInt(1) == 2)
    {
    /*
        u8 val1 = 0;
        u8 val2 = 0;
        u8 val3 = 0;
        u16 val4 = 0;
        u16 val5 = 0;
    //    u16 val6 = 0;
    //    u8 val7 = 0;

        bool SyntaxError = false;
        if(ArgC < 5)
        {
            SyntaxError = true;
        }

		if(IsArgNumeric(1) == false)
            SyntaxError = true;
        if(IsArgNumeric(2) == false)
            SyntaxError = true;
        if(IsArgNumeric(3) == false)
            SyntaxError = true;
        if(IsArgNumeric(4) == false)
            SyntaxError = true;
        if(IsArgNumeric(5) == false)
            SyntaxError = true;
    //    if(IsArgNumeric(6) == false)
    //        SyntaxError = true;
    //    if(IsArgNumeric(7) == false)
    //        SyntaxError = true;

        if(SyntaxError == true)
        {
            PMessage* tmpMsg1 = new PMessage(14);

            *tmpMsg1 << (u8)0x13;
            *tmpMsg1 << (u16)0x0000; // UDP ID placeholder
            *tmpMsg1 << (u16)0x0000; // SessionID placeholder
            *tmpMsg1 << (u8)0x08;    // Len (static, always 0x08
            *tmpMsg1 << (u8)0x03;
            *tmpMsg1 << (u16)0x0000; // Sub UDP ID placeholder
            *tmpMsg1 << (u8)0x26;    // Command FADE AWAY CHAR (kinda ^^)
            *tmpMsg1 << (u8)0x00;
            *tmpMsg1 << (u8)0x10;
            *tmpMsg1 << (u8)0x00;
            *tmpMsg1 << (u8)0x80;
            ClientManager->UDPBroadcast(tmpMsg1, source);
        }

        val1 = (u8)GetArgInt(1);
        val2 = (u8)GetArgInt(2);
        val3 = (u8)GetArgInt(3);
        val4 = (u16)GetArgInt(4);
        val5 = (u16)GetArgInt(5);
    //    val7 = (u16)GetArgInt(7);

        //tmpMsg = MsgBuilder->BuildCharUseQBSlotMsg1(source, 59);
        //source->SendUDPMessage(tmpMsg);
        //tmpMsg = NULL;

        PMessage* tmpMsg = new PMessage(29);
        *tmpMsg << (u8)0x13;
       *tmpMsg << (u16)0x0000;
       *tmpMsg << (u16)0x0000;
       *tmpMsg << (u8)0x16; // Message length
       *tmpMsg << (u8)0x03;
       *tmpMsg << (u16)0x0000;
       *tmpMsg << (u8)0x1b;
       *tmpMsg << (u8)0x00;
       *tmpMsg << (u8)0x10;
       *tmpMsg << (u8)0x00;
       *tmpMsg << (u8)0x80;
       *tmpMsg << (u8)0x19;
       *tmpMsg << (u8)0x55;
       *tmpMsg << (u8)0x74;
       *tmpMsg << (u8)0x80;
       *tmpMsg << (u8)0x82;
       *tmpMsg << (u8)0xc2;
       *tmpMsg << (u8)0x84;
       *tmpMsg << (u8)val1;
       *tmpMsg << (u8)val2;
       *tmpMsg << (u8)val3;
       *tmpMsg << (u16)val4;
       *tmpMsg << (u16)val5;
    // *tmpMsg << (u8)0x69;
    // *tmpMsg << (u8)0x00;

        ClientManager->UDPBroadcast(tmpMsg, source);

    // ***************************
        bool SyntaxError = false;
        if(ArgC < 1)
        {
            SyntaxError = true;
        }

        if(IsArgNumeric(1) == false)
            SyntaxError = true;

		if(SyntaxError == true)
        {
            Chat->send(source, CHAT_DIRECT, "Usage", "@test <id>");
            return;
        }

		u16 itemID;
        char effStr[128];
        PMessage* tmpMsg;

        itemID = (u16)GetArgInt(1);

        source->GetChar()->SetItemInHand(itemID);

        tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
        ClientManager->UDPBroadcast(tmpMsg, source);
        snprintf(effStr, 127, "Item in hand changes to value %d", itemID);
        effStr[127] = '\0';
        Chat->send(source, CHAT_DIRECT, "System", effStr);
    ===========================================================================
        u16 ItemToSpawn = 0;
        u8 Quality = 0;
        u8 Stack = 0;

        bool SyntaxError = false;
		if(ArgC < 3
        {
			SyntaxError = true;
        }
        else
        {
            if(IsArgNumeric(1) == true)
            {
                ItemToSpawn = (u16)GetArgInt(1);
                if(ItemToSpawn == 0)
                {
                    SyntaxError = true;
                }
            }
            else
            {
                SyntaxError = true;
            }

            if(IsArgNumeric(2) == true)
            {
                Quality = (u8)GetArgInt(2);
                if(Quality == 0)
                {
                    SyntaxError = true;
                }
            }
            else
            {
                SyntaxError = true;
            }

            if(IsArgNumeric(3) == true)
			{
                Stack = (u8)GetArgInt(3);
                if(Stack == 0)
                {
                    SyntaxError = true;
                }
            }
            else
            {
                SyntaxError = true;
            }
        }

        if(SyntaxError == true)
        {
            Chat->send(source, CHAT_DIRECT, "Usage", "@test <itemID> <quality> <itemID>");
            return;
        }

        PMessage* tmpMsg = new PMessage(29);
        source->IncreaseUDP_ID();
        source->IncreaseTransactionID();

        *tmpMsg << (u8)0x13;
       *tmpMsg << (u16)source->GetUDP_ID();
       *tmpMsg << (u16)source->GetSessionID();
       *tmpMsg << (u8)0x16; // Message length
       *tmpMsg << (u8)0x03;
       *tmpMsg << (u16)source->GetUDP_ID();
       *tmpMsg << (u8)0x1f;
       *tmpMsg << (u16)source->GetLocalID();
       *tmpMsg << (u8)0x25; // ??
       *tmpMsg << (u8)0x13; // ??
        *tmpMsg << (u16)source->GetTransactionID();
       *tmpMsg << (u8)0x18; // ??
       *tmpMsg << (u8)0x03; // ??
       *tmpMsg << (u8)0x01; // ??
       *tmpMsg << (u8)0x00; // ??
       *tmpMsg << (u8)0x05; // ??
       *tmpMsg << (u8)0x00; // ??
       *tmpMsg << ItemToSpawn;
       *tmpMsg << (u8)0x02; // ??
       *tmpMsg << (u8)0x01; // ??
       *tmpMsg << Quality;
       *tmpMsg << Stack;

    source->SendUDPMessage(tmpMsg);
    */
}
