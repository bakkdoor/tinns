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
    if (GetArgInt(1) == 1)
    {
        PMessage* tmpMsg = new PMessage(54);
        source->IncreaseUDP_ID();
//13 87 00 7A DF
//23 03 7E 00 28 00 01 2D 01 00 00 17 EF 6F 06 C8 11 72 78 00 80 3B 7D 00 57 00 6E 00 57 53 4B 00 31 38 30 00
//23 03 25 00 28 00 01 15 01 00 00 36 3f 14 53 01 00 58 83 80 7f 37 82 00 56 7e 00 00 57 43 4f 50 00 39 30 00
        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x00; // Message length
        *tmpMsg << (u8)0x03;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u8)0x28;
        *tmpMsg << (u16)0x0100;
        *tmpMsg << (u32)0x12d; // NpcId
        *tmpMsg << (u8)0x36;
        *tmpMsg << (u8)0x3F;
        *tmpMsg << (u8)0x14;
        *tmpMsg << (u8)0x53;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;

        *tmpMsg << (u16)(source->GetChar()->Coords.mY + 768);
        *tmpMsg << (u16)(source->GetChar()->Coords.mZ + 768);
        *tmpMsg << (u16)(source->GetChar()->Coords.mX + 768);

        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x56;
        *tmpMsg << (u8)0x7E;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x57;
        *tmpMsg << (u8)0x43;
        *tmpMsg << (u8)0x4F;
        *tmpMsg << (u8)0x50;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x39;
        *tmpMsg << (u8)0x30;
        *tmpMsg << (u8)0x00;
        *tmpMsg << 'T';
        *tmpMsg << 'i';
        *tmpMsg << 'n';
        *tmpMsg << 'N';
        *tmpMsg << 'S';
        *tmpMsg << ' ';
        *tmpMsg << 'r';
        *tmpMsg << '0';
        *tmpMsg << 'c';
        *tmpMsg << 'k';
        *tmpMsg << 'z';
        *tmpMsg << '!';
        *tmpMsg << (u8)0x00;

        (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
        source->SendUDPMessage(tmpMsg);
    }
    else if (GetArgInt(1) == 2)
    {

//13 31 00 a5 ff 15 1b e7 03 00 00 1f 2c 84 80 7f 6b 84 03 77 02  01 00 00 00 00 14
//NC UDP.| Sess  Ln Cs |..NPCID..| 1F |.Y.| |.Z.| |.X.| BM ?  Hlt TrgCh ?  ?  ?  Ac

        /*
        BM:
        00000001 (  1): Stand normal (?)
        00000010 (  2): Stand normal (?)
        00000100 (  4): Stand normal (?)
        00001000 (  8): Stand normal (?)
        00010000 ( 16): Duck
        00100000 ( 32): Stand normal (?)
        01000000 ( 64): Stand normal (?)
        10000000 (128): Die
        */
        PMessage* tmpMsg = new PMessage(27);
        //source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x15; // Message length
        *tmpMsg << (u8)0x1b;
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x1F;
        *tmpMsg << (u16)(source->GetChar()->Coords.mY + 768);
        *tmpMsg << (u16)(source->GetChar()->Coords.mZ + 768);
        *tmpMsg << (u16)(source->GetChar()->Coords.mX + 768);
        *tmpMsg << (u8)GetArgInt(2);
        *tmpMsg << (u8)GetArgInt(3);
        *tmpMsg << (u8)0x7F;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x18;
        source->SendUDPMessage(tmpMsg);
    }
    else if (GetArgInt(1) == 3)
    {
        PMessage* tmpMsg = new PMessage(15);
        source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x09;
        *tmpMsg << (u8)0x03;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)GetArgInt(2);
        source->SendUDPMessage(tmpMsg);
