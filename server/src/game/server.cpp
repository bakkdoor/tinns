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
        server.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
*/
#include "main.h"

PServer::PServer()
{
	mNumClients = 0;
	mMaxClients = Config->GetOptionInt("maxclients");
	mGMSlots = Config->GetOptionInt("gm_slots");
	if(mMaxClients==0)
		mMaxClients=1;
	if(mGMSlots==0)
		mGMSlots=1;
	Console->Print("Max clients: %i / GM slots: %i", mMaxClients, mGMSlots);
	mClients.reserve(mMaxClients + mGMSlots);
	for(int i=0; i<mMaxClients+mGMSlots; i++)
		mClients[i]=0;
}

PServer::~PServer()
{
	for(int i=0; i<mMaxClients+mGMSlots; i++)
		delete mClients[i];
}

int PServer::NewClient()
{
	if(mNumClients==mMaxClients+mGMSlots)
		return -1;

	for(int i=0; i<mMaxClients+mGMSlots; i++)
	{
		if(!mClients[i])
		{
			mClients[i]=new PClient(i);
			++mNumClients;
			return i;
		}
	}
	return -1;
}

PClient *PServer::GetClient(int Client) const
{
	if(Client < 0 || Client >= mMaxClients+mGMSlots)
		return 0;

	return mClients[Client];
}

void PServer::Update()
{
	for(int i=0; i<mMaxClients+mGMSlots; i++)
	{
		if(mClients[i])
		{
			mClients[i]->Update();
			if(mClients[i]->GetConnection()==PCC_NONE && mClients[i]->getTCPConn() == 0)
			{
				Console->Print("Removing client %i...", i);
				delete mClients[i];
				mClients[i]=0;
				--mNumClients;
			}
		}
	}
}

void PServer::Shutdown()
{
    Console->Print("======================");
	Console->Print("Shutting down Gameserver...");
	for(int i=0; i<mMaxClients+mGMSlots; i++)
	{
		if(mClients[i])
		{
			delete mClients[i];
			mClients[i]=0;
		}
	}
}

//SELECT `s_name` , NOW( ) , `s_lastupdate` , NOW( ) - `s_lastupdate` FROM `server_list` 
