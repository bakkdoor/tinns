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

void PCommands::doCmd_dev_t()
{
  const char* usage = "Usage: @t r <rawid> (to remove object) | @t d <rawid> [<v1=74> [<v2=29>]] (to send death packet to object with args v1 v2)";
  char tmpStr[128];
  const char* textMsg = usage;
  PMessage* tmpMsg = NULL;
  char Arg1[30];

  if ( IsAdmin() == false )
    return;

  Arg1[0] = tmpStr[0] = '\0';

  if ( ArgC >= 2 )
  {
    GetArgText( 1, Arg1, 30 );
    u32 targetObjectId = GetArgInt( 2 ) & 0xffffffff;
    if ( Arg1[0] == 't' )
    {
      u8 val1;
      if ( ArgC >= 3 )
      {
        val1 = GetArgInt( 3 ) & 0xff;
        tmpMsg = new PMessage( 15 );
        source->IncreaseUDP_ID();
        *tmpMsg << ( u8 )0x13;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u16 )source->GetSessionID();
        *tmpMsg << ( u8 )0x00; // Message length
        *tmpMsg << ( u8 )0x03;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u8 )0x2d;
        *tmpMsg << ( u32 )targetObjectId;
        *tmpMsg << ( u8 )val1; 
  
        ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
        source->SendUDPMessage( tmpMsg );
        snprintf( tmpStr, 127, "Sent 13/03/2d msg to object id 0x%08x with value %d", targetObjectId, val1 );
        textMsg = tmpStr;
      }
      else for(val1 = 2; val1 < 255; ++val1)
      {
        if(val1 == 6) continue;
        tmpMsg = new PMessage( 15 );
        source->IncreaseUDP_ID();
        *tmpMsg << ( u8 )0x13;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u16 )source->GetSessionID();
        *tmpMsg << ( u8 )0x00; // Message length
        *tmpMsg << ( u8 )0x03;
        *tmpMsg << ( u16 )source->GetUDP_ID();
        *tmpMsg << ( u8 )0x2d;
        *tmpMsg << ( u32 )targetObjectId;
        *tmpMsg << ( u8 )val1; 
  
        ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
        source->SendUDPMessage( tmpMsg );
        snprintf( tmpStr, 127, "Sent 13/03/2d msg to object id 0x%08x with value %d", targetObjectId, val1 );
        textMsg = tmpStr;
        tmpStr[127] = '\0';
        Chat->send( source, CHAT_DIRECT, "System", textMsg );
        source->getUDPConn()->update();
        source->getTCPConn()->update();
        sleep(1);
      }
      tmpMsg = NULL;
    }
    else if ( Arg1[0] == 'd' )
    {
      u8 val1 = 0x4a; // default values
      u8 val2 = 0x1e;

      if ( ArgC >= 3 )
        val1 = GetArgInt( 3 ) & 0xff;
      if ( ArgC >= 4 )
        val2 = GetArgInt( 4 ) & 0xff;
      tmpMsg = MsgBuilder->BuildNpcDeathMsg( source, targetObjectId, val1, val2 );
      snprintf( tmpStr, 127, "Sending Death update to object id 0x%08x with values 0x%02x 0x%02x", targetObjectId, val1, val2 );
      textMsg = tmpStr;
    }
    else if ( Arg1[0] == 'm' )
    {
      u8 nTxtGroupID = targetObjectId & 0xff;
      u16 nTxtID = 10;
      //u32 nVal = 0;
      if ( ArgC >= 3 )
        nTxtID = GetArgInt( 3 ) & 0xffff;
      //if(ArgC >= 4)
      //  val2 = GetArgInt(4) & 0xff;

      tmpMsg = new PMessage( 20 );

      source->IncreaseUDP_ID();
      *tmpMsg << ( u8 )0x13;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u16 )source->GetSessionID();
      *tmpMsg << ( u8 )0x0e; // Message length
      *tmpMsg << ( u8 )0x03;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u8 )0x1f;
      *tmpMsg << ( u16 )source->GetLocalID();
      *tmpMsg << ( u8 )0x25; // ??
      *tmpMsg << ( u8 )0x15; // ??
      *tmpMsg << nTxtGroupID;
      *tmpMsg << nTxtID;
      *tmpMsg << ( u16 )0x00; // ??
      *tmpMsg << ( u8 )0x01;
      *tmpMsg << ( u8 )0x04;
      *tmpMsg << ( u32 )0x00;
      //*tmpMsg << (u8)0x00; // ??
      //*tmpMsg << (u8)0x00; // ??
      //*tmpMsg << (u32)nVal;

      ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
      snprintf( tmpStr, 127, "Using msg n %d from group %d", nTxtID, nTxtGroupID );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );
      tmpMsg = NULL;
    }
    else if ( Arg1[0] == 'w' )
    {
      targetObjectId &= 0xffff;
      u16 val1 = 0;
      if ( ArgC >= 3 )
        val1 = GetArgInt( 3 ) & 0xffff;

      tmpMsg = new PMessage( 15 );

      source->IncreaseUDP_ID();

      *tmpMsg << ( u8 )0x13;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u16 )source->GetSessionID();
      *tmpMsg << ( u8 )0x0a; // Message length place;
      *tmpMsg << ( u8 )0x03;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u8 )0x23;
      *tmpMsg << ( u8 )0x000f; // cmd = ?
      *tmpMsg << ( u16 )targetObjectId; //0x0003
      *tmpMsg << ( u16 )val1; //0x0001


      ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

      snprintf( tmpStr, 127, "Sending W msg with param %d / %d", targetObjectId, val1 );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );
      tmpMsg = NULL;
    }
    else if ( Arg1[0] == 'k' )
    {
      tmpMsg = new PMessage( 15 );

      source->IncreaseUDP_ID();

      *tmpMsg << ( u8 )0x13;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u16 )source->GetSessionID();
      *tmpMsg << ( u8 )0x00; // Message length place;
      *tmpMsg << ( u8 )0x03;
      *tmpMsg << ( u16 )source->GetUDP_ID();
      *tmpMsg << ( u8 )0x1f;
      *tmpMsg << ( u16 )source->GetLocalID();
      *tmpMsg << ( u8 )0x16;
      *tmpMsg << ( u32 )0x00000000;

      ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

      snprintf( tmpStr, 127, "Sending kill(?) msg" );
      textMsg = tmpStr;

      source->SendUDPMessage( tmpMsg );

      tmpMsg = NULL;
    }
  }

  tmpStr[127] = '\0';
  Chat->send( source, CHAT_DIRECT, "System", textMsg );

  if ( tmpMsg )
    ClientManager->UDPBroadcast( tmpMsg, source );
}

