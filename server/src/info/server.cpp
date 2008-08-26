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

	MODIFIED: 27 Aug 2005 Hammag
	REASON: - Use mMaxClient member	instead of MAX_INFO_CLIENTS define

*/

#include "main.h"

PServer::PServer()
{
  mMaxClients = Config->GetOptionInt("maxclients");
  
	mClients.reserve(mMaxClients);
	mNumClients = 0;
	for(int i=0; i<mMaxClients; i++)
		mClients[i]=0;
}

PServer::~PServer()
{
	for(int i=0; i<mMaxClients; i++)
		delete mClients[i];
}

int PServer::NewClient()
{
	if(mNumClients==mMaxClients)
		return -1;

	for(int i=0; i<mMaxClients; i++)
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
	if(Client < 0 || Client >= mMaxClients)
		return 0;

	return mClients[Client];
}

void PServer::Update()
{
	for(int i=0; i<mMaxClients; i++)
	{
		if(mClients[i])
		{
			mClients[i]->Update();
			if(mClients[i]->GetConnection()==PCC_NONE && mClients[i]->getTCPConn() == 0)
			{
				Console->Print("Removing client ...");
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
	Console->Print("Shutting down Infoserver...");
	for(int i=0; i<mMaxClients; i++)
	{
		if(mClients[i])
		{
			delete mClients[i];
			mClients[i]=0;
		}
	}
}
