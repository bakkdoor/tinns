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
 clientmanager.h

 Authors:
 - bakkdoor

 MODIFIED: 13 Dec 2005 bakkdoor
 REASON: - introduced
 MODIFIED: 29 Jul 2006 Hammag
 REASON: - added UDP broadcast fonction
         - added "zone players say Hello" fonction
         - changed type ClientMap to PClientMap, changed members name prefix m_ to m

 MODIFIED: 12 Aug 2006 Hammag
 REASON: - duplicated UDPBroadcast() in two prototypes

*/

#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

typedef std::map<int, PClient*> PClientMap;

class PClientManager
{
  private:
    //int       mLastID;
    PClientMap mClientList;

  public:
    PClientManager();
    ~PClientManager();

    PClientMap::iterator   getClientListBegin() { return mClientList.begin(); }
    PClientMap::iterator   getClientListEnd() { return mClientList.end(); }

    bool addClientToList( PClient* newClient );
    //void deleteClientFromListByID(int id);
    void deleteClientFromList( int id );
    //bool deleteClientFromList(PClient* delClient); // maybe no use for this...
    PClient* getClientByID( int id ) const; // returns pointer to a client for further use
    PClient* getClientByChar( u32 CharID ) const;
    PClient* getClientByChar( const std::string &Name ) const;
    // int getClientID(PClient* _client); do _client->GetLocalID()
    bool IsWorldInUse( u16 nWorldID ) const;

    // each function return the number of messages sent.
    int UDPBroadcast( PMessage* nMessage, u32 nZoneID, u16 nX = 0, u16 nY = 0, u16 nZ = 0, u16 nMaxDist = 0, int nSkipSource = -1 );
    int UDPBroadcast( PMessage* nMessage, PClient* nClient, u16 nMaxDist = 0, bool nSkipSource = false, bool nNPCPing = false );
    int SendUDPZoneWelcomeToClient( PClient* nClient );

};

#endif

