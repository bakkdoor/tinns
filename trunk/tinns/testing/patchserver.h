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
		u32 StartPatch(PClient *Client, PPatchState *State);
		bool SendPatchData(PClient *Client, PPatchState *State) const;
		u32 StartFile(PClient *Client, PPatchState *State);
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

