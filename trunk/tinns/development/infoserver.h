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
	infoserver.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef INFOSERVER_H
#define INFOSERVER_H

#ifdef _MSC_VER
	#pragma once
#endif

class PInfoServer
{
	private :
		NLsocket mListener;
		int mNumClients;
		typedef std::map<PClient*, struct PInfoState*> InfoStateMap;
		InfoStateMap ClientStates;
	protected :
		bool ProcessClient(PClient *Client, PInfoState *State=NULL);
		void FinalizeClient(PClient *Client, PInfoState *State);
		void FinalizeClientDelayed(PClient *Client, PInfoState *State);

//		bool HandleHandshake(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
		bool HandleHandshake(PInfoState *State, const u8 *Packet, int PacketSize);
		bool HandleAuthenticate(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
//		bool HandleServerList(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
		bool HandleServerList(PClient *Client, const u8 *Packet, int PacketSize);
	public :
		PInfoServer();
		~PInfoServer();

		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
};

#endif

