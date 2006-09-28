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

#include "main.h"

PClient::PClient(int Index)
{
	mAccount = 0;
	mIndex = Index;
	mCharID = 0;
	mLevel = PCL_BANNED;
	mConnection = PCC_NONE;
	mUDP_ID = 0;
	mSessionID = 37917;
	mRemotePort = 0;
	m_TCPConnection = NULL;
	m_UDPConnection = NULL;
}

PClient::~PClient()
{
    if(m_TCPConnection)
    {
        delete m_TCPConnection;
        m_TCPConnection = NULL;
    }
    if(m_UDPConnection)
    {
        delete m_UDPConnection;
        m_UDPConnection = NULL;
    }
}

/*void PClient::IncreaseUDP_ID()
{
  if (++mUDP_ID > 65534)
  {
    mUDP_ID = 0; // is 65533 the higher valid value ???
  }
	//mSessionID = 37917 + mUDP_ID;
  if (++mSessionID > 65534)
  {
    mSessionID = 0; // is 65533 the higher valid value ???
  }
}*/
	
void PClient::GameDisconnect()
{
	mAccount = 0;

	if(m_TCPConnection)
	{
	    delete m_TCPConnection;
      m_TCPConnection = NULL;
	}
	
	if(m_UDPConnection)
	{
	    delete m_UDPConnection;
	    m_UDPConnection = NULL;
	}
	
  /**** Will be better to put that char-saving-at-disconnect in Char destructor, when only used Chars will be loaded ****/
  PChar *tChar = Database->GetChar(mCharID);
  if (tChar)
  {
    if (tChar->IsDirty())
    {
      bool res = tChar->SQLSave();
      if (res)
        Console->Print(GREEN, BLACK, "GameDisconnect: Char %i (Client %i) saved before disconnect.", tChar->GetID(), mIndex);
      else
        Console->Print(RED, BLACK, "GameDisconnect: Char %i (Client %i) saved before disconnect FAILED.", tChar->GetID(), mIndex);
    }
    else
    {
      Console->Print(GREEN, BLACK, "GameDisconnect: Char %i (Client %i) no save needed.", tChar->GetID(), mIndex);
      if (!tChar->IsOnline())
        Console->Print(GREEN, BLACK, "GameDisconnect: Char %i (Client %i) wasn't marked as ingame anyway...", tChar->GetID(), mIndex);
    }
  }
  else
  {
    //Console->Print(YELLOW, BLACK, "GameDisconnect: Client %i had no char online.", mIndex);
  }
  /**********************************/
    
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

PChar* PClient::GetChar() const 
{
  return Database->GetChar(mCharID);
}
