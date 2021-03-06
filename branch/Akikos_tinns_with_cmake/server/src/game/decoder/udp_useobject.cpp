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

	MODIFIED: 10 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem

	TODO:
	- use only RawObjectIDs to avoid complication & errors
*/


#include "main.h"

#include "udp_useobject.h"
#include "udp_vhc.h"

#include "include/worlds.h"
#include "include/furnituretemplate.h"
#include "include/doortemplate.h"
#include "include/appartements.h"
#include "include/container.h"
#include "include/subway.h"
#include "include/vhcaccessrequest.h"


u32 gVhcId = 0x3ff;

/**** PUdpVhcMove ****/

PUdpUseObject::PUdpUseObject( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x17";
}

PUdpMsgAnalyser* PUdpUseObject::Analyse()
{
  mDecodeData->mName << "=Object use";

  mRawItemID = mDecodeData->mMessage->U32Data( mDecodeData->Sub0x13Start + 8 );
  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpUseObject::DoAction()
{
  PClient* nClient = mDecodeData->mClient;
  PChar* nChar = nClient->GetChar();
  u32 ItemID;
  char DbgMessage[128];
  PMessage* tmpMsg;

  bool tHandleDynamicActor = false;

  /*PMessage* cMsg = mDecodeData->mMessage;
  u32 ClientTime = cMsg->U32Data(mDecodeData->Sub0x13Start+2);

  tmpMsg = MsgBuilder->BuildPingMsg(mDecodeData->mClient, ClientTime);
  mDecodeData->mClient->SendUDPMessage(tmpMsg);*/

  nChar->SetLastUsedObject( mRawItemID );

  if ( nClient->IsInRemoveActorMode() == true )
  {
    if ( WorldActors->IsDynamicActor( mRawItemID ) == true )
    {
      WorldActors->DelWorldActor( nClient, mRawItemID );
      mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
      return true;
    }
    else
      Chat->send( nClient, CHAT_DIRECT, "System", "This is not an dynamic worldactor. To remove it please type @remove <rawID>" );
  }

  if ( gDevDebug )
  {
    Console->Print( "Char at y=%f (0x%04x) z=%f (0x%04x) x=%f (0x%04x)", ( f32 )( nChar->Coords.mY - 32000 ), nChar->Coords.mY, ( f32 )( nChar->Coords.mZ - 32000 ), nChar->Coords.mZ, ( f32 )( nChar->Coords.mX - 32000 ), nChar->Coords.mX );
    if ( mRawItemID & 1023 )
      Console->Print( "using item %d (0x%08x)", mRawItemID, mRawItemID );
    else
      Console->Print( "using item %d (0x%08x) [dat entry %d (0x%08x)]", mRawItemID, mRawItemID, mRawItemID / 1024 - 1, mRawItemID / 1024 - 1 );
  }

  if ( nClient->GetDebugMode( DBG_ITEMID ) )
  {
    if ( mRawItemID & 1023 )
      snprintf( DbgMessage, 128, "using item [raw: %d (0x%08x)]", mRawItemID, mRawItemID );
    else
      snprintf( DbgMessage, 128, "using item %d (0x%08x) [raw: %d (0x%08x)]", mRawItemID / 1024 - 1, mRawItemID / 1024 - 1, mRawItemID, mRawItemID );
    Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
  }

  PWorld* CurrentWorld = Worlds->GetWorld( nChar->GetLocation() );
  if ( CurrentWorld )
  {
    if ( WorldActors->IsDynamicActor( mRawItemID ) == true )
    {
      tHandleDynamicActor = true;
      if ( gDevDebug )
        Console->Print( "%s Dynamic actor found, processing...", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    }

    if ( mRawItemID & 1023 && tHandleDynamicActor == false ) // non-furniture objects
    {
      if ( mRawItemID > 0x80 ) // maybe door
      {
        if ( nClient->GetDebugMode( DBG_ITEMID ) )
        {
          snprintf( DbgMessage, 128, "Door : %d", mRawItemID - 0x80 );
          Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
        }
        const PDoorTemplate* tDoor = CurrentWorld->GetDoor( mRawItemID - 0x80 );
        if ( tDoor )
        {
          if ( tDoor->IsTriggeredDoor() )
          {
            tmpMsg = MsgBuilder->BuildText100Msg( nClient, 6, mRawItemID );
            nClient->SendUDPMessage( tmpMsg );
          }
          else
          {
            if ( gDevDebug )
              Console->Print( "%s Opening %s door %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), ( tDoor->IsDoubleDoor() ? "double" : "simple" ), mRawItemID - 0x80 );
            tmpMsg = MsgBuilder->BuildDoorOpenMsg( mRawItemID, tDoor->IsDoubleDoor() );
            ClientManager->UDPBroadcast( tmpMsg, nClient );
          }
          mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        }
      }
      if ( !( mDecodeData->mState & DECODE_ACTION_DONE ) ) // else might be PC, NPC, VHC
      {
        if ( gDevDebug )
          Console->Print( "%s Clicking on char, npc or vhc %d (%x) - time = %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID, mRawItemID, GameServer->GetGameTime() );

        if ( PSpawnedVehicles::IsPotentialSpawnedVehicle( mRawItemID ) )
        {
          bool vhcFound = false;

          if ( nChar->GetSeatInUse() == seat_none ) // Refuse if Char is already sitting somewhere
          {
            PSpawnedVehicle* tVhc = CurrentWorld->GetSpawnedVehicules()->GetVehicle( mRawItemID );
            if ( tVhc )
            {
              if ( gDevDebug )
                Console->Print( "%s Using vhc %d (0x%04x)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID, mRawItemID );
              vhcFound = true;

              if ( tVhc->GetInformation().GetOwnerCharId() ==  nChar->GetID() ) // Requester is the owner
              {
                PUdpVhcUse::DoFreeSitting( nClient,  tVhc, mRawItemID );
              }
              else // Requester is not the owner
              {
                if ( tVhc->GetNumSeats() == 1 ) // single seat vhc
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 17, mRawItemID ); // "Not your vhc" msg
                  nClient->SendUDPMessage( tmpMsg );
                } // multi seats vhc
                else if ( ! tVhc->IsCharInside( tVhc->GetInformation().GetOwnerCharId() ) )
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 18, mRawItemID ); // "Owner not on board" msg
                  nClient->SendUDPMessage( tmpMsg );
                }
                else if ( tVhc->GetNbFreeSeats() == 0 )
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 5, mRawItemID ); // "No free seat" msg
                  nClient->SendUDPMessage( tmpMsg );
                }
                else
                {
                  // Send request to owner
                  PClient* ownerClient = ClientManager->getClientByChar( tVhc->GetInformation().GetOwnerCharId() );
                  if ( ownerClient )
                  {
                    PChar* ownerChar = ownerClient->GetChar();
                    // We could check for valid allowed access before adding a new one
                    u32 newReqId = ownerChar->GetVhcAccessRequestList()->Add( nChar->GetID(), tVhc->GetInformation().GetVehicleId() );
                    if ( newReqId )
                    {
                      tmpMsg = MsgBuilder->BuildVhcAccessRequestMsg( ownerClient, newReqId, nChar->GetID(), nClient->GetID(), tVhc->GetInformation().GetVehicleId() );
                      ownerClient->SendUDPMessage( tmpMsg );
                      tmpMsg = MsgBuilder->BuildText100Msg( nClient, 19, mRawItemID ); // "req transmitted" msg
                      nClient->SendUDPMessage( tmpMsg );
                    }
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID ); // Undefined failure / Owner not available
                    nClient->SendUDPMessage( tmpMsg );
                  }
                }
              }
              //////// Msg100 id
              //22 can't use that seat
            }
            else if (( nChar->GetLocation() == PWorlds::mNcSubwayWorldId ) && Subway->IsValidSubwayCab( mRawItemID ) && ( nChar->GetSeatInUse() == seat_none ) ) // Entering subway
            {
              vhcFound = true;
              if ( Subway->IsDoorOpen( mRawItemID, GameServer->GetGameTime() ) )
              {
                u8 freeSeat = Subway->GetFreeSeat( mRawItemID );
                if ( freeSeat && Subway->SetSeatUser( mRawItemID, freeSeat, nChar->GetID() ) )
                {
                  nChar->SetSeatInUse( seat_subway, mRawItemID, freeSeat );
                  tmpMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, mRawItemID, freeSeat );
                  ClientManager->UDPBroadcast( tmpMsg, nClient );
                }
                else
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 6, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              else
              {
                tmpMsg = MsgBuilder->BuildText100Msg( nClient, 6, mRawItemID );
                nClient->SendUDPMessage( tmpMsg );
              }
            }
            // else error: invalid vhc
          } // else char alreay sitting
          if ( vhcFound )
            mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        }

        if ( !( mDecodeData->mState & DECODE_ACTION_DONE ) ) // not a vhc
        {
          // Is it a PC ?
          PClient* tClient;
          if (( tClient = CurrentWorld->GetClientByCharLocalId( mRawItemID ) ) )
          {
            /*if(gDevDebug)*/
            Console->Print( "%s clicking on PC %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID );
            tmpMsg = MsgBuilder->BuildCharInteractionMenuMsg( nClient, mRawItemID );
            nClient->SendUDPMessage( tmpMsg );
            mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
          }
        }

        if ( !( mDecodeData->mState & DECODE_ACTION_DONE ) ) // not a vhc nor a pc
        {
		  //Console->Print(">>> Searching world");
          // Is it a NPC ?
          PNPC* targetNPC = 0;
          PNPCWorld* currentNPCWorld = NPCManager->GetWorld( nChar->GetLocation() );
          if ( currentNPCWorld )
          {
              //Console->Print(">>> Searching NPC (SQL Version)");
            targetNPC = currentNPCWorld->GetNPC( mRawItemID );
            if(!targetNPC)
            {
                //Console->Print(">>> Searching NPC (DEF Version)");
                // Note to myself: This is UGLY!!!! and BAD!!! but it works for now. CHANGE THIS!
                targetNPC = currentNPCWorld->GetNPC( mRawItemID - 255 );
            }
          }
          if ( targetNPC )
          {
            /*if(gDevDebug)*/
            Console->Print( "%s Player talks to NPC %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID );
            //if(gDevDebug) tContainer->Dump();

            // Well its not "start a conversation" its more "User clicked NPC, do anything with it (Trade, script,...)
            targetNPC->StartConversation(nClient);

            //tmpMsg = MsgBuilder->BuildTraderItemListMsg( nClient, mRawItemID );
            //tmpMsg->Dump();
            //nClient->FragmentAndSendUDPMessage( tmpMsg, 0xac );
          }
        }

        mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
      }
    }
    else // furniture objects
    {
      const PFurnitureItemTemplate* tFurnitureTemplate = NULL;
      const PDefWorldModel* tFurnitureModel = NULL;
      if ( tHandleDynamicActor == false )
      {
        // We have an STATIC ACTOR, which means we DONT KNOW the FUNCTION VALUE from pak_worldmodel.def yet
        // (the setentry one). So we need to get it over the .dat file:

        // Dat files have smaller IDs
        ItemID = mRawItemID / 1024 - 1;

        // Now grab the template from .dat file
        tFurnitureTemplate = CurrentWorld->GetFurnitureItemTemplate( ItemID );

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
        tFurnitureModel = CurrentWorld->GetFurnitureItemModel( ItemID );
      }
      else
      {
        // We have an DYNAMIC ACTOR, which means we DO KNOW the FUNCTION VALUE (stored in SQL).

        // First, assign the RawID to the "real" itemID, which is used for several objects to identify them clientside
        ItemID = mRawItemID;

        // Now get the get the function value:
        int tFunctionVal = WorldActors->GetWorldActorFunctionID( mRawItemID );

        // Then get the FUNCTION VALUE as furniture model so we can access its subvalues etc
        tFurnitureModel = GameDefs->WorldModels()->GetDef( tFunctionVal );

        if ( gDevDebug )
          Console->Print( "%s Processing dynmic actor %d with Functionvalue %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), mRawItemID, tFunctionVal );
      }

      if ( tFurnitureModel ) // valid active furniture (else invalid or passive furniture)
      {
        if ( nClient->GetDebugMode( DBG_ITEMID ) )
        {
          snprintf( DbgMessage, 128, "Item : %s", tFurnitureModel->GetName().c_str() );
          Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
        }

        if ( tFurnitureModel->GetUseFlags() & ufChair )
        {
          u32 cSeatObjectId;
          PSeatType cSeatType = nChar->GetSeatInUse( &cSeatObjectId );
          if (( cSeatType == seat_none ) || ( cSeatType == seat_chair ) )
          {
            if ( CurrentWorld->CharUseChair( nClient->GetLocalID(), ItemID ) )
            {
              if ( gDevDebug )
                Console->Print( "%s Localchar %d was previously using chair %d.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nClient->GetLocalID(), cSeatObjectId );
              if (( cSeatType ) && ( cSeatObjectId != ItemID ) )
              {
                CurrentWorld->CharLeaveChair( nClient->GetLocalID(), cSeatObjectId );
              }
              nChar->SetSeatInUse( seat_chair, ItemID );
              if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
              {
                tFurnitureTemplate->GetFrontPos( &( nChar->Coords.mX ), &( nChar->Coords.mY ), &( nChar->Coords.mZ ) );
                //(nChar->Coords).mLR = tFurnitureTemplate->GetFrontLR();
              }
              else
              {
                WorldActors->GetFrontPos( mRawItemID, &( nChar->Coords.mX ), &( nChar->Coords.mY ), &( nChar->Coords.mZ ) );
                //(nChar->Coords).mLR = WorldActors->GetFrontLR();
              }

              tmpMsg = MsgBuilder->BuildCharUseSeatMsg( nClient, mRawItemID );
              ClientManager->UDPBroadcast( tmpMsg, nClient );
            }
            else
            {
              tmpMsg = MsgBuilder->BuildText100Msg( nClient, 1, mRawItemID ); // "Already in use" msg
              nClient->SendUDPMessage( tmpMsg );
            }
          }

          mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
        }
        else
        {
          if ( gDevDebug )
            Console->Print( "%s Item function type: %d value: %d", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), tFurnitureModel->GetFunctionType(), tFurnitureModel->GetFunctionValue() );

          DbgMessage[0] = 0;
          switch ( tFurnitureModel->GetFunctionType() )
          {
            case 6: //Respawn Station (GenRep)
            {
              u32 nLocation = nChar->GetLocation();
              u16 nEntity;
              if ( Worlds->IsPotentialAppartement( nLocation ) )
              {
                nLocation = 0xffffffff; // (u32)-1;
                nEntity = 0xffff; //(u16)-1;
              }
              else
              {
                //nEntity = MySQL->GetWorldItemOption(mRawItemID/256, nLocation, 1);
                // This is a kind of nearly-not-hardcoded-hack ...
                int nEntityInt = 0;
                if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
                {
                  nEntityInt = GameDefs->Respawns()->GetRespawnEntity( nChar->GetLocation(), tFurnitureTemplate->GetLinkedObjectID() );
                }
                else
                {
                  nEntityInt = GameDefs->Respawns()->GetRespawnEntity( nChar->GetLocation(), WorldActors->GetLinkedObjectID( mRawItemID ) );
                }

                nEntity = ( nEntityInt < 0 ? 0xffff : ( u16 )nEntityInt );
              }

              tmpMsg = MsgBuilder->BuildCharUseGenrepMsg( nClient, mRawItemID, nLocation, nEntity );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 7: //GoGuardian
            {
              tmpMsg = MsgBuilder->BuildCharUseGogoMsg( nClient );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 9: //Appartement Eingang
            {
              tmpMsg = MsgBuilder->BuildCharUseLiftMsg( nClient, mRawItemID, tFurnitureModel->GetFunctionValue() );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 10: //Appartement Ein/Ausgang
            {
              tmpMsg = MsgBuilder->BuildCharUseLiftMsg( nClient, mRawItemID, 0 );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 15: //HOLOMATCH EXIT
            {
              // temp hack + wrong entity... guess it works same as function 20
              u32 HoloNum = nChar->GetLocation() - 90000; // value 1 to 16, to transalte to 540..547 550..557 for worldmodel.def

              const PDefWorldModel* tHoloExitModel = GameDefs->WorldModels()->GetDef( 539 + HoloNum + ( HoloNum > 8 ? 2 : 0 ) );
              if ( tHoloExitModel && ( tHoloExitModel->GetFunctionType() == 14 ) ) // we use the Holomatch entry. Takes care of bad zone id
              {
                const PDefAppPlace* nAppPlace = GameDefs->AppPlaces()->GetDef( tHoloExitModel->GetFunctionValue() );
                if ( nAppPlace )
                {
                  u32 Location = nAppPlace->GetExitWorldID();
                  u16 Entity = nAppPlace->GetExitWorldEntity();
                  u8 SewerLevel = 0;

                  tmpMsg = MsgBuilder->BuildChangeLocationMsg( nClient, Location, Entity, SewerLevel, 0 ); //mRawItemID
                  nClient->SendUDPMessage( tmpMsg );
                  if ( gDevDebug )
                    Console->Print( "%s Holoexit: Client[%d] Char[%s] moving to zone %d (%s)", Console->ColorText( GREEN, BLACK, "[Debug]" ), nClient->GetID(), nChar->GetName().c_str(), Location, nAppPlace->GetName().c_str() );
                  if ( gDevDebug )
                    Console->Print( "%s Location=%d Entity=%d Level=%d", Console->ColorText( CYAN, BLACK, "[Debug]" ), Location, Entity, SewerLevel );
                }
                else
                {
                  Console->Print( "%s Client[%d] Char[%s] invalid destination %d (appplaces.def)", Console->ColorText( RED, BLACK, "[Warning]" ), nClient->GetID(), nChar->GetName().c_str(), tFurnitureModel->GetFunctionValue() );
                }
              }
              else
              {
                Console->Print( "%s Client[%d] Char[%s] invalid holoentry used %d", Console->ColorText( RED, BLACK, "[Warning]" ), nClient->GetID(), nChar->GetName().c_str(), tFurnitureModel->GetFunctionValue() );
              }
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 18: // "WORLDCHANGEACTOR"
            case 20: // "DATFILE WORLDCHANGE ACTOR"
            case 29: //Underground Exit
            {
              const PDefAppPlace* nAppPlace;
              if ( tFurnitureModel->GetFunctionType() == 29 )
              {
                nAppPlace = GameDefs->AppPlaces()->GetDef( nChar->GetLocation() ); // special for UG exit
              }
              else
              {
                nAppPlace = GameDefs->AppPlaces()->GetDef( tFurnitureModel->GetFunctionValue() );
              }
              if ( nAppPlace )
              {
                u32 Location = nAppPlace->GetExitWorldID();
                u16 Entity = nAppPlace->GetExitWorldEntity();
                u8 SewerLevel = 0;
                //if(((tFurnitureModel->GetFunctionType() == 20) && nAppPlace->GetSewerLevel()) || (tFurnitureModel->GetFunctionType() == 29))
                if (( tFurnitureModel->GetFunctionType() == 20 ) || ( tFurnitureModel->GetFunctionType() == 29 ) )
                {
                  SewerLevel = 1;
                }

                tmpMsg = MsgBuilder->BuildChangeLocationMsg( nClient, Location, Entity, SewerLevel, mRawItemID ); //mRawItemID
                nClient->SendUDPMessage( tmpMsg );
                if ( gDevDebug )
                {
                  Console->Print( "%s Dungeon in/out: Client[%d] Char[%s] moving to zone %d, %s", Console->ColorText( GREEN, BLACK, "[Debug]" ), nClient->GetID(), nChar->GetName().c_str(), Location, nAppPlace->GetName().c_str() );
                  Console->Print( "%s Location=%d Entity=%d Level=%d", Console->ColorText( CYAN, BLACK, "[Debug]" ), Location, Entity, SewerLevel );
                  Console->Print( "%s Function: %d, Value: %d - Sewer level in appplaces.def for this point: %d", Console->ColorText( CYAN, BLACK, "[Debug]" ), tFurnitureModel->GetFunctionType(), tFurnitureModel->GetFunctionValue(), nAppPlace->GetSewerLevel() );
                  Console->Print( "%s Worldmodel id: %d", Console->ColorText( CYAN, BLACK, "[Debug]" ), tFurnitureModel->GetID() );
                }
              }
              else
              {
                Console->Print( "%s Client[%d] Char[%s] invalid destination %d (appplaces.def)", Console->ColorText( RED, BLACK, "[Warning]" ), nClient->GetID(), nChar->GetName().c_str(), tFurnitureModel->GetFunctionValue() );
                // send a "refused" msg ?
              }
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              //nextAnalyser = new PUdpCharJump(mDecodeData);
              break;
            }

            case 2: //Terminal
            case 3: //Outfitter
            case 16: //HOLOMATCH REFRESH
            case 17: //HOLOMATCH HEAL & Recreation units
            case 26: //Outpost Switch
            {
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 31: //Venture Warp Station
            {
              tmpMsg = MsgBuilder->BuildCharUseVentureWarpMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 28: //Fahrzeug Depot Interface
            {
              tmpMsg = MsgBuilder->BuildCharUseVhcTerminalMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 11: //Appartement Klingel/�ffner
            {
              if ( Appartements->CanFreelyEnter( nChar, nChar->GetLocation() ) )
              {
                if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
                {
                  if ( tFurnitureTemplate->GetLinkedObjectID() )
                  {
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor( tFurnitureTemplate->GetLinkedObjectID() )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                    nClient->SendUDPMessage( tmpMsg );
                  }
                }
                else
                {
                  if ( WorldActors->GetLinkedObjectID( mRawItemID ) )
                  {
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + WorldActors->GetLinkedObjectID( mRawItemID ), CurrentWorld->GetDoor( WorldActors->GetLinkedObjectID( mRawItemID ) )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                    nClient->SendUDPMessage( tmpMsg );
                  }
                }
              }
              else
              {
                tmpMsg = MsgBuilder->BuildFurnitureActivateMsg( nClient, mRawItemID, 5 ); // Ring
                ClientManager->UDPBroadcast( tmpMsg, nClient );
              }
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 12: //Standard Button
            {
              if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
              {
                if ( tFurnitureTemplate->GetLinkedObjectID() )
                {
                  tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor( tFurnitureTemplate->GetLinkedObjectID() )->IsDoubleDoor() );
                  ClientManager->UDPBroadcast( tmpMsg, nClient );
                }
                else
                {
                  tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              else
              {
                if ( WorldActors->GetLinkedObjectID( mRawItemID ) )
                {
                  tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + WorldActors->GetLinkedObjectID( mRawItemID ), CurrentWorld->GetDoor( WorldActors->GetLinkedObjectID( mRawItemID ) )->IsDoubleDoor() );
                  ClientManager->UDPBroadcast( tmpMsg, nClient );
                }
                else
                {
                  tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 13: //Hack Button
            {
              if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
              {
                if ( tFurnitureTemplate->GetLinkedObjectID() )
                {
                  if ( nClient->GetAccountLevel() >= PAL_GM ) // Allow GameMasters and higher to just bypass HackButtons
                  {
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor( tFurnitureTemplate->GetLinkedObjectID() )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildTextIniMsg( nClient, 6, 106 ); // Damn, locked!
                    nClient->SendUDPMessage( tmpMsg );
                  }

                }
                else
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              else
              {
                u32 linkobjID = WorldActors->GetLinkedObjectID( mRawItemID );
                if ( linkobjID )
                {
                  if ( nClient->GetAccountLevel() >= PAL_GM ) // Allow GameMasters and higher to just bypass HackButtons
                  {
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + linkobjID, CurrentWorld->GetDoor( linkobjID )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildTextIniMsg( nClient, 6, 106 ); // Damn, locked!
                    nClient->SendUDPMessage( tmpMsg );
                  }

                }
                else
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            case 23: //EINTRITTSGELD BUTTON
            {
              if ( tHandleDynamicActor == false && tFurnitureTemplate != NULL )
              {
                if ( tFurnitureTemplate->GetLinkedObjectID() )
                {
                  u32 OldCash = nChar->GetCash();
                  u32 DoorFee = ( u32 )tFurnitureModel->GetFunctionValue();
                  if ( OldCash >= DoorFee )
                  {
                    u32 NewCash = nChar->SetCash( OldCash - DoorFee );
                    PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg( nClient, NewCash );
                    nClient->SendUDPMessage( tmpMsg_cash );
                    tmpMsg_cash = NULL;
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + tFurnitureTemplate->GetLinkedObjectID(), CurrentWorld->GetDoor( tFurnitureTemplate->GetLinkedObjectID() )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildTextIniMsg( nClient, 6, 12 );  // You dont have enough money!
                    nClient->SendUDPMessage( tmpMsg );
                  }
                }
                else
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }
              else
              {
                u32 linkobjID = WorldActors->GetLinkedObjectID( mRawItemID );
                if ( linkobjID )
                {
                  u32 OldCash = nChar->GetCash();
                  u32 DoorFee = ( u32 )tFurnitureModel->GetFunctionValue();
                  if ( OldCash >= DoorFee )
                  {
                    u32 NewCash = nChar->SetCash( OldCash - DoorFee );
                    PMessage* tmpMsg_cash = MsgBuilder->BuildCharMoneyUpdateMsg( nClient, NewCash );
                    nClient->SendUDPMessage( tmpMsg_cash );
                    tmpMsg_cash = NULL;
                    tmpMsg = MsgBuilder->BuildDoorOpenMsg( 0x80 + linkobjID, CurrentWorld->GetDoor( linkobjID )->IsDoubleDoor() );
                    ClientManager->UDPBroadcast( tmpMsg, nClient );
                  }
                  else
                  {
                    tmpMsg = MsgBuilder->BuildTextIniMsg( nClient, 6, 12 );  // You dont have enough money!
                    nClient->SendUDPMessage( tmpMsg );
                  }
                }
                else
                {
                  tmpMsg = MsgBuilder->BuildText100Msg( nClient, 0, mRawItemID );
                  nClient->SendUDPMessage( tmpMsg );
                }
              }

              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 1: //Itemcontainer
            {
              // TODO: Add check if container is already open / Make containers persistent
              // PContainer* tContainer = World->GetContainer(mRawItemID);
              PContainer* tContainer = new PContainerAutoCompactOnClose( INV_CABINET_MAXSLOTS );
              int functionVal = tFurnitureModel->GetFunctionValue();
              if ( functionVal <= 5 ) // force full random for cabinets for item testing
                functionVal = -1;
              tContainer->RandomFill( INV_CABINET_MAXSLOTS, functionVal );

              /*nItem = new PItem(19, 1, 250, 250, 250, 250, 250, 250);
              if(nItem->GetItemID());
                tContainer->AddItem(nItem);*/
              if ( gDevDebug ) Console->Print( "%s Temporary container created", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
              //if(gDevDebug) tContainer->Dump();
              if ( tContainer->StartUse( nChar->GetID() ) )
              {
                nChar->SetContainerInExclusiveUse( tContainer );
                tmpMsg = MsgBuilder->BuildCharOpenContainerMsg( nClient, mRawItemID, tContainer );
                //tmpMsg->Dump();
                nClient->FragmentAndSendUDPMessage( tmpMsg, 0x05 );
              }
              else
              {
                tmpMsg = MsgBuilder->BuildFurnitureActivateMsg( nClient, mRawItemID, 10 );
                nClient->SendUDPMessage( tmpMsg );
                tmpMsg = MsgBuilder->BuildText100Msg( nClient, 1, mRawItemID );
                nClient->SendUDPMessage( tmpMsg );
              }

              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 32: // Text message
            {
              tmpMsg = MsgBuilder->BuildText100Msg( nClient, ( u8 )( 255 & tFurnitureModel->GetFunctionValue() ), mRawItemID );
              nClient->SendUDPMessage( tmpMsg );

              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            // Temp placeholder implementations ============================
            case 4: //Trader
            {
              snprintf( DbgMessage, 128, "That's a trader" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 5: //Mineral
            {
              snprintf( DbgMessage, 128, "That's some minerals" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 8: //Hackterminal
            {
              snprintf( DbgMessage, 128, "That's a hacking terminal" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 14: //HOLOMATCH ENTRANCE
            {
              snprintf( DbgMessage, 128, "That's an holomatch entrance" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 19: //CLANTERMINAL
            {
              snprintf( DbgMessage, 128, "That's a clan terminal" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 25: //EXPLOSIVE
            {
              snprintf( DbgMessage, 128, "That's explosive !" );
              Chat->send( nClient, CHAT_GM, "Debug", DbgMessage );
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }
            case 0: //May be roadsign if touchable
            {
              if ( ( tFurnitureModel->GetUseFlags() & ufTouchable ) ) // Touchable ?
              {
                snprintf( DbgMessage, 128, "You're at %s", tFurnitureModel->GetName().c_str() );
                Chat->send( nClient, CHAT_GM, "Information", DbgMessage );
                tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
                nClient->SendUDPMessage( tmpMsg );
                mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
                break;
              }
              // else we continue to undefined
            }
            case 27: //Old goguardian
              //case 21: //LOCATION FOR 20
              //case 22: //
            case 24: //TUTORIALEXIT
              //case 30: //Static FX (Value=Type. 1=Fire 2=Smoke 3=Steam 4=Sparkle)
            {
              tmpMsg = MsgBuilder->BuildCharUseFurnitureMsg( nClient, mRawItemID );
              nClient->SendUDPMessage( tmpMsg );
              mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
              break;
            }

            default:
              break;
          }
        }
      }
      else
      {
        if ( gDevDebug )
          Console->Print( "%s Item not known from world template (maybe seen as PASSIVE ?)", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      }
    }

    mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] PUdpUseObject::DoAction : No World defined for client %d (char %d)", nClient->GetID(), nChar->GetID() );
    mDecodeData->mState = DECODE_ACTION_FAILED | DECODE_ACTION_DONE | DECODE_FINISHED;
  }

  return true;
}

/**** PUdpCloseItemContainer ****/

PUdpCloseItemContainer::PUdpCloseItemContainer( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x27";
}

PUdpMsgAnalyser* PUdpCloseItemContainer::Analyse()
{
  mDecodeData->mName << "=Closing item container";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpCloseItemContainer::DoAction()
{
//  PClient* nClient = mDecodeData->mClient;
//  PChar* nChar = nClient->GetChar();

  PChar* nChar = mDecodeData->mClient->GetChar();
  PContainer* tContainer = nChar->GetContainerInExclusiveUse();
  if ( tContainer )
  {
    nChar->SetContainerInExclusiveUse( NULL );
    tContainer->EndUse( nChar->GetID() );
    if ( ! tContainer->GetOwnerId() )
    {
      delete tContainer;
      if ( gDevDebug ) Console->Print( "%s Temporary container deleted", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    }
  }

  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;
}
