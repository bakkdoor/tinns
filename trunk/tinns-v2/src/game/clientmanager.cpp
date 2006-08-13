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
	
  TODO:   - these are just begining of modif, as the Client Manager is bound to be a major component for multiplayer management	            
*/

#include "main.h"


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

bool PClientManager::addClientToList(PClient* newClient)
{
  if (!newClient)
    return false;
    
  PClientMap::const_iterator it = mClientList.find(newClient->GetLocalID());
	if(it == mClientList.end()) // only if client not found in list
	{
/*	    mClientList.insert(std::make_pair(m_LastID, newClient));
        ++m_LastID; */

	    mClientList.insert(std::make_pair(newClient->GetLocalID(), newClient));
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

void PClientManager::deleteClientFromList(int id)
{
    PClientMap::iterator it = mClientList.find(id);
    if (it != mClientList.end())
    {
      mClientList.erase(it);
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

PClient* PClientManager::getClientByID(int id)
{
	PClientMap::const_iterator it = mClientList.find(id);
	return ( (it != mClientList.end()) ? (PClient*)(it->second) : NULL );
}

/* int PClientManager::getClientID(PClient* _client)
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
int PClientManager::UDPBroadcast(PMessage* nMessage, u32 nZoneID, u16 nX, u16 nY, u16 nZ, u16 nMaxDist)
{
  int msgCount = 0;
  PChar* nChar;
  PMessage* tmpMsg;
  PClient* itClient;
  u16 DX, DY, Dapprox;
  
  nZ=nZ; // avaoid warning

//float fDX, fDY, fDist; // for dist approx debug

  for(PClientMap::iterator it=mClientList.begin(); it != mClientList.end(); it++)
  {
      itClient = (PClient*)(it->second);
      if (itClient->getUDPConn())
      {
        nChar = (Database->GetChar(itClient->GetCharID()));
        if (nChar->GetLocation() != nZoneID) // if limited to zone, do check
          continue;

        if (nMaxDist) // if limited to distance, do check
        {
          DX = ((nChar->Coords).mX > nX) ? (nChar->Coords).mX - nX : nX - (nChar->Coords).mX;
          DY = ((nChar->Coords).mY > nY) ? (nChar->Coords).mY - nY : nY - (nChar->Coords).mY;
          // We use Dapprox = max(dx, dy) + K * min(dx, dy)
          // Dapprox = (DX > DY) ? DX + (DY >> 1) : DY + (DX >> 1); // Fastest, but max error on dist is around 10% real dist
          Dapprox = (u16)((DX > DY) ? DX + 0.33*DY : DY + 0.33*DX); // max error on dist is around 5%, which should be enough
/*
fDX=((nChar->Coords).mX - nX);
fDY=((nChar->Coords).mY - nY);
fDist=sqrt(fDX*fDX + fDY*fDY);
if (fDist != 0) Console->Print("Dist: %f\tApprox: %d\tError: %d (%d%)", fDist, Dapprox, (int)(Dapprox-fDist), (int)(100*(Dapprox-fDist)/fDist));
*/
          if (Dapprox >  nMaxDist)
            continue;
        }

        tmpMsg = new PMessage(nMessage->GetMaxSize());
        (*tmpMsg) = (*nMessage);
         
        if ((tmpMsg->GetSize() > 9) && (tmpMsg->U8Data(0x00) == 0x13) && (tmpMsg->U8Data(0x06) == 0x03))
        {
          itClient->IncreaseUDP_ID();
          tmpMsg->U16Data(0x07) = itClient->GetUDP_ID();
        }
        tmpMsg->U16Data(0x01) = itClient->GetUDP_ID();
        tmpMsg->U16Data(0x03) = itClient->GetSessionID();
       
        itClient->getUDPConn()->SendMessage(tmpMsg);
        ++msgCount;
      }
  }

//Console->Print("Broadcast in zone %d to %d chars", nZoneID, msgCount); 
  delete nMessage;  
  return msgCount;  
}

int PClientManager::UDPBroadcast(PMessage* nMessage, PClient* nClient, u16 nMaxDist)
{
  PChar* nChar; 

  if (nClient && (nChar = Database->GetChar(nClient->GetCharID())))
  {
    return UDPBroadcast(nMessage, nChar->GetLocation(), (nChar->Coords).mX, (nChar->Coords).mY, (nChar->Coords).mZ, nMaxDist);
  }
  else
    return 0;
}

int PClientManager::SendUDPZoneWelcomeToClient(PClient* nClient)
{
  int msgCount = 0;
  PChar* nChar;
  PMessage* tmpMsg;
  u32 nZoneID;
  PClient* itClient;
    
  if (nClient && (nChar = Database->GetChar(nClient->GetCharID()))) // if nClient is set, always use its zone
  {
    nZoneID = nChar->GetLocation();
  }
  else
    return 0;
  
  for(PClientMap::iterator it=mClientList.begin(); it!=mClientList.end(); it++)
  {
    if (nClient->GetLocalID() == it->first)
      continue;
      
    itClient = (PClient*)(it->second);
    if (itClient->getUDPConn())
    {
      nChar = Database->GetChar(itClient->GetCharID());
      if (nChar->GetLocation() != nZoneID) // limit to zone
        continue;
        
      tmpMsg = GameServer->BuildCharHelloMsg(itClient);
       
      nClient->IncreaseUDP_ID();
      tmpMsg->U16Data(0x01) = nClient->GetUDP_ID();
      tmpMsg->U16Data(0x03) = nClient->GetSessionID();
      tmpMsg->U16Data(0x07) = nClient->GetUDP_ID();

//Console->Print("Welcome data sent from client %d to client %d", itClient->GetIndex(), nClient->GetIndex());
//tmpMsg->Dump();             
      nClient->getUDPConn()->SendMessage(tmpMsg);
      ++msgCount;
    }
  }
  
  return msgCount;        
}
