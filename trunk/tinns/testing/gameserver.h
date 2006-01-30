#ifndef GAMESERVER_H
#define GAMESERVER_H

#ifdef _MSC_VER
	#pragma once
#endif

class PGameServer
{
	private :
		NLsocket mListener;
		int mNumClients;
		
		typedef std::map<PClient*, struct PGameState*> GameStateMap;
		GameStateMap ClientStates;

	protected :
		bool ProcessClient(PClient *Client, PGameState *State=0);
		void FinalizeClient(PClient *Client, PGameState *State);
		void FinalizeClientDelayed(PClient *Client, PGameState *State);

		bool HandleHandshake(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleAuthenticate(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGameData(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleRequestChars(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleCharList(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGetStatus(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGameInfo(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleGame(PClient *Client, PGameState *State);
		
		bool HandleUDPType13(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		bool HandleUDPType03(PClient *Client, PGameState *State, const u8 *Packet, int PacketSize);
		
		void SendBaseLine(PClient *Client, PGameState *State);
		void SendUDPType13(PClient *Client, PGameState *State, u8 *Packet, int PacketSize);
	public :
		PGameServer();
		~PGameServer();

		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
		void UDPStreamClosed(PClient *Client);
};

#endif

