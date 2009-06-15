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
	- bakkdoor
	- Akiko

	MODIFIED: 13 Dec 2005 bakkdoor
	REASON: - introduced
	MODIFIED: 29 Jul 2006 Hammag
	REASON: - Added UDP broadcast fonction
		- Added "zone players say Hello" fonction
		- Modified the ID used as would should avoid muliplying the IDs for the same client.
		  Now using Client.GetLocalID() as the key (=Client.mIndex + 1) which is used in NC protocol
		- Modified getClientByID()
		- Removed getClientID(). Simply do Client->GetLocalID()
		- Modified deleteClientFromListByID() and renamed to deleteClientFromList()
		- Modified destructor as to not destroy stored clients, which is already done in PServer
		  (even if it could well be done here in near futur)

	MODIFIED: 12 Aug 2006 Hammag
	REASON: - implemented range filtering in UDPBroadcast()
		- implemented the two versions of UDPBroadcast()
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem
		- clean ups

	TODO:   - these are just begining of modif, as the Client Manager is bound to be a major component for multiplayer management
*/

#include "main.h"

#include "include/msgbuilder.h"

PClientManager::PClientManager()
{
  //mLastID = 0;
}


PClientManager::~PClientManager()
{
  /*
    for(PClientMap::iterator it=mClientList.begin(); it!=mClientList.end(); it++)
    {
        delete it->second;
    }
  */
}

bool PClientManager::addClientToList( PClient* newClient )
{
  if ( !newClient )
    return false;

  PClientMap::const_iterator it = mClientList.find( newClient->GetLocalID() );
  if ( it == mClientList.end() ) // only if client not found in list
  {
    /*     mClientList.insert(std::make_pair(m_LastID, newClient));
            ++m_LastID; */

    mClientList.insert( std::make_pair( newClient->GetLocalID(), newClient ) );
    //Console->Print(GREEN, BLACK, "Client %d added to clientmanager", newClient->GetIndex());
    /*        if(newClient)
            {
                return true;
            } */
    return true;
  }
  else
    return false;
}

// Check if a zone is in use
bool PClientManager::IsWorldInUse( u32 nWorldID ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second &&  it->second->GetChar() )
    {
      if ( it->second->GetChar()->GetLocation() == nWorldID )
        return true;
    }
  }
  return false;
}

// Check if a rawObjectId belongs to a char is in use
PClient* PClientManager::GetClientByCharLocalId( u32 rawObjectId, u32 nWorldID ) const
{
  PClient* nClient;
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    // note: atm, charid = clientid+1 in any zone (client.h)
    if ( (nClient = it->second) && ( nClient->GetLocalID() == rawObjectId ) )
    {
      if ( nClient->GetChar() && ( nClient->GetChar()->GetLocation() == nWorldID ) )
        return nClient;
    }
  }
  return 0;
}

void PClientManager::deleteClientFromList( u32 id )
{
  PClientMap::iterator it = mClientList.find( id );
  if ( it != mClientList.end() )
  {
    mClientList.erase( it );
    //Console->Print(YELLOW, BLACK, "Client %d removed from clientmanager", ((PClient*)(it->second))->GetIndex());
  }
}

/* bool PClientManager::deleteClientFromList(PClient* delClient)
{
    for(PClientMap::iterator it=mClientList.begin(); it!=mClientList.end(); it++)
    {
        if(delClient == it->second)
        {
      //     if(it->second)
      //    {
      //         delete delClient;
                mClientList.erase( it );
                return true;
      //    }
        }
    }
    return false;
} */

PClient* PClientManager::getClientByID( u32 id ) const
{
  PClientMap::const_iterator it = mClientList.find( id );
  return (( it != mClientList.end() ) ? ( PClient* )( it->second ) : NULL );
}

PClient* PClientManager::getClientByChar( u32 CharID ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second )
    {
      if ( it->second->GetCharID() == CharID )
        return it->second;
    }
  }
  return NULL;
}

PClient* PClientManager::getClientByChar( const std::string &Name ) const
{
  for ( PClientMap::const_iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( it->second )
    {
      if ( it->second->GetChar()->GetName() == Name )
        return it->second;
    }
  }
  return NULL;
}

/* u32 PClientManager::getClientID(PClient* _client)
{
    for(PClientMap::iterator it=mClientList.begin(); it!=mClientList<p align="center"></p>.end(); it++)
    {
        if(_client == it->second)
        {
            return it->first;
        }
    }

    return -1;
} */

