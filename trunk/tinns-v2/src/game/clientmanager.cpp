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
*/

#include "tinns.h"

PClientManager::PClientManager()
{
    m_LastID = 0;
}

PClientManager::~PClientManager()
{
    for(ClientMap::iterator it=m_ClientList.begin(); it!=m_ClientList.end(); it++)
    {
        delete it->second;
    }
}

bool PClientManager::addClientToList(PClient* newClient)
{
	if(newClient != getClientByID(newClient->GetIndex())) // only if client isn't in list already!
	{
	    m_ClientList.insert(std::make_pair(m_LastID, newClient));
        ++m_LastID;

        if(newClient)
        {
            return true;
        }
	}

    return false;
}

void PClientManager::deleteClientFromListByID(int id)
{
    ClientMap::iterator it = m_ClientList.find(id);

	/*
    if(it->second)
    {
        delete it->second;
    }
	*/

    m_ClientList.erase(it);
}

bool PClientManager::deleteClientFromList(PClient* delClient)
{
    for(ClientMap::iterator it=m_ClientList.begin(); it!=m_ClientList.end(); it++)
    {
        if(delClient == it->second)
        {
			/*
            if(it->second)
            {
                delete delClient;
			*/
                m_ClientList.erase( it );
                return true;
			/*
            }
			*/
        }
    }

    return false;
}

PClient* PClientManager::getClientByID(int id)
{
    ClientMap::iterator it = m_ClientList.find(id);

	// if pointer is valid (client existing), then return it's adress
	if(it->second)
	{
		return it->second;
	}

	return NULL;
}

int PClientManager::getClientID(PClient* _client)
{
    for(ClientMap::iterator it=m_ClientList.begin(); it!=m_ClientList.end(); it++)
    {
        if(_client == it->second)
        {
            return it->first;
        }
    }

    return -1;
}
