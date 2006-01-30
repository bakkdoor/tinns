#include "tinns.h"

static const int RCON_INPUTLEN = 512;

struct PRConClient
{
	enum
	{
		RCCS_AUTH,
		RCCS_AUTH_USER,
		RCCS_AUTH_PASS,
		RCCS_VALID,
		RCCS_DISCONNECT
	} mState;
	
	PSocket *mSocket;
	PAccount *mAccount;
	char mInput[RCON_INPUTLEN];
	int mInputLen;
	bool mEcho;
	std::clock_t mSleep, mSleepStart;
	int mNumFailures;
			
	inline PRConClient(NLsocket &Sock)
	{
		mSocket = new PSocket(Sock);
		mAccount = 0;
		mInputLen = 0;
		mEcho = true;
		mSleep = 0;
		mSleepStart = 0;
		mNumFailures = 0;
	}
	
	inline ~PRConClient()
	{
		delete mSocket;
	}
	
	inline void Print(const char *Fmt, ...)
	{
		static char Str[256];
		va_list args;
		va_start(args, Fmt);
		vsnprintf(Str, 255, Fmt, args);
		va_end(args);
		mSocket->Write(Str);
	}
	
};

PRConsole::PRConsole()
{
	mListener = NL_INVALID;
}

PRConsole::~PRConsole()
{
	if(mListener != NL_INVALID)
		nlClose(mListener);
	
	for(ClientList::iterator i=mClients.begin(); i!=mClients.end(); i++)
		delete *i;
}

void PRConsole::Start()
{
	Console->Print("Starting remote console...");
	u16 Port = Config->GetOptionInt("rconsole port");
	mListener = nlOpen(Port, NL_TCP);
	if(mListener == NL_INVALID)
	{
		Console->Print("Remote console failed");
		return;
	}
	
	nlListen(mListener);
}

void PRConsole::Update()
{
	if(mListener==NL_INVALID)
		return;

	NLsocket temp = nlAcceptConnection(mListener);		
	if(temp != NL_INVALID)
	{
		// TODO: print client ip addr
		Console->Print("RConsole: client connected");
		PRConClient *cl = new PRConClient(temp);
		cl->mState = PRConClient::RCCS_AUTH;
		mClients.push_back(cl);		
		cl->Print("TinNS shell [%s]\r\n", TINNS_VERSION);
	}	

	for(ClientList::iterator i=mClients.begin(); i!=mClients.end();)
	{
		ClientList::iterator j=i;
		PRConClient *cl = *i;
		++i;
		if(!cl->mSocket->Update() || cl->mSocket->TimeOut())
		{
			Console->Print("RConsole: client disconnected");
			mClients.erase(j);
			delete cl;
			continue;
		}

		if(cl->mSleep > 0)
		{
			std::clock_t t = std::clock();
			cl->mSleep -= (t-cl->mSleepStart);
			cl->mSleepStart = t;
			
			if(cl->mSleep < 0)
				cl->mSleep = 0;
				
			// flush socket while sleeping
			int Size=0;
			cl->mSocket->Read(&Size);
			
			continue;
		}
		
		if(cl->mState==PRConClient::RCCS_AUTH)
		{
			cl->Print("\r\nlogin: ");
			cl->mState = PRConClient::RCCS_AUTH_USER;
		}
		
		const u8 *Buf = 0;
		int Size=0;
		if((bool)(Buf = cl->mSocket->Read(&Size)))
		{
			for(int i=0; i<Size; i++)
				if(cl->mInputLen < RCON_INPUTLEN)
				{
					switch(Buf[i])
					{
						case 0x08 :
						{
							if(cl->mInputLen > 0)
							{
								if(cl->mEcho)
									cl->mSocket->Write(Buf[i]);
								cl->mInput[cl->mInputLen]=0;
								--cl->mInputLen;
							}
							break;
						}
						
						case '\n' :
						{
							if(cl->mEcho)
								cl->mSocket->Write(Buf[i]);
							cl->mInput[cl->mInputLen]=0;
							ProcessClient(cl);
							cl->mInputLen = 0;
							break;
						}
						
						default :
						{
							cl->mInput[cl->mInputLen++]=Buf[i];
							if(cl->mEcho)
								cl->mSocket->Write(Buf[i]);
							break;
						}
					}
				}
		}
	}
}

void PRConsole::ProcessClient(PRConClient* Client)
{
	Console->Print(">%s", Client->mInput);
	if(Client->mInputLen > 1)
	{
		if(Client->mInput[Client->mInputLen-1] == '\r')
			Client->mInput[Client->mInputLen-1]=0;
			
		if(Client->mState == PRConClient::RCCS_AUTH_USER)
		{
			Client->mAccount = Database->GetAccount(Client->mInput);
			Client->mState = PRConClient::RCCS_AUTH_PASS;
			Client->Print("password: ");
			Client->mEcho = false;
		} else
		if(Client->mState == PRConClient::RCCS_AUTH_PASS)
		{
			Client->mEcho = true;
			if((!Client->mAccount) || (Client->mAccount->GetPassword() != Client->mInput) || (!Client->mAccount->IsConsoleAllowed()))
			{
				Client->Print("Invalid user or password\r\n");
				Client->mSleepStart = std::clock();
				++Client->mNumFailures;
				if(Client->mNumFailures >= 3)
				{
					// sleep 1 minute 
					Client->mSleep = 60*CLOCKS_PER_SEC;
					Client->mNumFailures = 0;
				} else
					Client->mSleep = 5*CLOCKS_PER_SEC;

				Client->mState = PRConClient::RCCS_AUTH;
			} else
			{
				Client->Print("\r\n\nUser %s logged in\r\n", Client->mAccount->GetName().c_str());
				Client->mState = PRConClient::RCCS_VALID;
				// disconnect after 30 minutes lacking socket activity
				Client->mSocket->SetTimeOutValue(1800);
				Prompt(Client);
			}
		}
	}
}

void PRConsole::Prompt(PRConClient *Client)
{
	Client->Print("$ ", Client->mAccount->GetName().c_str(), Config->GetOption("server name").c_str());
}