// Distance checking doesn't care for Z axis ATM
int PClientManager::UDPBroadcast( PMessage* nMessage, u32 nZoneID, u16 nX, u16 nY, u16 nZ, u16 nMaxDist, u32 nSkipCharId, bool nNPCPing )
{
  int msgCount = 0;
  PChar* nChar;
  PMessage* tmpMsg;
  PClient* itClient;
  u16 Dapprox;

  for ( PClientMap::iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    itClient = ( PClient* )( it->second );

    // Dont send NPC alive messages when client is not ready for them
    if ( !itClient->IsAcceptingNPCUpdates() && nNPCPing )
      continue;

    if ( itClient->getUDPConn() )
    {
      nChar = itClient->GetChar();
      if ( nChar && ( nChar->GetLocation() != nZoneID ) )// if limited to zone, do check
        continue;

      if (itClient->GetCharID() == nSkipCharId ) // if source of broadcast should be skipped
        continue;

      if ( nMaxDist ) // if limited to distance, do check
      {
        Dapprox = DistanceApprox(( nChar->Coords ).mX, ( nChar->Coords ).mY, ( nChar->Coords ).mZ, nX, nY, nZ );
        if ( Dapprox >  nMaxDist )
          continue;
      }

      /*tmpMsg = new PMessage(nMessage->GetMaxSize());
      (*tmpMsg) = (*nMessage);*/
      tmpMsg = new PMessage( *nMessage );

      itClient->FillInUDP_ID( tmpMsg );
      itClient->SendUDPMessage( tmpMsg );
      ++msgCount;
    }
  }

  //Console->Print("Broadcast in zone %d to %d chars", nZoneID, msgCount);
  delete nMessage;
  return msgCount;
}

int PClientManager::UDPBroadcast( PMessage* nMessage, PClient* nClient, u16 nMaxDist, bool nSkipSource, bool nNPCPing )
{
  PChar* nChar;
  u32 skipCharId = nSkipSource ? nClient->GetCharID() : 0 ;

  if ( nClient && ( nChar = nClient->GetChar() ) )
  {
    return UDPBroadcast( nMessage, nChar->GetLocation(), ( nChar->Coords ).mX, ( nChar->Coords ).mY, ( nChar->Coords ).mZ, nMaxDist, skipCharId, nNPCPing );
  }
  else
  {
    delete nMessage;
    return 0;
  }
}

int PClientManager::SendUDPZoneWelcomeToClient( PClient* nClient )
{
  int msgCount = 0;
  PChar* nChar;
  PChar* itChar;
  PMessage* tmpMsg;
  u32 nZoneID;
  PClient* itClient;

  if ( nClient && ( nChar = nClient->GetChar() ) ) // if nClient is set, always use its zone
  {
    nZoneID = nChar->GetLocation();
  }
  else
    return 0;

  for ( PClientMap::iterator it = mClientList.begin(); it != mClientList.end(); it++ )
  {
    if ( nClient->GetLocalID() == it->first )
      continue;

    itClient = ( PClient* )( it->second );
    if ( itClient->getUDPConn() )
    {
      itChar = itClient->GetChar();
      if ( itChar->GetLocation() != nZoneID ) // limit to zone
        continue;

      tmpMsg = MsgBuilder->BuildCharHelloMsg( itClient );

      nClient->FillInUDP_ID( tmpMsg );
      nClient->SendUDPMessage( tmpMsg );
      //Console->Print("Welcome data sent from client %d to client %d", itClient->GetIndex(), nClient->GetIndex());
      //tmpMsg->Dump();

      if ( itChar->GetSeatInUse() != seat_none )
      {
        tmpMsg = MsgBuilder->BuildCharPosUpdateMsg( itClient );
        nClient->FillInUDP_ID( tmpMsg );
        nClient->SendUDPMessage( tmpMsg );

        //Console->Print("Sit on chair %d sent from client %d to client %d", (itChar->GetChairInUse()+1)*1024, itClient->GetIndex(), nClient->GetIndex());
        /*tmpMsg = MsgBuilder->BuildCharUseSeatMsg(itClient, (itChar->GetChairInUse()+1)*1024);
        nClient->FillInUDP_ID(tmpMsg);
        nClient->SendUDPMessage(tmpMsg);*/
      }
      ++msgCount;
    }
  }

  return msgCount;
}
