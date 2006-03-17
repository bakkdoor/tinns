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
*/

#include "tinns.h"

PClient::PClient(int Index)
{
	mAccount = 0;
	mIndex = Index;
	mCharID = 0;
	mLevel = PCL_BANNED;
	mConnection = PCC_NONE;
	mUDP_ID = 0;
	mSessionID = 37917;
}

PClient::~PClient()
{
    if(m_TCPConnection)
    {
        delete m_TCPConnection;
    }
    if(m_UDPConnection)
    {
        delete m_UDPConnection;
    }
}

void PClient::GameDisconnect()
{
	mAccount = 0;

	if(m_TCPConnection)
	{
	    delete m_TCPConnection;
	}
	m_TCPConnection = 0;

	if(m_UDPConnection)
	{
	    delete m_UDPConnection;
	}
	m_UDPConnection = 0;

	//mConnection &= ~PCC_GAME;
	mConnection = PCC_NONE;
}

void PClient::LoggedIn(PAccount *Account)
{
	mAccount = Account;
}

void PClient::Update()
{
    if(m_TCPConnection)
    {
        if(m_TCPConnection->timeOut())
        {
            Console->Print("GameSocket: Client %i: timeout", mIndex);
            GameServer->ClientDisconnected(this);
        }
        else
        {
            if(!m_TCPConnection->update())
            {
                GameServer->ClientDisconnected(this);
            }
        }
    }

    if(m_UDPConnection)
    {
        if(m_UDPConnection->timeOut())
        {
            Console->Print("Game UDP: Client %i: timeout", mIndex);
            GameServer->UDPStreamClosed(this);
        }
        else
        {
            if(!m_UDPConnection->update())
			{
			    GameServer->UDPStreamClosed(this);
			}
        }
    }
}

