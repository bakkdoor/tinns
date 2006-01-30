#ifndef SERVER_H
#define SERVER_H

class PServer
{
	private :
		int mMaxClients;
		int mGMSlots;
		int mNumClients;
		std::vector<PClient*> mClients;
	protected :
	public :
		PServer();
		~PServer();
		
		inline int GetMaxClients() const { return mMaxClients; }
		inline int GetGMSlots() const { return mGMSlots; }
		inline int GetNumClients() const { return mNumClients; }
		int NewClient();
		PClient *GetClient(int Client) const;
		void Update();
		void Shutdown();
};

#endif

