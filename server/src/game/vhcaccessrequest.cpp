/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  vhcaccessrequest.cpp - classe for pendinf vhc access requests
               

 CREATION: 14 Apr 2009 Hammag

*/

#include "main.h"
#include "vhcaccessrequest.h"

PVhcAccessRequest::PVhcAccessRequest()
{
	mVhcGlobalId = mCharId = 0;
	mStatus = 0;
	mTimestamp = 0;
}

PVhcAccessRequest::PVhcAccessRequest(u32 nRequesterCharId, u32 nVhcGlobalId)
{
	mCharId = nRequesterCharId;
	mVhcGlobalId = nVhcGlobalId;
	mStatus = 0;
	mTimestamp = std::time( NULL );
}

/*
typedef std::map<u32, PVhcAccessRequest> PVhcAccessRequestMap;

PVhcAccessRequestMap mActiveRequests;
u32 mNextRequestId;

std::time_t mResponseWaitTime; // How long do we wait for owner answer
std::time_t mCheckWaitTime; // How long do we wait for user access check (0: remaining time from mResponseWaitTime)
std::time_t mReuseWaitTime; // How long do we allow user to re-use the autorization after first check
*/
void PVhcAccessRequestList::DropTimedOut()
{
	std::time_t now = std::time( NULL );
	std::time_t timeout;
	
	for(PVhcAccessRequestMap::iterator it=mActiveRequests.begin(); it!=mActiveRequests.end(); it++)
	{
		switch(it->second.mStatus)
		{
			case 0:
				timeout = mResponseWaitTime;
				break;
			case 1:
				timeout = ( mCheckWaitTime ? mCheckWaitTime : mResponseWaitTime );
				break;
			case 2:
				timeout = mReuseWaitTime;
				break;
			default:
				timeout = 0;
				break;
		}
		if( it->second.mTimestamp > (now + timeout) )
		{
			mActiveRequests.erase(it);
		}
	}
}

PVhcAccessRequestList::PVhcAccessRequestList()
{
	mNextRequestId = 1;
	SetParameters( 10, 20, 0 );
}

PVhcAccessRequestList::~PVhcAccessRequestList()
{
}

void PVhcAccessRequestList::SetParameters(std::time_t nResponseWaitTime, std::time_t nCheckWaitTime, std::time_t nReuseWaitTime)
{
	mResponseWaitTime = nResponseWaitTime;
	mCheckWaitTime = nCheckWaitTime;
	mReuseWaitTime = nReuseWaitTime;
}

u32 PVhcAccessRequestList::Add(u32 nRequesterCharId, u32 nVhcGlobalId)
{
	DropTimedOut();
	// We could check for existing entries before
	PVhcAccessRequest newReq(nRequesterCharId, nVhcGlobalId);
	u32 newId = mNextRequestId;
	mActiveRequests[newId] = newReq;
	
	if(++mNextRequestId == 0)
		mNextRequestId = 1;
	
	return (newId);
}

bool PVhcAccessRequestList::RegisterResponse(u32 nRequestId, bool nStatus)
{
	DropTimedOut();
	PVhcAccessRequestMap::iterator it = mActiveRequests.find(nRequestId);
	if(it != mActiveRequests.end())
	{
		if( nStatus )
		{
			it->second.mStatus = 1;
			if( mCheckWaitTime )
			{
				it->second.mTimestamp = std::time( NULL );
			}
		}
		else
		{
			mActiveRequests.erase(it);
		}
		return true;
	}
	else
		return false;
}

bool PVhcAccessRequestList::Check(u32 nRequestId, u32 nRequesterCharId, u32 nVhcGlobalId)
{
	DropTimedOut();
	PVhcAccessRequestMap::iterator it = mActiveRequests.find(nRequestId);
	if(it != mActiveRequests.end())
	{
		if( (it->second.mCharId == nRequesterCharId) && (it->second.mVhcGlobalId == nVhcGlobalId) && (it->second.mStatus > 0) )
		{
			if( mReuseWaitTime && (it->second.mStatus == 1) )
			{
				it->second.mStatus = 2;
				it->second.mTimestamp = std::time( NULL );
			}
			else
			{
				mActiveRequests.erase(it);
			}
			
			return true;
		}
	}
	return false;
}