//09 03 5d 00 2d 24 01 00 00 06
    }
    else if (GetArgInt(1) == 4)
    {
        PMessage* tmpMsg = new PMessage(14);
        source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x08;
        *tmpMsg << (u8)0x03;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        source->SendUDPMessage(tmpMsg);
//08 03 17 00 27 24 01 00 00
    }
    else if (GetArgInt(1) == 5)
    {
        PMessage* tmpMsg = new PMessage(23);
        //source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x11;
        *tmpMsg << (u8)0x1B;
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x1F;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mY + 768);*/
        *tmpMsg << (u16)31062;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mZ + 768);*/
        *tmpMsg << (u16)32512;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mX + 768);*/
        *tmpMsg << (u16)33973;
        *tmpMsg << (u8)0x40;
        *tmpMsg << (u8)0x53;
        *tmpMsg << (u8)0x48;
        *tmpMsg << (u8)GetArgInt(2);
        *tmpMsg << (u8)GetArgInt(3);
        //*tmpMsg << (u8)GetArgInt(4);
//        *tmpMsg << (u8)GetArgInt(5);
//        *tmpMsg << (u8)GetArgInt(6);
        source->SendUDPMessage(tmpMsg);
//11 1b 24 01 00 00 1f 6c 74 00 7f cc 8a 40 ed 4b ff 11
    }
    else if (GetArgInt(1) == 6)
    {
        PMessage* tmpMsg = new PMessage(23);
        //source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x15;
        *tmpMsg << (u8)0x1B;
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x1F;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mY + 768);*/
        *tmpMsg << (u16)31062;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mZ + 768);*/
        *tmpMsg << (u16)32512;
        /**tmpMsg << (u16)(source->GetChar()->Coords.mX + 768);*/
        *tmpMsg << (u16)33973;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x53;
        *tmpMsg << (u8)0x48;
        *tmpMsg << (u16)GetArgInt(2);
        *tmpMsg << (u16)GetArgInt(3);
        *tmpMsg << (u16)GetArgInt(4);
//        *tmpMsg << (u8)GetArgInt(5);
//        *tmpMsg << (u8)GetArgInt(6);
        source->SendUDPMessage(tmpMsg);
