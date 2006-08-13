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
	gameserver.h

	Authors:
	- v00d00
	- Akiko

	MODIFIED: 30 Nov 2005 Akiko
	REASON: - added GPL
*/

#ifndef GAMESERVER_H
#define GAMESERVER_H

class PGameServer
{
	private :
		int mNumClients;
		u32 mBaseGameTime;
    struct timespec mStartTime;
    
		typedef std::map<PClient*, struct PGameState*> GameStateMap;
		GameStateMap ClientStates;

	protected :
		bool ProcessClient(PClient *Client, PGameState *State=0);
		void FinalizeClient(PClient *Client, PGameState *State);
		void FinalizeClientDelayed(PClient *Client, PGameState *State);

//		bool HandleHandshake(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleHandshake(PGameState *State, const u8 *Packet, int PacketSize);

		bool HandleAuthenticate(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
//		bool HandleGameData(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGameData(PClient *Client, PGameState *State, const u8 *Packet);

//		bool HandleRequestChars(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleRequestChars(PClient *Client, PGameState *State, const u8 *Packet);

		bool HandleCharList(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
//		bool HandleGetStatus(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGetStatus(PClient *Client, PGameState *State, const u8 *Packet);

//		bool HandleGameInfo(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGameInfo(PClient *Client, PGameState *State, const u8 *Packet);

		bool HandleGame(PClient *Client, PGameState *State);

		bool HandleUDPType13(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
//		bool HandleUDPType03(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
        bool HandleUDPType03(PGameState *State);

		void SendBaseLine(PClient *Client, PGameState *State); // reactivated State param for state changing at the right time (for futur use mostly)
//		void SendBaseLine(PClient *Client); 

		void SendUDPType13(PClient *Client, PGameState *State, u8 *Packet, int PacketSize);

	public :
		PGameServer();
		~PGameServer();

		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
		void UDPStreamClosed(PClient *Client);
		void SetGameTime(u32 newtime);
		u32 GetGameTime();
		
/**** Packet building & sending function put here in wait for a better place ****/
//Does NOT includes UDP_ID increment when needed, no UDP_ID / SessionID setting
// as these must be set on a by message destination basis

PMessage* BuildCharHelloMsg(PClient* Client); 
PMessage* BuildCharClanInfoMsg (PClient* nClient, u16 nReqType, u32 nCharId);
PMessage* BuildCharHealthUpdateMsg (PClient* nClient);
PMessage* BuildCharPosUpdateMsg (PClient* nClient);
PMessage* BuildCharSittingMsg (PClient* nClient, u16 nState);

};

void ChangeLocation(int ClientNum);

#endif

