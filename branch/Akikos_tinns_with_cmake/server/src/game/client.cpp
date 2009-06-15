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
	client.cpp

	Authors:
	- v00d00
	- Akiko
	- Namikon

	MODIFIED: 30 Nov 2005 Akiko
	REASON: - added GPL
		- added modifications by Namikon
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
*/

#include "main.h"

#include "include/worlds.h"
#include "include/msgbuilder.h"
#include "include/subway.h"
#include "decoder/udp_charmove.h"

PClient::PClient( int Index )
{
  mAccountID = 0;
  mAccountLevel = 0;
  mIndex = Index;
  mCharID = 0;
  mConnection = PCC_NONE;
  mRemotePort = 0;
  m_TCPConnection = NULL;
  m_UDPConnection = NULL;

  for ( int i = 0; i < DEBUG_MODES ; i++ )
    mDebugMode[i] = false;

  //********
  mAwaitingWarpto = false;
  mTargetX = 0;
  mTargetY = 0;
  mTargetZ = 0;
  //********
  mActorRemoveMode = false;
  mAcceptNPCUpdates = false;

  testval8 = 0;
}

PClient::~PClient()
{
  if ( m_TCPConnection )
  {
    delete m_TCPConnection;
    m_TCPConnection = NULL;
  }
  if ( m_UDPConnection )
  {
    delete m_UDPConnection;
    m_UDPConnection = NULL;
  }
}
/// ******************************************************

