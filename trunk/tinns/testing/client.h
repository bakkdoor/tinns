#ifndef CLIENT_H
#define CLIENT_H

#ifdef _MSC_VER
	#pragma once
#endif

enum PClientConnection
{
	PCC_NONE = 0,
	PCC_PATCH = 1,
	PCC_INFO = 2,
	PCC_GAME = 4
};

enum PClientLevel
{
	PCL_UNKNOWN = 0,
	PCL_PLAYER,
	PCL_GAMEMASTER,
	PCL_ADMIN
};

class PClient
{
	private :
		NLaddress mAddress;
		char mAddrString[24];
		PAccount *mAccount;
		
		PGameSocket *mPatchSocket;
		PGameSocket *mInfoSocket;
		PGameSocket *mGameSocket;
		PSocket *mUDPSocket;
		NLaddress mLocalUDPAddr;
		NLaddress mRemoteUDPAddr;
		int mIndex;
		u32 mCharID;
		PClientLevel mLevel;
		int mConnection;
	protected :
	public :
		PClient(int Index);
		~PClient();
		
		inline int GetIndex() const { return mIndex; }
		inline u32 GetCharID() const { return mCharID; }

		inline void SetCharID(int id) { mCharID=id; }//NEW added

		inline PClientLevel GetLevel() const { return mLevel; }
		inline PGameSocket *GetPatchSocket() const { return mPatchSocket; }
		inline PGameSocket *GetInfoSocket() const { return mInfoSocket; }
		inline PGameSocket *GetGameSocket() const { return mGameSocket; }
		inline PSocket *GetUDPSocket() { return mUDPSocket; }
		inline int GetConnection() const { return mConnection; }
		inline const char *GetAddress() const { return mAddrString; }
		inline PAccount *GetAccount() const { return mAccount; }
		
		bool PatchConnect(const NLsocket &Socket);
		void PatchDisconnect();
		
		bool InfoConnect(const NLsocket &Socket);
		void InfoDisconnect();
		
		bool GameConnect(const NLsocket &Socket);
		void GameDisconnect();
		void SetRemoteUDPAddr(u32 Addr, u16 Port);
		u16 OpenUDP();

		void LoggedIn(PAccount *Account);
		void Update();
};

#endif

