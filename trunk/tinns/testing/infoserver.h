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

		bool HandleHandshake(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
		bool HandleAuthenticate(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
		bool HandleServerList(PClient *Client, PInfoState *State, const u8 *Packet, int PacketSize);
	public :
		PInfoServer();
		~PInfoServer();
		
		void Start();
		void Update();
		void ClientDisconnected(PClient *Client);
};

#endif

