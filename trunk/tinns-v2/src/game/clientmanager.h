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
*/

#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

typedef std::map<int, PClient*> ClientMap;

class PClientManager
{
    private:
            int  	    m_LastID;
            ClientMap	m_ClientList;

    public:
            PClientManager();
            ~PClientManager();

            ClientMap::iterator   getClientListBegin() { return m_ClientList.begin(); }
            ClientMap::iterator   getClientListEnd() { return m_ClientList.end(); }

            bool 		addClientToList(PClient* newClient);
            void 		deleteClientFromListByID(int id);
            bool 		deleteClientFromList(PClient* delClient); // maybe no use for this...
            PClient* 	getClientByID(int id); // returns pointer to a client for further use
            int 	    getClientID(PClient* _client);
};

#endif