//11 1b 24 01 00 00 1f 6c 74 00 7f cc 8a 40 ed 4b ff 11
    }
    else if (GetArgInt(1) == 7)
    {
        PMessage* tmpMsg = new PMessage(23);
        source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x0F;
        *tmpMsg << (u8)0x03;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u8)0x1F;
        *tmpMsg << (u16)source->GetLocalID();
        *tmpMsg << (u8)0x25;
        *tmpMsg << (u8)0x0B;
        *tmpMsg << (u16)GetArgInt(2);
        *tmpMsg << (u8)GetArgInt(3);
        *tmpMsg << (u32)50;
        source->SendUDPMessage(tmpMsg);
    }
    else if (GetArgInt(1) == 10)
    {
        PMessage* tmpMsg = new PMessage(23);
        //source->IncreaseUDP_ID();

        *tmpMsg << (u8)0x13;
        *tmpMsg << (u16)source->GetUDP_ID();
        *tmpMsg << (u16)source->GetSessionID();
        *tmpMsg << (u8)0x11;
        *tmpMsg << (u8)0x1B;
        *tmpMsg << (u8)0x2D;
        *tmpMsg << (u8)0x01;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x1F;

        *tmpMsg << (u16)GetArgInt(2);
        *tmpMsg << (u16)GetArgInt(3);
        *tmpMsg << (u16)GetArgInt(4);

        *tmpMsg << (u8)0x40;
        *tmpMsg << (u8)0x53;
        *tmpMsg << (u8)0x48;
        *tmpMsg << (u8)0x00;
        *tmpMsg << (u8)0x00;
        //*tmpMsg << (u8)GetArgInt(4);
//        *tmpMsg << (u8)GetArgInt(5);
//        *tmpMsg << (u8)GetArgInt(6);
        source->SendUDPMessage(tmpMsg);
    }
    else if (GetArgInt(1) == 11)
    {
        PMessage* tmpMsg = new PMessage();
        source->IncreaseUDP_ID();

        *tmpMsg << ( u8 )0x13;
        *tmpMsg << ( u32 ) 0x0000; // UDP Placeholder
        //*tmpMsg << ( u16 )nClient->GetUDP_ID();
        //*tmpMsg << ( u16 )nClient->GetSessionID();
        *tmpMsg << ( u8 )0x00; // Message length
        *tmpMsg << ( u8 )0x03;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u8 )0x1f;
        *tmpMsg << ( u16 )source->GetLocalID();
        *tmpMsg << ( u8 )0x09;
        *tmpMsg << ( u8 )0x01;
        *tmpMsg << ( u8 )0x00;
        *tmpMsg << ( u8 )0x00;
        // Todo: is this correct? random u32 value??
        //*tmpMsg << ( u16 ) GetRandom( 32000, 1 );
        //*tmpMsg << ( u16 ) GetRandom( 32000, 1 );
        *tmpMsg << ( u8 ) GetArgInt(2);
        *tmpMsg << ( u8 ) GetArgInt(3);
        *tmpMsg << ( u8 ) GetArgInt(4);
        *tmpMsg << ( u8 ) GetArgInt(5);
        *tmpMsg << ( u32 ) 0x0000;
        *tmpMsg << ( u8 )0x18;
        *tmpMsg << ( u8 )0x44;
        *tmpMsg << ( u8 )0x55;
        *tmpMsg << ( u8 )0x4d;
        *tmpMsg << ( u8 )0x4d;
        *tmpMsg << ( u8 )0x59;
        *tmpMsg << ( u8 )0x5f;
        *tmpMsg << ( u8 )0x53;
        *tmpMsg << ( u8 )0x45;
        *tmpMsg << ( u8 )0x43;
        *tmpMsg << ( u8 )0x00;

        ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

        source->IncreaseUDP_ID();

        *tmpMsg << ( u8 )0x0a;
        *tmpMsg << ( u8 )0x03;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u8 )0x1f;
        *tmpMsg << ( u16 )source->GetLocalID();
        *tmpMsg << ( u8 )0x1a;
        *tmpMsg << ( u8 )0x00;
        *tmpMsg << ( u8 )0x00;
        *tmpMsg << ( u8 )0x00;

        ( *tmpMsg )[1] = ( u16 )source->GetUDP_ID();
        ( *tmpMsg )[3] = ( u16 )source->GetSessionID();

        source->SendUDPMessage(tmpMsg);
    }
    else if (GetArgInt(1) == 12)
    {
        PItem* NewItem = new PItem( GetArgInt(2) );
        if ( NewItem->GetItemID() )
        {
            NewItem->MakeItemStandard( GetArgInt(3), GetArgInt(3) ); // global quality range
            source->GetChar()->GetInventory()->AddItem( NewItem );

            PMessage* tmpMsg = new PMessage();
            source->IncreaseUDP_ID();
            *tmpMsg << ( u8 )0x13;
            *tmpMsg << ( u32 ) 0x0000; // UDP Placeholder
            //*tmpMsg << ( u16 )nClient->GetUDP_ID();
            //*tmpMsg << ( u16 )nClient->GetSessionID();
            *tmpMsg << ( u8 )0x00; // Message length
            *tmpMsg << ( u8 )0x03;
            *tmpMsg << ( u16 )source->GetUDP_ID();
            *tmpMsg << ( u8 )0x1f;
            *tmpMsg << ( u16 )source->GetLocalID();
            *tmpMsg << ( u8 )0x25;
            *tmpMsg << ( u8 )0x13;

            source->IncreaseTransactionID();
            *tmpMsg << ( u16 ) source->GetTransactionID();

            *tmpMsg << ( u8 )0x18;
            *tmpMsg << ( u8 )0x03;
            *tmpMsg << ( u8 )0xFF;
            *tmpMsg << ( u8 )0xFF;
            *tmpMsg << ( u8 )0x04;
            *tmpMsg << ( u8 )0x00;
            *tmpMsg << ( u16 ) GetArgInt(2);
            *tmpMsg << ( u8 ) 0x01;
            *tmpMsg << ( u8 ) 0x01;
            *tmpMsg << ( u8 ) 0x0a;
            *tmpMsg << ( u8 ) 0x00;

            ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
            tmpMsg->U16Data( 1 ) = source->GetUDP_ID();
            tmpMsg->U16Data( 3 ) = source->GetSessionID();

            source->SendUDPMessage(tmpMsg);
        }
        else
        {
            Chat->send(source, CHAT_DIRECT, "Error", "Invalid item ID %d", GetArgInt(2));
            return;
        }

    }








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
    //	*tmpMsg << (u8)0x69;
    //	*tmpMsg << (u8)0x00;

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
        if(ArgC < 3)
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
