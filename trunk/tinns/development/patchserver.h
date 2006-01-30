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
	patchserver.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef PATCHSERVER_H
#define PATCHSERVER_H

#ifdef _MSC_VER
	#pragma once
#endif

class PPatchServer
{
	private :
		NLsocket mListener;
		int mNumClients;
		int mNumFileTransfers;
		typedef std::map<PClient*, struct PPatchState*> PatchStateMap;
		PatchStateMap ClientStates;
	protected :
		bool ProcessClient(PClient *Client, PPatchState *State=0);
//		u32 StartPatch(PClient *Client, PPatchState *State);
		u32 StartPatch(PPatchState *State);
		bool SendPatchData(PClient *Client, PPatchState *State) const;
//		u32 StartFile(PClient *Client, PPatchState *State);
		u32 StartFile(PPatchState *State);
		bool SendFileData(PClient *Client, PPatchState *State) const;
		void FinalizeClient(PClient *Client, PPatchState *State);
		void FinalizeClientDelayed(PClient *Client, PPatchState *State);

		bool HandleHandshake(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize);
		bool HandleVersionRequest(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize);
		bool HandleFileRequests(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize);
	public :
		PPatchServer();
		~PPatchServer();

		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
};

#endif