/*** Packet fields testing. Please do not delete (Hammag)
void PCommands::doCmd_dev_t()
{
    if(IsAdmin() == false)
        return;

    char Arg1[30], Arg2[30];

    Arg1[0] = '\0';
    Arg2[0] = '\0';

    if(ArgC > 0)
    {
        GetArgText(1, Arg1, 30);
        if(ArgC > 1)
        {
            GetArgText(2, Arg2, 30);
        }
    }

    u32 val1;
    u8 val2;
    char tmpStr[128];
    static PMessage* tmpMsg = NULL;
    bool SetUDP_IDNeeded = true;
    PChar* nChar = source->GetChar();
    (nChar->Coords).mY += 20;
    (nChar->Coords).mZ += 20;
    (nChar->Coords).mX += 20;

    if (!tmpMsg)
    {
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      SetUDP_IDNeeded = false;
    }

    if(Arg1[0] != '\0' && Arg2[0] != '\0')
    {
        val1 = atoi(Arg1);
        val2 = (u8)(atoi(Arg2) & 0xff);
        //tmpMsg->U8Data(16 + val1) = val2;
        tmpMsg->U8Data(10 + val1) = val2;
        snprintf(tmpStr, 127, "Data #%d set to value 0x%02x", val1, val2);
    }
    else
    {
      if (tmpMsg)
        delete tmpMsg;
      //tmpMsg = MsgBuilder->BuildCharHelloMsg(source);
      tmpMsg = MsgBuilder->BuildPacket0Msg(source);
      SetUDP_IDNeeded = false;
      snprintf(tmpStr, 127, "Data reset to normal values");
    }

    tmpStr[127] = '\0';
    Chat->send(source, CHAT_DIRECT, "System", tmpStr);

    PMessage* SendMsg = new PMessage(*tmpMsg);
    if(SetUDP_IDNeeded) {
      source->FillInUDP_ID(SendMsg);
    }
    SendMsg->Dump();
    //ClientManager->UDPBroadcast(SendMsg, source);
    source->SendUDPMessage(SendMsg);
}
***/

