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

struct PGameState
{
	struct TCP
	{
		enum State
		{
			GS_UNKNOWN,
			GS_CONNECTED,
			GS_HANDSHAKE0,
			GS_AUTHENTICATE,
			GS_GAMEDATA,
			GS_REQUESTCHARS,
			GS_CHARLIST,
			GS_GETSTATUS,
			GS_GAMEINFO,
			GS_INGAME
		} mState;

		bool mWaitSend;	// wait-for-completition flag
	} TCP;

	struct UDP
	{
		enum State
		{
			GUS_UNKNOWN,
			GUS_SYNC0,
			GUS_SYNC1,
			GUS_SYNC2,
			GUS_SYNC3
		} mState;

		bool mSynced;
		u16 mServerPacketNum;
		u16 mSequence;
		//u16 mClientPacketNum;
	} UDP;

	PGameState()
	{
		TCP.mState = TCP::GS_UNKNOWN;
		TCP.mWaitSend = false;

		UDP.mState = UDP::GUS_UNKNOWN;
		UDP.mSynced = false;
		UDP.mServerPacketNum = 0x9c9f;
		UDP.mSequence = 2;
		//UDP.mClientPacketNum = 0;
	};
};

class PUdpMsgDecoder;

class PGameServer
{
	private :
        std::time_t mServerStartupTime;
		int mNumClients;
		u32 mBaseGameTime;
    struct timespec mStartTime;
    u32 mInternalRand;

		typedef std::map<PClient*, struct PGameState*> GameStateMap;
		GameStateMap ClientStates;
		PUdpMsgDecoder* MsgDecoder;

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

	public :
		PGameServer();
		~PGameServer();

		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
		void UDPStreamClosed(PClient *Client);
		void SetGameTime(u32 newtime);
		u32 GetGameTime();

		inline std::time_t GetStartTime() const { return mServerStartupTime; };

		u16 GetRandom(u16 MaxVal, u16 MinVal = 0); // u16 value between MinVal and MaxVal (inclusive) with max range 32768
		f32 GetRandomFloat(); // f32 value between 0 and 1
};

#endif