u16 PClient::GetUDP_ID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetUDP_ID();
  else
    Console->Print( "%s Unable to get UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

void PClient::SetUDP_ID( int id )
{
  if ( m_UDPConnection )
    m_UDPConnection->SetUDP_ID( id );
  else
    Console->Print( "%s Unable to set UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::IncreaseUDP_ID()
{
  if ( m_UDPConnection )
    m_UDPConnection->IncreaseUDP_ID();
  else
    Console->Print( "%s Unable to increase UDP_ID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

u16 PClient::GetSessionID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetSessionID();
  else
    Console->Print( "%s Unable to get UDP SessionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

u16 PClient::GetTransactionID()
{
  if ( m_UDPConnection )
    return m_UDPConnection->GetTransactionID();
  else
    Console->Print( "%s Unable to get UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
  return 0;
}

void PClient::ResetTransactionID()
{
  if ( m_UDPConnection )
    m_UDPConnection->ResetTransactionID();
  else
    Console->Print( "%s Unable to reset UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::IncreaseTransactionID( u8 nInc )
{
  if ( m_UDPConnection )
    m_UDPConnection->IncreaseTransactionID( nInc );
  else
    Console->Print( "%s Unable to increase UDP TransactionID, UDP ConnectionClass is not yet initialized!", Console->ColorText( RED, BLACK, "[WARNING]" ) );
}

void PClient::FillInUDP_ID( PMessage* nMessage )
{
  u16 CurrPos;

  if (( nMessage->GetSize() > 9 ) && ( nMessage->U8Data( 0x00 ) == 0x13 ) )
  {
    CurrPos = 5;
    while ( CurrPos < nMessage->GetSize() ) // do UDP_ID mgt for each 0x03 subtype subpacket
    {
      if ( nMessage->U8Data( CurrPos + 1 ) == 0x03 )
      {
        IncreaseUDP_ID();
        nMessage->U16Data( CurrPos + 2 ) = GetUDP_ID();
      }
      CurrPos = CurrPos + nMessage->U8Data( CurrPos ) + 1;
    }
  }
  nMessage->U16Data( 0x01 ) = GetUDP_ID();
  nMessage->U16Data( 0x03 ) = GetSessionID();
}

void PClient::FragmentAndSendUDPMessage( PMessage* nMessage, u8 nType )
{
  PMessage* ChunkBuffer;
  PMessage* ChunkMsg;
  const u16 ChunkSize = 220;
  u16 StartIncUDPIDOnChunk = 0;
  u16 IncludedHeaderSize = 0;
  bool ReplaceFirstByte = false;
  u8 ReplaceFirstByteValue = 0;
  u16 MultiTriggeringSize = 0;

  switch ( nType )
  {
    case 0x04:
    {
      Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Message type 0x%02x not managed yet", nType );
      break;
    }
    case 0x05: //CharOpenContainerMsg with header & UDP_ID incremented
    {
      StartIncUDPIDOnChunk = 1;
      IncludedHeaderSize = 9;
      ReplaceFirstByte = true;
      ReplaceFirstByteValue = 0x15;
      MultiTriggeringSize = 230;
      break;
    }
    case 0x19: //BaselineMsg (with no header)
    {
      break;
    }
    case 0xac: //BuildTraderItemListMsg with header & UDP_ID incremented
    {
      StartIncUDPIDOnChunk = 1;
      IncludedHeaderSize = 9;
      ReplaceFirstByte = true;
      ReplaceFirstByteValue = 0x15;
      MultiTriggeringSize = 230;
      break;
    }
    default:
    {
      Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Message type 0x%02x not managed", nType );
      break;
    }
  }

  if ( nMessage->GetSize() >= MultiTriggeringSize )
  {
    if ( gDevDebug )
      Console->Print( YELLOW, BLACK, "[Debug] Fragmenting message type 0x%02x", nType );
    if ( ReplaceFirstByte )
    {
      nMessage->U8Data( IncludedHeaderSize ) = ReplaceFirstByteValue;
    }

    u16 ChunksNum = ( nMessage->GetSize() - IncludedHeaderSize + ChunkSize - 1 ) / ChunkSize;

    for ( u16 ChunkID = 0; ChunkID < ChunksNum; ChunkID++ )
    {
      ChunkBuffer = nMessage->GetChunk( IncludedHeaderSize, ChunkSize, ChunkID );
      if ( ChunkBuffer == NULL )
      {
        Console->Print( RED, BLACK, "[Error] PClient::FragmentAndSendUDPMessage: Bad chunk number: %d for size %d", ChunksNum, nMessage->GetSize() );
        break;
      }

      ChunkMsg = new PMessage( ChunkSize + 15 );
      if ( ChunkID >= StartIncUDPIDOnChunk )
      {
        IncreaseUDP_ID();
      }

      *ChunkMsg << ( u8 )0x13;
      *ChunkMsg << ( u16 )GetUDP_ID();
      *ChunkMsg << ( u16 )GetSessionID();
      *ChunkMsg << ( u8 )( 9 + ChunkBuffer->GetSize() );
      *ChunkMsg << ( u8 )0x03;
      *ChunkMsg << ( u16 )GetUDP_ID();
      *ChunkMsg << ( u8 )0x07; // Fragmented
      *ChunkMsg << ( u16 )ChunkID;
      *ChunkMsg << ( u16 )ChunksNum;
      *ChunkMsg << ( u8 )nType;
      *ChunkMsg << *ChunkBuffer;

      delete ChunkBuffer;
      //Console->Print(YELLOW, BLACK, "[Debug] Sending Fragment %d/%d", ChunkID+1, ChunksNum);
      //ChunkMsg->Dump();
      SendUDPMessage( ChunkMsg );
    }
    delete nMessage;
  }
  else
  {
    //Console->Print(YELLOW, BLACK, "[Debug] Sending message WITHOUT Fragmenting");
    SendUDPMessage( nMessage );
  }
}

/// ******************************************************

void PClient::SetDebugMode( PDebugMode nDebugID, bool nVal )
{
  if ( nDebugID == DBG_ALL )
  {
    for ( int i = 0; i < DEBUG_MODES ; i++ )
      mDebugMode[i] = nVal;
  }
  else
    mDebugMode[nDebugID] = nVal;
}

bool PClient::ChangeCharLocation( u32 nLocation, bool DoForce )
{
  if ( gDevDebug )
    Console->Print( "%s PClient::ChangeCharLocation", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
  if ( Worlds->IsValidWorld( nLocation ) )
  {
    mAcceptNPCUpdates = false; // Zone changed, reject NPC updates till initial NPC spawn
    PChar* tChar = GetChar();
    u32 CurrentLocation = tChar->GetLocation();
    if (( CurrentLocation == nLocation ) && !DoForce )
      return true;

    // DoForce is used in GM teleport and jail/unjail command to free player from jail
    if (( tChar->IsJailed() == true ) && !DoForce )
    {
      if ( CurrentLocation != 550 && CurrentLocation != 551 ) // If player managed to "get out of jail" somehow, replace him there
      {
        // Do nothing here. This doesnt work as long as we are able to warp the player around just by
        // sending some packets out. Enable/modify this if fixed
        //tChar->SetLocation(550);
      }
      // Player still in 550 or 551 and trying to warp out (by death for example) just
      // return true and let him respawn at 550 entrypoint 0
      //return true;
    }

    PWorld* nWorld;
    if (( nWorld = Worlds->LeaseWorld( nLocation ) ) )
    {
      if ( tChar->GetLocationLeased() )
      {
        // TAke care of sitting chars
        u32 ChairObjectId;
        u8 tSeatId;
        PSeatType tSeatType;
        if (( tSeatType = tChar->GetSeatInUse( &ChairObjectId, &tSeatId ) ) )
        {
          bool vhcZoning = false;
          PSpawnedVehicle* tVhc = 0;
          if ( (tSeatType == seat_vhc) && IsVhcZoning() ) // If seat is vhc,
          { // Do  additionnal check
            if (( tVhc = nWorld->GetSpawnedVehicules()->GetVehicle( ChairObjectId ) ) )
            {
              if ( tVhc->GetSeatUser( tSeatId ) == tChar->GetID() )
              {
                vhcZoning = true;
              }
            }
          }

          if ( ! vhcZoning )
          {
            PUdpCharExitChair::DoLeaveChair( tChar, this, tVhc, true );
          }
          /*{
            Worlds->GetWorld( CurrentLocation )->CharLeaveChair( GetLocalID(), ChairObjectId );
            tChar->SetSeatInUse( seat_none );
          }*/
        }
        this->InitWarpCircle();
        this->InitCharVanish();
        Worlds->ReleaseWorld( CurrentLocation );
      }
      tChar->SetLocation( nLocation );
      tChar->SetLocationLeased();

      return true;
    }
  }
  else if ( nLocation != 1 ) // try to fall back if bad location
  {
    Console->Print( YELLOW, BLACK, "[Warning] Client %d want to zone to invalid world %d. Falling back to world 1", mIndex, nLocation );
    return ChangeCharLocation( 1, DoForce );
  }

  return false;
}

void PClient::GameDisconnect()
{
  mAccountID = 0;

  if ( m_TCPConnection )
  {
    delete m_TCPConnection;
    m_TCPConnection = NULL;
  }

  if ( m_UDPConnection )
  {
    delete m_UDPConnection;
    m_UDPConnection = NULL;
  }

  /**** Will be better to put that char-saving-at-disconnect in Char destructor, when only used Chars will be loaded ****/
  PChar *tChar = GetChar();
  if ( tChar )
  {
    SetZoning();
    if ( tChar->GetLocationLeased() )
    {
      if ( tChar->GetSeatInUse() )
      {
        //if(gDevDebug)
          Console->Print( "%s Trying to get leaving char out of her seat", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
        PUdpCharExitChair::DoLeaveChair( tChar, this, NULL, true );
      }
      /* // replaced by the lines above
      PSeatType cSeatType;
      u32 cSeatObjectId;
      u8 cSeatId;
      PWorld* cWorld;

      cWorld = Worlds->GetWorld(tChar->GetLocation());
      cSeatType = tChar->GetSeatInUse(&cSeatObjectId, &cSeatId);
      if(cSeatType)
      {
        if(cSeatType == seat_chair)
        {
          cWorld->CharLeaveChair(GetLocalID(), cSeatObjectId);
          tChar->SetSeatInUse(seat_none);
        }
        else if(cSeatType == seat_subway)
        {
          Subway->UnsetSeatUser(cSeatObjectId, cSeatId, GetLocalID());
          tChar->SetSeatInUse(seat_none);
        }
        else if(cSeatType == seat_vhc)
        {
          cWorld->GetSpawnedVehicules()->GetVehicle(cSeatObjectId)->UnsetSeatUser(cSeatId, GetLocalID());
          tChar->SetSeatInUse(seat_none);
        }
        else
        {
          Console->Print(RED, BLACK, "[Notice] PClient::GameDisconnect : Leaving seat of unkown type %d", cSeatType);
        }
      }
      */
    }

    if ( tChar->IsDirty() )
    {
      bool res = tChar->SQLSave();
      if ( res )
        Console->Print( "%s GameDisconnect: Char %i (Client %i) saved before disconnect.", Console->ColorText( GREEN, BLACK, "[DEBUG]" ),  tChar->GetID(), mIndex );
      else
        Console->Print( RED, BLACK, "[DEBUG] GameDisconnect: Char %i (Client %i) saving before disconnect and FAILED.", tChar->GetID(), mIndex );
    }
    else
    {
      Console->Print( "%s GameDisconnect: Char %i (Client %i) no save needed.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tChar->GetID(), mIndex );
      if ( !tChar->IsOnline() )
        Console->Print( "%s GameDisconnect: Char %i (Client %i) wasn't marked as ingame anyway...", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tChar->GetID(), mIndex );
    }

    if ( tChar->GetLocationLeased() )
    {
      if ( gDevDebug )
        Console->Print( "%s Sending char leaving effect", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      InitWarpCircle();
      InitCharVanish();

      Worlds->ReleaseWorld( tChar->GetLocation() );
      tChar->SetLocationLeased( false );
    }

    Chars->RemoveChar( mCharID );
    delete tChar;
    mCharID = 0;
  }
  else
  {
    //Console->Print(YELLOW, BLACK, "GameDisconnect: Client %i had no char online.", mIndex);
  }
  /**********************************/

  //mConnection &= ~PCC_GAME;
  mConnection = PCC_NONE;
}

void PClient::RefreshAccountInfo( PAccount *Account )
{
  mAccountID = Account->GetID();
  mAccountLevel = Account->GetLevel();
}

void PClient::Update()
{
  if ( m_TCPConnection )
  {
    if ( m_TCPConnection->timeOut() )
    {
      Console->Print( "%s GameSocket: Client %i: timeout", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), mIndex );
      GameServer->ClientDisconnected( this );
    }
    else
    {
      if ( !m_TCPConnection->update() )
      {
        GameServer->ClientDisconnected( this );
      }
    }
  }

  if ( m_UDPConnection )
  {
    if ( m_UDPConnection->timeOut() )
    {
      Console->Print( "%s Game UDP: Client %i: timeout", Console->ColorText( YELLOW, BLACK, "[NOTICE]" ), mIndex );
      GameServer->UDPStreamClosed( this );
    }
    else
    {
      if ( !m_UDPConnection->update() )
      {
        GameServer->UDPStreamClosed( this );
      }
    }
  }
}

PChar* PClient::GetChar() const
{
  if ( mCharID )
  {
    return Chars->GetChar( mCharID );
  }
  else
  {
    return NULL;
  }
}

bool PClient::GetCharAwaitingWarpto( u16* PosX, u16* PosY, u16* PosZ )
{
  if ( PosX )
    *PosX = mTargetX;
  if ( PosY )
    *PosY = mTargetY;
  if ( PosZ )
    *PosZ = mTargetZ;

  return mAwaitingWarpto;
  /*
    if(mAwaitingWarpto == true)
    {

        // Position update doesnt work. Uncomment&Change function if ever required again
        mAwaitingWarpto = false;
            (GetChar()->Coords).mX = mTargetX;
            (GetChar()->Coords).mY = mTargetY;
            (GetChar()->Coords).mZ = mTargetZ;
        return true;
  //        PMessage* tmpMsg_posupdate;
  //
  //        tmpMsg_posupdate = MsgBuilder->BuildCharPosMoveMsg(this, mTargetX, mTargetY, mTargetZ);
  //        ClientManager->UDPBroadcast(tmpMsg_posupdate, this);
  //        tmpMsg_posupdate = NULL;
    }
    return false;
  */
}

void PClient::InitWarpCircle()
{
  PMessage* tmpMsg_circle = MsgBuilder->BuildCharShowGlowCircleMsg( this );
  ClientManager->UDPBroadcast( tmpMsg_circle, this, 1000, true ); // send only in a 1000 radius
}

void PClient::InitCharVanish()
{
  PMessage* tmpMsg_vanish = MsgBuilder->BuildRemoveWorldObjectMsg( GetLocalID() );
  ClientManager->UDPBroadcast( tmpMsg_vanish, this, 0, true );
}
