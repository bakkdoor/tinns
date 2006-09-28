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

	udp_useobject.cpp - decoder classes for object use related messages
  
	CREATION: 17 Sep 2006 Hammag

	MODIFIED:
	REASON: - 

*/

#include "main.h"
#include "udp_useobject.h"

/**** PUdpVhcMove ****/

PUdpUseObject::PUdpUseObject(PMsgDecodeData* nDecodeData) : PUdpMsgAnalyser(nDecodeData)
{
  nDecodeData->mName << "/0x17";
} 

PUdpMsgAnalyser* PUdpUseObject::Analyse()
{
  mDecodeData->mName << "=Object use";
  
  mItemID = mDecodeData->mMessage->U32Data(mDecodeData->Sub0x13Start+8);
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  
  return this;
}

bool PUdpUseObject::DoAction()
{
    /*PMessage* cMsg = mDecodeData->mMessage;
    u32 ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);
    
    PMessage* tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
    mDecodeData->mClient->getUDPConn()->SendMessage(tmpMsg);*/

PChar* Char = mDecodeData->mClient->GetChar();
Console->Print("Char at y=%f (0x%04x) z=%f (0x%04x) x=%f (0x%04x)", (f32)(Char->Coords.mY - 32000), Char->Coords.mY, (f32)(Char->Coords.mZ - 32000), Char->Coords.mZ, (f32)(Char->Coords.mX - 32000), Char->Coords.mX);
if (mItemID & 1023)
Console->Print("using item %d (0x%08x)", mItemID, mItemID);    
else
Console->Print("using item %d (0x%08x) [%d (0x%08x)]", mItemID, mItemID, mItemID/1024 -1, mItemID/1024 -1);

    OldHandler(); // Temp
    
    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}

