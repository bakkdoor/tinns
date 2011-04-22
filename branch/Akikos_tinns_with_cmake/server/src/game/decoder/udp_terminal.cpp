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
	udp_terminal.cpp - decoder classes for UDP terminal related messages

	CREATION: 08 Jan 2007 Namikon

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/


#include "main.h"

#include "udp_terminal.h"
#include "udp_charmove.h"

#include "include/terminal.h"
#include "include/vehicle.h"
#include "include/worlds.h"
#include "include/furnituretemplate.h"

#include <cmath>

/*******************************************************************************************/
/**** PUdpReceiveDB ****/
/*******************************************************************************************/
PUdpReceiveDB::PUdpReceiveDB( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x17";
  mOptionsCount = 0;
}

PUdpMsgAnalyser* PUdpReceiveDB::Analyse()
{
  PMessage* TmpMsg = mDecodeData->mMessage;
  u16 Unknown3, OptionSize;

  mDecodeData->mName << "=ReceiveDB request from client";

  mTerminalSessionId = TmpMsg->U8Data( mDecodeData->Sub0x13Start + 6 );
  TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 17 );
  ( *TmpMsg ) >> mUnknown2; // Some unknown var maxx used. No idea what that is
  ( *TmpMsg ) >> mUnknown1; // command name size
  ( *TmpMsg ) >> mDBId; // variable, increments => DB ID. Look in several terminal .tsc files, ENV(DBID) it is!
  ( *TmpMsg ) >> Unknown3; // constant ? => Size of Options!
  ( *TmpMsg ) >> mCommandName; // null terminated string


  while (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && ( mOptionsCount < mMaxOptions ) )
  {
    ( *TmpMsg ) >> OptionSize;
    if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
    {
      ( *TmpMsg ) >> mOptions[mOptionsCount++];
      //if(mOptions[mOptionsCount-1].size() != (OptionSize-1)) Warning (but no pb)!
    }
    else
    {
      break;
    }
  }

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpReceiveDB::DoAction()
{
  //PMessage* tmpMsg;
  //PClient* tClient = mDecodeData->mClient;
  //PChar* tChar = tClient->GetChar();
  bool Result = false;

  if ( gDevDebug )
  {
    Console->Print( "%s ReceiveDB request from client", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
	Console->Print( "%s Open Terminal - Terminal session %04x (?) - Unknown1 %04x - DBId %04x - Unknown2 %02x", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTerminalSessionId, mUnknown1, mDBId, mUnknown2 );
    Console->Print( "%s Command: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), i, mOptions[i].c_str() );
  }

  if ( mCommandName == "VehicleListing" )
  {
    Result = ActionVehicleListing();
  }
  else if ( mCommandName == "VehicleControl" )
  {
    Result = ActionVehicleControl();
  }
  else
  {
      // Let Terminal try before error
      Result = Terminal->HandleReceiveDB(mDecodeData->mClient, mTerminalSessionId, &mCommandName, mOptions, mOptionsCount, mDBId, mUnknown2);
  }

  if ( !Result )
  {
    Console->Print( "%s PUdpReceiveDB - Error or unknown command %s", Console->ColorText( RED, BLACK, "[WARNING]" ), mCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( RED, BLACK, "[NOTICE]" ), i, mOptions[i].c_str() );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return Result;
}

bool PUdpReceiveDB::ActionVehicleListing()
{
  PMessage* tmpMsg;
  PClient* tClient = mDecodeData->mClient;
  PChar* tChar = tClient->GetChar();

  if ( mOptionsCount == 3 ) // CharId, StartVhcEntry, MaxVhcEntries
  {
    if ((( u32 )atol( mOptions[0].c_str() ) ) != tChar->GetID() )
    {
      // Err: invalid CharId. Can alert, But we don't care :-) (except if used for other terminal function)
    }
    // !!! some more check/regex on values to do before using !!!
    u16 StartIndex = atoi( mOptions[1].c_str() );
    u16 MaxEntries = atoi( mOptions[2].c_str() );

    u8 nStatus = 1;
    u16 nErrcode = 0;

    tmpMsg = MsgBuilder->BuildDBRequestStatusMsg( tClient, &mCommandName, nStatus, nErrcode );
    tClient->SendUDPMessage( tmpMsg );

    PVhcInfoList* VhcList = Vehicles->GetCharVehicles( tChar->GetID(), MaxEntries, StartIndex );
    if ( ! VhcList->empty() )
    {
      u16 NumEntries = VhcList->size();
      std::string* Answer = new std::string[4 * NumEntries];
      u16 Index = 0;
      PVehicleInformation* EntryInfo;
//Console->Print("VHc entries : %d", NumEntries);

      while ( ! VhcList->empty() )
      {
        EntryInfo = VhcList->front();
        VhcList->pop();
        Answer[Index++] = Ssprintf( "%u", EntryInfo->GetVehicleId() ); //vhcId
        Answer[Index++] = Ssprintf( "%u", EntryInfo->GetVehicleType() ); //vhcType
        Answer[Index++] = Ssprintf( "%u", EntryInfo->GetStatus() ); //vhcStatus 0:parking, 1:in_service, 2:destroyed
        Answer[Index++] = Ssprintf( "%u", EntryInfo->GetHealth() ); //vhcHealth%
//Console->Print("Entry: %s/%s/%s/%s", Answer[Index-4].c_str(), Answer[Index-3].c_str(), Answer[Index-2].c_str(), Answer[Index-1].c_str());
        delete EntryInfo;
      }

      tmpMsg = MsgBuilder->BuildDBAnswerMsg( tClient, &mCommandName, Answer, NumEntries, 4 );
      tClient->SendUDPMessage( tmpMsg );
      delete [] Answer;
    }
    delete VhcList;

    return true;
  }
  else
    return false;
  /*
  // Option1=CharId, Option2=resultEntryStart, Option3=maxResultEntries
  S=> 03/2b/1a <u16 size incl 0><u8 bool succes ?><u16 err code ?>VehicleListing+0
  13 2a 00 7c be 19
  03 2a 00 2b 1a 0f 00 01 00 00 56 65 68 69 63 6c  .*.+......Vehicl
  65 4c 69 73 74 69 6e 67 00  eListing.
  S=> 03/2b/17 0f 00 08 00 04 00 <0f 00><u16 entries nb><04 00>
  VehicleListing+0
  <id_size incl 0><id_string +0>
  <type_size><type_id_string? +0>
  <status_size><status_string +0> (val: 0=stored)
  <health_size><health_string +0> (val: 0-255)
  ==
  03/2b/17 0f 00 01 00 04 00 <0f 00><u16 entries nb><04 00>
  31
  03 54 00 2b 17 0f 00 01 00 04 00 56 65 68 69 63 6c 65 4c 69 73 74  .....VehicleList
  69 6e 67 00 06 00 32 35 32 37 37 00 03 00 36 30  ing...25277...60
  00 02 00 30 00 04 00 32 35 35 00  ...0...255.
  */
}

bool PUdpReceiveDB::ActionVehicleControl()
{
  PMessage* tmpMsg;
  PClient* tClient = mDecodeData->mClient;

  if ( mOptionsCount == 2 ) // VhcId, CharId
  {
    // !!! validate values !!!
    u8 nStatus = 1;
    u16 nErrcode = 0;

    tmpMsg = MsgBuilder->BuildDBRequestStatusMsg( tClient, &mCommandName, nStatus, nErrcode );
    tClient->SendUDPMessage( tmpMsg );

    u32 VhcId = ( u32 )atol( mOptions[0].c_str() );
    PVehicleInformation EntryInfo;
    if ( Vehicles->GetVehicleInfo( VhcId, &EntryInfo ) )
    {
      std::string* Answer = new std::string[4];
      Answer[0] = Ssprintf( "%u", EntryInfo.GetVehicleType() ); //vhcType
      Answer[1] = Ssprintf( "%u", EntryInfo.GetStatus() ); //vhcStatus
      Answer[2] = Ssprintf( "%u", EntryInfo.GetHealth() ); //vhcHealth%
      Answer[3] = Ssprintf( "%u", ( 255 - EntryInfo.GetHealth() ) * 1000 * EntryInfo.GetVehicleType() / 255 ); //Repair cost
      if ( gDevDebug )
        Console->Print( "%s Entry: %s/%s/%s/%s", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), Answer[0].c_str(), Answer[1].c_str(), Answer[2].c_str(), Answer[3].c_str() );
      tmpMsg = MsgBuilder->BuildDBAnswerMsg( tClient, &mCommandName, Answer, 1, 4 );
      tClient->SendUDPMessage( tmpMsg );
      delete [] Answer;
    }
    return true;
  }
  else
    return false;
  /*
  // Option1=VhcId, Option2=CharId
  S=> 03/2b/1a VehicleControl
  13 77 00 c9 be 19
  03 76 00 2b 1a 0f 00 01 00 00 56 65 68 69 63 6c  .v.+......Vehicl
  65 43 6f 6e 74 72 6f 6c 00  eControl.
  S=> 03/2b/17 0f 00 01 00 04 00 VehicleControl  4 0 255 4255(
  2f
  03 77 00 2b 17 0f 00 01 00 04 00 56 65 68 69 63 6c 65 43 6f 6e 74  .....VehicleCont
  72 6f 6c 00 02 00 34 00 02 00 30 00 04 00 32 35  rol...4...0...25
  35 00 05 00 34 32 35 35 00                       5...4255.
  */
}

/*******************************************************************************************/
/**** PUdpUpdateDB ****/
/*******************************************************************************************/
PUdpUpdateDB::PUdpUpdateDB( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x18";
  mOptionsCount = 0;
}

PUdpMsgAnalyser* PUdpUpdateDB::Analyse()
{
  PMessage* TmpMsg = mDecodeData->mMessage;
  u16 Unknown3, OptionSize;

  mDecodeData->mName << "=UpdateDB request from client";

  mTerminalSessionId = TmpMsg->U8Data( mDecodeData->Sub0x13Start + 6 );
  TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 17 );
  ( *TmpMsg ) >> mUnknown2; // Some unknown var maxx used. No idea what that is
  ( *TmpMsg ) >> mUnknown1; // command name size
  ( *TmpMsg ) >> mDBId; // variable, increments => DB ID. Look in several terminal .tsc files, ENV(DBID) it is!
  ( *TmpMsg ) >> Unknown3; // constant ? => Size of Options!
  ( *TmpMsg ) >> mCommandName; // null terminated string


  while (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && ( mOptionsCount < mMaxOptions ) )
  {
    ( *TmpMsg ) >> OptionSize;
    if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
    {
      ( *TmpMsg ) >> mOptions[mOptionsCount++];
      //if(mOptions[mOptionsCount-1].size() != (OptionSize-1)) Warning (but no pb)!
    }
    else
    {
      break;
    }
  }

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpUpdateDB::DoAction()
{
  if ( gDevDebug )
  {
    Console->Print( "%s UpdateDB request from client", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    Console->Print( "%s Open Terminal - Terminal session %04x (?)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTerminalSessionId );
    Console->Print( "%s Command: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), i, mOptions[i].c_str() );
  }
   bool Result = false;
    Result = Terminal->HandleUpdateDB(mDecodeData->mClient, mTerminalSessionId, &mCommandName, mOptions, mOptionsCount, mDBId, mUnknown2);

 if ( !Result )
  {
    Console->Print( "%s PUdpUpdateDB - Error or unknown command %s", Console->ColorText( RED, BLACK, "[WARNING]" ), mCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( RED, BLACK, "[NOTICE]" ), i, mOptions[i].c_str() );
  }
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
/*******************************************************************************************/
/**** PUdpTryAccessDB ****/
/*******************************************************************************************/
PUdpTryAccessDB::PUdpTryAccessDB( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x19";
  mOptionsCount = 0;
}

PUdpMsgAnalyser* PUdpTryAccessDB::Analyse()
{
  PMessage* TmpMsg = mDecodeData->mMessage;
  u16 Unknown3, OptionSize;

  mDecodeData->mName << "=TryAccess request from client";

  mTerminalSessionId = TmpMsg->U8Data( mDecodeData->Sub0x13Start + 6 );
  TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 17 );
  ( *TmpMsg ) >> mUnknown2; // Some unknown var maxx used. No idea what that is
  ( *TmpMsg ) >> mUnknown1; // command name size
  ( *TmpMsg ) >> mDBId; // variable, increments => DB ID. Look in several terminal .tsc files, ENV(DBID) it is!
  ( *TmpMsg ) >> Unknown3; // constant ? => Size of Options!
  ( *TmpMsg ) >> mCommandName; // null terminated string
}

  while (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && ( mOptionsCount < mMaxOptions ) )
  {
    ( *TmpMsg ) >> OptionSize;
    if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
    {
      ( *TmpMsg ) >> mOptions[mOptionsCount++];
      //if(mOptions[mOptionsCount-1].size() != (OptionSize-1)) Warning (but no pb)!
    }
    else
    {
      break;
    }
  }

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;


bool PUdpTryAccessDB::DoAction()
{
    // Let the terminal class handle the request
    Terminal->HandleTryAccess(mDecodeData->mClient, mTerminalSessionId, &mCommandName, mOptions, mOptionsCount, mDBId, mUnknown2);

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
    return true;
}
/*******************************************************************************************/
/**** PUdpQueryDB ****/
/*******************************************************************************************/
PUdpQueryDB::PUdpQueryDB( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1b";
  mOptionsCount = 0;
}

PUdpMsgAnalyser* PUdpQueryDB::Analyse()
{
  PMessage* TmpMsg = mDecodeData->mMessage;
  u16 OptionSize;

  mDecodeData->mName << "=QueryDB request from client";

  mTerminalSessionId = TmpMsg->U8Data( mDecodeData->Sub0x13Start + 6 );
  TmpMsg->SetNextByteOffset( mDecodeData->Sub0x13Start + 18 );
  ( *TmpMsg ) >> OptionSize; // Size of data
  ( *TmpMsg ) >> OptionSize; // Size of DB Command Name
  if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
  {
    ( *TmpMsg ) >> mDBCommandName;
  }
  ( *TmpMsg ) >> OptionSize; // Size of Command Name
  if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
  {
    ( *TmpMsg ) >> mCommandName; // null terminated string
  }

  while (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && ( mOptionsCount < mMaxOptions ) )
  {
    ( *TmpMsg ) >> OptionSize;
    if (( TmpMsg->GetNextByteOffset() < mDecodeData->Sub0x13StartNext ) && OptionSize )
    {
      ( *TmpMsg ) >> mOptions[mOptionsCount++];
      //if(mOptions[mOptionsCount-1].size() != (OptionSize-1)) Warning (but no pb)!
    }
    else
    {
      break;
    }
  }

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpQueryDB::DoAction()
{
  //PMessage* tmpMsg;
  //PClient* tClient = mDecodeData->mClient;
  bool Result = false;

  if ( gDevDebug )
  {
    Console->Print( "%s QueryDB request from client", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    Console->Print( "%s Open Terminal - Terminal session %04x (?)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTerminalSessionId );
    Console->Print( "%s DBCommand: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mDBCommandName.c_str() );
    Console->Print( "%s Command: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), i, mOptions[i].c_str() );
  }

  if ( mDBCommandName == "SPAWNVEHICLE" )
  {
    Result = ActionSpawnVehicle();
  }
  else if ( mDBCommandName == "REPAIRVEHICLE" )
  {
    Result = ActionRepairVehicle();
  }
  else if ( mDBCommandName == "DISMISSVEHICLE" )
  {
    Result = ActionDismissVehicle();
  }
  else
  {
      // Let Terminal try before error
      Result = Terminal->HandleQueryDB(mDecodeData->mClient, &mDBCommandName, &mCommandName, mOptions, mOptionsCount);
  }

  if ( !Result )
  {
    Console->Print( "%s PUdpQueryDB - Error or unknown command %s", Console->ColorText( RED, BLACK, "[WARNING]" ), mDBCommandName.c_str() );
    for ( u8 i = 0; i < mOptionsCount; ++i )
      Console->Print( "%s Option %d: '%s'", Console->ColorText( RED, BLACK, "[NOTICE]" ), i, mOptions[i].c_str() );
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return Result;
}

bool PUdpQueryDB::ActionSpawnVehicle()
{
  PMessage* tmpMsg;
  PClient* tClient = mDecodeData->mClient;
  PChar* tChar = tClient->GetChar();

  if ( mOptionsCount == 3 ) // 0, VhcId, CharId
  {
    u32 VhcId = ( u32 )atol( mOptions[1].c_str() );
    //u32 CharId = (u32)atol(mOptions[2].c_str());
    // !!! validate values !!!
    // !!! + check CharId = current char && CharId is owner of VhcId
    u8 nStatus = 1; // 1=OK, 0=Err
    u16 nErrcode = 16; // 0=n/a 16=Already Spawned // MsdId = 2500+nErrcode [MISC]

    tmpMsg = MsgBuilder->BuildDBRequestStatusMsg( tClient, &mCommandName, nStatus, nErrcode );
    tClient->SendUDPMessage( tmpMsg );

    PVhcCoordinates NewPosition;
    PWorld* CurrentWorld = Worlds->GetWorld( tChar->GetLocation() );
    bool relativePos = false;
    if ( tChar->GetLastUsedObject() && !WorldActors->IsDynamicActor( tChar->GetLastUsedObject() ) && CurrentWorld )
    {
      const PFurnitureItemTemplate* tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate( tChar->GetLastUsedObject() / 1024 - 1 );
      if ( tFurnitureTemplate && ( tFurnitureTemplate->GetFunctionType() == 28 ) ) // vhc term
      {
        f32 decal = 1000; // distance at which to spawn the vhc
        f32 nPosX, nPosY, nPosZ;
        tFurnitureTemplate->GetPos( &nPosX, &nPosY, &nPosZ );
        nPosX += 32000;
        nPosY += 32000;
        f32 dX = ( tChar->Coords.mX - nPosX );
        f32 dY = ( tChar->Coords.mY - nPosY );
        f32 d = decal / sqrt( dX * dX + dY * dY );
        NewPosition.SetPosition( static_cast<u16>( nPosY + d * dY ), ( tChar->Coords ).mZ + 100, static_cast<u16>( nPosX + d * dX ), ( tChar->Coords ).mUD, 34683, 32403 );
        relativePos = true;
      }
    }

    if( ! relativePos )
      NewPosition.SetPosition(( tChar->Coords ).mY, ( tChar->Coords ).mZ + 150, ( tChar->Coords ).mX, ( tChar->Coords ).mUD, 34683, 32403 );

    PSpawnedVehicle* NewVhc = Vehicles->SpawnVehicle( VhcId, tChar->GetLocation(), &NewPosition );
    if ( NewVhc )
    {
      tmpMsg = MsgBuilder->BuildVhcPosUpdateMsg( NewVhc );
      ClientManager->UDPBroadcast( tmpMsg, tChar->GetLocation() );
    }

    return true;
  }
  else
    return false;
}

bool PUdpQueryDB::ActionRepairVehicle()
{
  PMessage* tmpMsg;
  PClient* tClient = mDecodeData->mClient;

  if ( mOptionsCount == 3 ) // 0, VhcId, CharId
  {
    //u32 VhcId = (u32)atol(mOptions[1].c_str());
    //u32 CharId = (u32)atol(mOptions[2].c_str());
    // !!! validate values !!!

    u8 nStatus = 1; // 1=OK, 0=Err
    u16 nErrcode = 18; // 0=n/a 18=Still Spawned, 19=Not enough money

    tmpMsg = MsgBuilder->BuildDBRequestStatusMsg( tClient, &mCommandName, nStatus, nErrcode );
    tClient->SendUDPMessage( tmpMsg );

    // Action here

    return true;
  }
  else
    return false;
}

bool PUdpQueryDB::ActionDismissVehicle()
{
  PMessage* tmpMsg;
  PClient* tClient = mDecodeData->mClient;
  //PChar* tChar = tClient->GetChar();
  bool Result = false;

  if ( mOptionsCount == 3 ) // 0, VhcId, CharId
  {
    u32 VhcId = ( u32 )atol( mOptions[1].c_str() );
    //u32 CharId = (u32)atol(mOptions[2].c_str());
    // !!! validate values !!!
    // !!! + check CharId = current char && CharId is owner of VjhcId
    // !!! Check vhc empty
    u8 nStatus = 0; // 1=OK, 0=Err
    u16 nErrcode = 17; // 0=n/a 17=Not Spawned
    PVehicleInformation nInfo;
    u32 tLocalId = 0;
    u32 tLocation = 0;

    PSpawnedVehicle* tVhc = Vehicles->GetSpawnedVehicle( VhcId );
    if ( tVhc )
    {
      tLocalId = tVhc->GetLocalId();
      tLocation = tVhc->GetLocation();
      u32 tCharId;
      PChar* tChar;
      for ( u8 i = 0; i < tVhc->GetNumSeats(); ++i )
      {
        if (( tCharId = tVhc->GetSeatUser( i ) ) )
        {
          if (( tChar = Chars->GetChar( tCharId ) ) )
          {
            PUdpCharExitChair::DoLeaveChair( tChar, NULL, tVhc );
          }
        }
      }
      Result = Vehicles->UnspawnVehicle( VhcId );
    }

    if ( Result )
    {
      nStatus = 1;
      nErrcode = 0;
    }
    tmpMsg = MsgBuilder->BuildDBRequestStatusMsg( tClient, &mCommandName, nStatus, nErrcode );
    tClient->SendUDPMessage( tmpMsg );

    if ( Result )
    {
      tmpMsg = MsgBuilder->BuildRemoveWorldObjectMsg( tLocalId );
      ClientManager->UDPBroadcast( tmpMsg, tLocation );
    }
    return true;
  }
  else
    return false;
}

/*******************************************************************************************/
/**** PUdpTeminal0x1f ****/
/*******************************************************************************************/
PUdpTeminal0x1f::PUdpTeminal0x1f( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x1f";
}

PUdpMsgAnalyser* PUdpTeminal0x1f::Analyse()
{
  mDecodeData->mName << "=Open Terminal";
  mTerminalSessionId = mDecodeData->mMessage->U8Data( mDecodeData->Sub0x13Start + 6 );

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpTeminal0x1f::DoAction()
{
  if ( gDevDebug )
    Console->Print( "%s Open Terminal - Terminal session %04x (?)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mTerminalSessionId );
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