/*** Subwy testing. Please do not delete (Hammag)
void PCommands::doCmd_dev_t()
{
  if(IsAdmin() == false)
      return;

  char tmpStr[128];
 u8 SubWay[] = {0x13, 0x71, 0x00, 0x9b, 0xde,
  //Subway 1 (fc 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfc, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfc, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 2 (fb 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfb, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x41, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfb, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 3 (fa 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xfa, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x70, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xfa, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 4 (f9 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf9, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xb4, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf9, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 5 (f8 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf8, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0x42, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf8, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 6 (f7 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf7, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x16, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf7, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00};

 u8 SubWay2[] = {0x13, 0x71, 0x00, 0x9b, 0xde,
  //Subway 7 (f6 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf6, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x34, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf6, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 8 (f5 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf5, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x52, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf5, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 9 (f4 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf4, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x70, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf4, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 10 (f3 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf3, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x87, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf3, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00,
  //Subway 11 (f2 03)
  0x11,
  0x03, 0x6d, 0x00, 0x28, 0x27, 0x00, 0xf2, 0x03, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x96, 0x43, 0x00, 0x01,
  0x0d,
  0x03, 0x71, 0x00, 0x2d, 0xf2, 0x03, 0x00, 0x00, 0x0a, 0x00,
  0x00, 0x00, 0x00};

 if (source->GetChar()->GetLocation() != 1000)
  return;

 //SubWay List
 //Subway 1
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[7] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[25] = source->GetUDP_ID();

 //Subway 2
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[39] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[57] = source->GetUDP_ID();

 //Subway 3
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[71] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[89] = source->GetUDP_ID();

 //Subway 4
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[103] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[121] = source->GetUDP_ID();

 //Subway 5
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[135] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[153] = source->GetUDP_ID();

 //Subway 6
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[167] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay[185] = source->GetUDP_ID();

 //Subway 7
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[7] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[25] = source->GetUDP_ID();

 //Subway 8
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[39] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[57] = source->GetUDP_ID();

 //Subway 9
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[71] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[89] = source->GetUDP_ID();

 //Subway 10
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[103] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[121] = source->GetUDP_ID();

 //Subway 11
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[135] = source->GetUDP_ID();
 source->IncreaseUDP_ID();
 *(u16*)&SubWay2[153] = source->GetUDP_ID();

 *(u16*)&SubWay[1] = source->GetUDP_ID();
 *(u16*)&SubWay[3] = source->GetSessionID();

 PMessage* msg;
 msg = new PMessage(197);
 msg->Write(SubWay, sizeof(SubWay));
Console->Print("---- Working 1 ----");
msg->Dump();
  source->SendUDPMessage(msg);
msg = MsgBuilder->BuildSubwaySpawnMsg(source, false);
Console->Print("---- Not Working 1 ----");
msg->Dump();
source->SendUDPMessage(msg);

 *(u16*)&SubWay2[1] = source->GetUDP_ID();
 *(u16*)&SubWay2[3] = source->GetSessionID();

 msg = new PMessage(197);
 msg->Write(SubWay2, sizeof(SubWay2));
Console->Print("---- Working 2 ----");
msg->Dump();
  source->SendUDPMessage(msg);
msg = MsgBuilder->BuildSubwaySpawnMsg(source, true);
Console->Print("---- Not Working 2 ----");
msg->Dump();
source->SendUDPMessage(msg);

  snprintf(tmpStr, 127, "Initial subway data sent");
  tmpStr[127] = '\0';
  Chat->send(source, CHAT_DIRECT, "System", tmpStr);
}
***/