void PUdpUseObject::OldHandler()
{
  PClient* Client = mDecodeData->mClient;
  PChar* Char = Client->GetChar();
  const u8* Packet = (const u8*)(mDecodeData->mMessage->GetMessageData()) + mDecodeData->Sub0x13Start;
  
/** --------------------- Packet defs --------------------- **/
char DoubleDoorPacket[] = {
    0x13, 0x05, 0x00, 0x22, 0x94,
    0x0f,
    0x03, 0x27, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x15,
    0x0f,
    0x03, 0x28, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x05,
    0x00, 0x00, 0x00, 0x00, 0x15}; // 37

char SingleDoorPacket[] = {
    0x13, 0x05, 0x00, 0x22, 0x94,
    0x0f,
    0x03, 0x27, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0xc8, 0x00, 0xff, 0x10,
    0x0f,
    0x03, 0x28, 0x00, 0x1b, 0x82, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0xc8, 0x00, 0xff, 0x10};  // 37

char GenRepUse[] = {
    0x13, 0x1a, 0x00, 0x61, 0xb1,
    0x06,
    0x2d, 0x00, 0xc4, 0x00, 0x00, 0x0a,
    //    |Item ID |
    0x0d,
    0x03, 0x2d, 0x00, 0x1f, 0x01, 0x00, 0x2d, 0x02, 0x00, 0x00,
    0x00, 0x64, 0x00};//                      |    Location |
//    |location| //26

char WorldItemUse[] = {
    0x13, 0x1a, 0x00, 0x61, 0xb1,
    0x06,
    0x2d, 0x00, 0xc4, 0x00, 0x00, 0x0a,
    //    |Item ID |
    0x0b,
    0x03, 0x2d, 0x00, 0x1f, 0x01, 0x00, 0x17, 0x00, 0xc4, 0x00,
    0x00}; //                                       |Item ID | // 24

char GogoUse[] = {
    0x13, 0x40, 0x00, 0x22, 0xed,
    0x0b,
    0x03, 0x40, 0x00, 0x1f, 0x01, 0x00, 0x3d, 0x0d, 0x00, 0x00,
    0x00}; // 17

//This is for dungeons as it zones you right away.
char DungeonUse[] = {
    0x13,
    0x49, 0x00,                             // UDP ID
    0x11, 0xd2,                             // Session ID
    0x06, 0x2d, 0x00,                       // Unknown
    0x00, 0x00,                             // Worlditem ID
    0x00, 0x0a, 0x0f, 0x03,                 // Unknown
    0x48, 0x00,                             // UDP ID
    0x1f, 0x01, 0x00, 0x38, 0x04,           // Unknown
    0x00,                                   // 0x00 for City<>Lv1, 0x01 for Lv1<>Lv2 (?)
    0x06, 0x04, 0x00, 0x00,                 // Destination WorldID
    0x09, 0x00};                            // Destination world-entry
//0x07 0x04 0x00 0x00 = Location

char AptItemUse[] = {
    0x13, 0xc1, 0x00, 0x61, 0xf8,
    0x06,
    0x2d, 0x00, 0x38, 0x00, 0x00, 0x0a,
    0x11,
    0x03, 0xc1, 0x00, 0x1f, 0x01, 0x00, 0x38, 0x01, 0x00, 0x38,
    0x00, 0x00, 0xf3, 0x00, 0x00, 0x00, 0x00};
//          |Apt Place|

char ChairUse[] = {
    0x13, 0x47, 0x00, 0x1a, 0xd6,
    0x0c,
    0x03, 0x46, 0x00, 0x1f, 0x01, 0x00, 0x21, 0x00, 0x68, 0x01,
    0x00, 0x00, //         |Item ID |
    0x0c,
    0x03, 0x47, 0x00, 0x1f, 0x01, 0x00, 0x21, 0x00, 0x68, 0x01,
    0x00, 0x00};//         |Item ID |

char VehicleUse[] = {
    0x13, 0x52, 0x00, 0x94, 0xae,
    0x13,
    0x03, 0x51, 0x00, 0x1f, 0x01, 0x00, 0x3d, 0x0e, 0x00, 0x00,
    0x00, 0xfc, 0x03, 0x00, 0x00, 0x05, 0x00, 0x0f, 0x00};

char DoorBuzz[] = {
    0x13, 0x21, 0x00, 0xfc, 0xb1, 0x09, 0x03,
    0x21, 0x00, 0x2d, 0x00, 0x08, 0x00, 0x00, 0x05};

char AptDoorUse[] = {
    0x13, 0x00, 0x00, 0x00, 0x00,
    0x06,
    0x2d, 0x00, 0x00, 0x00, 0x00, 0x0a,
    0x0f,
    0x03, 0x28, 0x00, 0x1b, 0x81, 0x00, 0x00, 0x00, 0x20, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00,
    0x10,
    0x03, 0x3c, 0x00, 0x1f, 0x01, 0x00, 0x25, 0x13, 0x28, 0xb5,
    0x13, 0x00, 0x00, 0x60, 0x71, 0xc5};

char DoorLocked[] = {
    0x13, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x03, 0x007, 0x00, 0x1f, 0x01, 0x00, 0x31, 0x01, 0x81, 0x00, 0x00, 0x00};
/** ------------------------------------------------------- **/

  if(Packet[8] == 0x00)  // 0x00 means Isnt a door
  {
    PAccount *Account = Client->GetAccount();
    if(Account->IsAdminDebug() == true)
    {
        char debugmsg[100];
        sprintf(debugmsg, "ID: %d LOC: %d", *(unsigned short*)&Packet[9], Char->GetLocation());
        Chat->send(Client, CHAT_DIRECT, "System", debugmsg);
    }
//Console->Print("Usage of non-door worlditem ID %d, SubClassID %d", Packet[9], Packet[10]);
    char ErrorMsg[256];
    int itemID = *(unsigned short*)&Packet[9]; // NeoX says : u32 @offset 8 !!!
    //int subClass = *(unsigned short*)&Packet[10];
    int Location = Char->GetLocation();
    //int worlditemtype = MySQL->GetWorldItemType(*(unsigned short*)&Packet[9], *(unsigned short*)&Packet[10], Char->GetLocation());
    int worlditemtype = MySQL->GetWorldItemType(itemID, Location);
    if(worlditemtype == -2)
    {
        sprintf(ErrorMsg, "Duplicate entry for WorlditemID %d, Location %d!", itemID, Location);
        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
        return;
        //break;
    }
    if(worlditemtype == -1)
    {
        sprintf(ErrorMsg, "WorlditemID %d, Location %d is unknown", itemID, Location);
        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
        return;
        //break;
    }
    if (worlditemtype == 1) // GenRep
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&GenRepUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&GenRepUse[3] = Client->GetSessionID();
        *(unsigned short*)&GenRepUse[14] = Client->GetUDP_ID();

        //*(unsigned short*)&GenRepUse[8] = *(unsigned short*)&Packet[9];
        
        int Option1 = MySQL->GetWorldItemOption(mItemID/256, Location, 1); // from NeoX

				*(u32*)&GenRepUse[7] = (u32)mItemID; // good itemID
				*(u32*)&GenRepUse[20] = (u32)Location; // from NeoX
				*(u16*)&GenRepUse[24] = (u16)Option1; // from NeoX
				
        *(u16*)&GenRepUse[17] = (u16)Client->GetLocalID(); // from NeoX
        
        Client->getUDPConn()->write(GenRepUse, sizeof(GenRepUse));
        Console->Print("Genrep");
        //                        Network_SendUDP (GenRepUse, sizeof(GenRepUse), ClientNum);
    }
    else if (worlditemtype == 2) // Appartment Station // Lift Access IF
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&AptItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&AptItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&AptItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&AptItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&AptItemUse[21] = *(unsigned short*)&Packet[9];

        *(u16*)&AptItemUse[17] = (u16)Client->GetLocalID(); // from NeoX
        
        Client->getUDPConn()->write(AptItemUse, sizeof(AptItemUse));
        Console->Print("Appartmentstation");
        //                        Network_SendUDP (AptItemUse, sizeof(AptItemUse), ClientNum);
    }
    else if (worlditemtype == 3) // GoGuardian
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&GogoUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&GogoUse[3] = Client->GetSessionID();
        *(unsigned short*)&GogoUse[7] = Client->GetUDP_ID();

        *(u16*)&GogoUse[10] = (u16)Client->GetLocalID(); // from NeoX
        
        Client->getUDPConn()->write(GogoUse, sizeof(GogoUse));
        Console->Print("Gogo");
        //                        Network_SendUDP (GogoUse, sizeof(GogoUse), ClientNum);
    }
    else if (worlditemtype == 4) // Zoning Doors
    {
        int option1 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 1);
        int option2 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 2);
        int option3 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 3);
        Console->Print("o1: %d  o2: %d  o3: %d  src: %d", option1, option2, option3, Char->GetLocation());

        if(option1 == -1 || option2 == -1)
        {
            return;
            //break;
        }

        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&DungeonUse[14] = Client->GetUDP_ID();
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&DungeonUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&DungeonUse[3] = Client->GetSessionID();

        *(unsigned short*)&DungeonUse[8] = *(unsigned short*)&Packet[9];

        *(u8*)&DungeonUse[21] = option3;
        *(unsigned int*)&DungeonUse[22] = option1;
        *(unsigned short*)&DungeonUse[26] = option2;
        //Console->Print("Warping player from %d to %d entry %d", Char->GetLocation(), option1, option2);

        *(u16*)&DungeonUse[17] = (u16)Client->GetLocalID(); // from NeoX

        Client->getUDPConn()->write(DungeonUse, sizeof(DungeonUse));
    }
    else if (worlditemtype == 5) // Chairs
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&ChairUse[7] = Client->GetUDP_ID();
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&ChairUse[20] = Client->GetUDP_ID();
        *(unsigned short*)&ChairUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&ChairUse[3] = Client->GetSessionID();

        *(unsigned short*)&ChairUse[14] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&ChairUse[27] = *(unsigned short*)&Packet[9];

        *(u16*)&ChairUse[10] = (u16)Client->GetLocalID(); // From NeoX
        *(u16*)&ChairUse[23] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(ChairUse, sizeof(ChairUse));
        Console->Print("Chair");
        //                        Network_SendUDP (WorldItemUse, sizeof(WorldItemUse), ClientNum);
    }
    else if (worlditemtype == 6) // Condition Locks
    {
        int option1 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 1);
        int option2 = MySQL->GetWorldItemOption(itemID, Char->GetLocation(), 2);
        Console->Print("Condition lock");
        switch (option1)
        {
            default:
            Client->SetUDP_ID(Client->GetUDP_ID()+1);
            //Client->IncreaseUDP_ID();
            *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
            *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
            *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

            *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
            *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

            *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX
            
            Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
            Console->Print("default lock");
            break;

            case 2: // Appartment and Locked
            if(MySQL->GetAptOwner(Char->GetLocation()) == (int)Char->GetID())
            {
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&AptDoorUse[14] = Client->GetUDP_ID();
                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&AptDoorUse[30] = Client->GetUDP_ID();
                *(unsigned short*)&AptDoorUse[1] = Client->GetUDP_ID();
                *(unsigned short*)&AptDoorUse[3] = Client->GetSessionID();

                *(unsigned short*)&AptDoorUse[8] = *(unsigned short*)&Packet[9];
                *(unsigned short*)&AptDoorUse[17] = *(unsigned short*)&option2;
                Console->Print("Case2 lock");

                switch(option2)
                {
                    case 3: // Single Locked
                    AptDoorUse[22] = 0x00;
                    AptDoorUse[24] = 0x64;
                    AptDoorUse[26] = 0xFF;
                    AptDoorUse[27] = 0x10;
                    break;
                    case 4: // Double Locked
                    AptDoorUse[22] = 0x05;
                    AptDoorUse[24] = 0x00;
                    AptDoorUse[26] = 0x00;
                    AptDoorUse[27] = 0x15;
                    break;
                    default:
                    AptDoorUse[22] = 0x00;
                    AptDoorUse[24] = 0x64;
                    AptDoorUse[26] = 0xFF;
                    AptDoorUse[27] = 0x10;
                    break;
                }
                Client->getUDPConn()->write(AptDoorUse, sizeof(AptDoorUse));
            }
            else // Not owner? Buzz!
            {
                Console->Print("Buzzing door");

                //Client->IncreaseUDP_ID();
                Client->SetUDP_ID(Client->GetUDP_ID()+1);
                *(unsigned short*)&DoorBuzz[1] = Client->GetUDP_ID();
                *(unsigned short*)&DoorBuzz[3] = Client->GetSessionID();
                *(unsigned short*)&DoorBuzz[7] = Client->GetUDP_ID();

                *(unsigned short*)&DoorBuzz[11] = *(unsigned short*)&Packet[9];

                //Network_SendUDP (SendBuffer, 15, ClientNum);
                Client->getUDPConn()->write(DoorBuzz, sizeof(DoorBuzz));
            }
            break;
        }
    }
    // *************************************************************************************************************
    else if (worlditemtype == 7) // Venture Warp
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Venture");
    }
    else if (worlditemtype == 8) // Vehicle Depot
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("VehicDepot");
    }
    else if (worlditemtype == 9) // Holo Exit
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX
        
        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Holoexit");
    }
    else if (worlditemtype == 10) // Holo Weapon
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Holo weapon");
    }
    else if (worlditemtype == 11) // Holo Heal
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Holoheal");
    }
    else if (worlditemtype == 12) // CityCom
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Citycom");
    }
    else if (worlditemtype == 13) // OutFitters
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Outfitter");
    }
    else if (worlditemtype == 14) // Cabs
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Cab");
    }
    else if (worlditemtype == 15) // Containers
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Container");
    }
    else if (worlditemtype == 16) // Street Signs
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID();

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse)); // but no real action on client side, except sound
        
        Console->Print("Street sign");
    }
    else if (worlditemtype == 17) // Street Signs
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Street sign 2");
    }
    else if (worlditemtype == 18) // Recreation Units
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Recreation unit");
    }
    else if (worlditemtype == 19) // Outpost hack
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&WorldItemUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&WorldItemUse[3] = Client->GetSessionID();
        *(unsigned short*)&WorldItemUse[14] = Client->GetUDP_ID();

        *(unsigned short*)&WorldItemUse[8] = *(unsigned short*)&Packet[9];
        *(unsigned short*)&WorldItemUse[21] = *(unsigned short*)&Packet[9];
        
        *(u16*)&WorldItemUse[17] = (u16)Client->GetLocalID(); // From NeoX

        Client->getUDPConn()->write(WorldItemUse, sizeof(WorldItemUse));
        Console->Print("Outpost hack");
    }
    //*********************************************************************************************************
    else
    {
        Chat->send(Client, CHAT_DIRECT, "System", "This kind of worlditem isnt coded yet");
    }
    //PrintPacket((u8*)Packet, PacketSize);
    // --------------------------------------------------------------------------------------------------------
  }
  else
  {
//Console->Print("Usage of door worlditem");
    PAccount *Account = Client->GetAccount();
    if(Account->IsAdminDebug() == true)
    {
        char debugmsg[100];
        sprintf(debugmsg, "ID: %d LOC: %d", *(unsigned short*)&Packet[8], Char->GetLocation());
        Chat->send(Client, CHAT_DIRECT, "System", debugmsg);
    }
    char ErrorMsg[1024];
    int i = MySQL->GetWorldDoorType(*(unsigned int*)&Packet[8], Char->GetLocation());
    if(i == -2)
    {
        if(Char->GetLocation() > 100000)
            sprintf(ErrorMsg, "Duplicate entry for doorID %d, appartment %d", Packet[8], Char->GetLocation() - 100000);
        else
            sprintf(ErrorMsg, "Duplicate entry for doorID %d, location %d", Packet[8], Char->GetLocation());

        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
    }
    else if(i == -1)
    {
        if(Char->GetLocation() > 100000)
            sprintf(ErrorMsg, "Doortype not found: DoorID %d, appartment %d", Packet[8], Char->GetLocation() - 100000);
        else
            sprintf(ErrorMsg, "Doortype not found: DoorID %d, location %d", Packet[8], Char->GetLocation());

        Chat->send(Client, CHAT_DIRECT, "System", ErrorMsg);
    }
    else if(i == 1)  // Single door
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&SingleDoorPacket[7] = Client->GetUDP_ID()-1;
        *(unsigned short*)&SingleDoorPacket[23] = Client->GetUDP_ID();
        *(unsigned short*)&SingleDoorPacket[1] = Client->GetUDP_ID();
        *(unsigned short*)&SingleDoorPacket[3] = Client->GetSessionID();

        *(unsigned int *)&SingleDoorPacket[10] = *(unsigned int *)&Packet[8];
        *(unsigned int *)&SingleDoorPacket[26] = *(unsigned int *)&Packet[8];

        //Console->Print("Opening Singledoor id %d", Packet[8]);
        Client->getUDPConn()->write(SingleDoorPacket, sizeof(SingleDoorPacket));
    }
    else if(i == 2) // Double door
    {
        //Client->IncreaseUDP_ID();
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+2);
        *(unsigned short*)&DoubleDoorPacket[7] = Client->GetUDP_ID()-1;
        *(unsigned short*)&DoubleDoorPacket[23] = Client->GetUDP_ID();
        *(unsigned short*)&DoubleDoorPacket[1] = Client->GetUDP_ID();
        *(unsigned short*)&DoubleDoorPacket[3] = Client->GetSessionID();

        *(unsigned int *)&DoubleDoorPacket[10] = *(unsigned int *)&Packet[8];
        *(unsigned int *)&DoubleDoorPacket[26] = *(unsigned int *)&Packet[8];

        //Console->Print("Opening Doubledoor id %d", Packet[8]);
        Client->getUDPConn()->write(DoubleDoorPacket, sizeof(DoubleDoorPacket));
    }
    else if (i == 3) //Locked Door (Opens by other means, usually access panel)
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&DoorLocked[1] = Client->GetUDP_ID();
        *(unsigned short*)&DoorLocked[3] = Client->GetSessionID();
        *(unsigned short*)&DoorLocked[7] = Client->GetUDP_ID();

        *(unsigned int *)&DoorLocked[14] = *(unsigned int *)&Packet[8];

        Client->getUDPConn()->write(DoorLocked, sizeof(DoorLocked));
        Console->Print("locked door");
    }
    else if (*(unsigned int *)&Packet[8] > 1000)
    {
        //Client->IncreaseUDP_ID();
        Client->SetUDP_ID(Client->GetUDP_ID()+1);
        *(unsigned short*)&VehicleUse[1] = Client->GetUDP_ID();
        *(unsigned short*)&VehicleUse[3] = Client->GetSessionID();
        *(unsigned short*)&VehicleUse[7] = Client->GetUDP_ID();

        *(unsigned int *)&VehicleUse[17] = *(unsigned int *)&Packet[8];

        PSpawnedVehicles Vehic;
        Vehicles->GetSpawnedVehicle (&Vehic, *(unsigned int *)&Packet[8]);
        VehicleUse[21] = Vehic.Type;

        Client->getUDPConn()->write(VehicleUse, sizeof(VehicleUse));
        Console->Print("Vehicleuse");
    }
    else // Wrong answer should never happen...
    {
        Console->Print("%s Error in door function, illegal return value from GetWolrdDoorType: %d", Console->ColorText(RED, BLACK, "[Error]"), i);
    }
  }  
}
