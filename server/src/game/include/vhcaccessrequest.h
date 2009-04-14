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
  vhcaccessrequest.h - classe for pendinf vhc access requests
               

 CREATION: 14 Apr 2009 Hammag

*/

#ifndef VHCACCESSREQ_H
#define VHCACCESSREQ_H

class PVhcAccessRequest
{
	public:
		//u32 mRequestId;
		std::time_t mTimestamp;
		u32 mCharId;
		u32 mVhcGlobalId;
		u8 mStatus; // 0: no answer yet, 1: access allowed (refused req are dropped) 2: result checked

		PVhcAccessRequest();
		PVhcAccessRequest(u32 nRequesterCharId, u32 nVhcGlobalId);
};

class PVhcAccessRequestList
{
	typedef std::map<u32, PVhcAccessRequest> PVhcAccessRequestMap; 

	private:
		PVhcAccessRequestMap mActiveRequests;
		u32 mNextRequestId;

		std::time_t mResponseWaitTime; // How long do we wait for owner answer
		std::time_t mCheckWaitTime; // How long do we wait for user access check (0: remaining time from mResponseWaitTime)
		std::time_t mReuseWaitTime; // How long do we allow user to re-use the autorization after first check

		void DropTimedOut();

	public:
		PVhcAccessRequestList() ;
		~PVhcAccessRequestList();

		void SetParameters(std::time_t nResponseWaitTime, std::time_t nCheckWaitTime, std::time_t nReuseWaitTime);
		
		u32 Add(u32 nRequesterCharId, u32 nVhcGlobalId);
		bool RegisterResponse(u32 nRequestId, bool nStatus);
		bool Check(u32 nRequestId, u32 nRequesterCharId, u32 nVhcGlobalId);
};

#endif
