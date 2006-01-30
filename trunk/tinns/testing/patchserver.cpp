#include "tinns.h"

struct PPatchState
{
	enum State
	{
		PS_UNKNOWN,
		PS_CONNECTED,
		PS_HANDSHAKE0,
		PS_HANDSHAKE1,
		PS_VERSIONREQUEST,
		PS_GETPATCHORFILE,
		PS_SENDPATCH,
		PS_SENDFILE
	} mState;
	
	u16 mSerial;
	
	u32 mCurrentPatch;
	u32 mPatchOffset;
	u32 mPatchSize;
	std::FILE *mPatchFile;
	
	std::string mCurrentFile;
	u32 mFileOffset;
	u32 mFileSize;
	PFile *mSendFile;
	
	bool mWaitSend;	// wait-for-completition flag
	PPatchState()
	{
		mState = PS_UNKNOWN;
		mSerial = 0;
		mCurrentPatch = 0;
		mPatchOffset = 0;
		mPatchFile = 0;
		
		mCurrentFile = "";
		mFileOffset = 0;
		mFileSize = 0;
		mSendFile = 0;
		
		mWaitSend = false;
	};
	
	~PPatchState()
	{
		if(mPatchFile)
			std::fclose(mPatchFile);
		if(mSendFile)
			Filesystem->Close(mSendFile);
	}
};

// -------------------------------------------------------

PPatchServer::PPatchServer()
{
	mListener = NL_INVALID;
	mNumClients = 0;
	mNumFileTransfers = 0;
}

PPatchServer::~PPatchServer()
{
	if(mListener != NL_INVALID)
	{
		nlClose(mListener);
		mListener=NL_INVALID;
	}
	
	for(PatchStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end(); i++)
		delete i->second;
}

void PPatchServer::Start()
{
	u16 Port = Config->GetOptionInt("patchserver port");
	if(Port==0)
		Port=8040;
		
	Console->Print("Starting patchserver on port %i...", Port);
	mListener = nlOpen(Port, NL_TCP);
	if(mListener != NL_INVALID)
	{
		nlListen(mListener);
	} else
	{
		int err = nlGetError();
		if(err==NL_SYSTEM_ERROR)
			Console->Print("Patchserver error: %s", nlGetSystemErrorStr(nlGetSystemError()));
		else
			Console->Print("Patchserver error: %s", nlGetErrorStr(err));
	}
}

void PPatchServer::Update()
{
	if(mListener==NL_INVALID)
		return;
	
	NLsocket temp = nlAcceptConnection(mListener);		
	if(temp != NL_INVALID)
	{
		int clid = Server->NewClient();
		if(clid!=-1)
		{
			Console->Print("Patchserver: client %i connected", clid);
			PClient *Client = Server->GetClient(clid);
			Client->PatchConnect(temp);
			Console->Print("Client address: %s", Client->GetAddress());
			++mNumClients;
			
			PPatchState *state = new PPatchState();
			ClientStates.insert(std::make_pair(Client, state));
			state->mState = PPatchState::PS_CONNECTED;
		} else
		{
			Console->Print("Patchserver: Client connection refused (server full?)");
		}
	}
	
	for(PatchStateMap::iterator i=ClientStates.begin(); i!=ClientStates.end();)
	{
		PClient *Client = i->first;
		PPatchState *State = i->second;
		// node gets erased in FinalizeClient, increment iterator now
		++i;
		if(!ProcessClient(Client, State))
			FinalizeClient(Client, State);
	}
}

void PPatchServer::FinalizeClient(PClient *Client, PPatchState *State)
{
	Console->Print("Patchserver: client %i disconnected", Client->GetIndex());
	Client->PatchDisconnect();
	ClientStates.erase(Client);
	if(State->mPatchFile)
		--mNumFileTransfers;
	if(State->mSendFile)
		--mNumFileTransfers;
	delete State;
	--mNumClients;
}

// completes pending packets before disconnect
void PPatchServer::FinalizeClientDelayed(PClient *Client, PPatchState *State)
{
	Console->Print("Patchserver: client %i is about to be disconnected", Client->GetIndex());
	State->mWaitSend = true;
}

void PPatchServer::ClientDisconnected(PClient *Client)
{
	PatchStateMap::iterator node = ClientStates.find(Client);
	if(node == ClientStates.end())
		return;

	PPatchState *State = node->second;	
	FinalizeClient(Client, State);
}

bool PPatchServer::HandleHandshake(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize)
{
	static const u8 HANDSHAKE1A[6]={0xfe, 0x03, 0x00, 0x80, 0x01, 0x73};

	PGameSocket *Socket = Client->GetPatchSocket();
	
	switch(State->mState)
	{
		case PPatchState::PS_HANDSHAKE0 :
		{
			if(PacketSize==6 && *(u16*)&Packet[3]==0x0280 && Packet[5]==0x64)
			{
				Socket->Write(HANDSHAKE1A, sizeof(HANDSHAKE1A));
				State->mState = PPatchState::PS_HANDSHAKE1;
			} else
			{
				Console->Print("Patchserver protocol error (PS_HANDSHAKE0): invalid packet [%04x]", *(u16*)&Packet[3]);
				return false;
			}
			
			break;
		}
		
		case PPatchState::PS_HANDSHAKE1 :
		{
			if(PacketSize==6 && *(u16*)&Packet[3]==0x0080 && Packet[5]==0x6c)
				State->mState = PPatchState::PS_VERSIONREQUEST;
			else
			{
				Console->Print("Patchserver protocol error (PS_HANDSHAKE1): invalid packet [%04x]", *(u16*)&Packet[3]);
				return false;
			}
			break;
		}
		default:
			break;
	}
	
	return true;
}

bool PPatchServer::HandleVersionRequest(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize)
{
	static u8 VERSIONPACKET[13]={0xfe, 0x0a, 0x00, 0x37, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	
	PGameSocket *Socket = Client->GetPatchSocket();
	if(PacketSize==9 && *(u16*)&Packet[3]==0x007b)
	{
		State->mSerial = *(u16*)&Packet[7];
		*(u16*)&VERSIONPACKET[7]=State->mSerial;
		u32 ver = Config->GetOptionInt("server version");
		*(u32*)&VERSIONPACKET[9]=ver;
		Socket->Write(VERSIONPACKET, 13);
		State->mState = PPatchState::PS_GETPATCHORFILE;
	} else
	{
		Console->Print("Patchserver protocol error (PS_VERSIONREQUEST): invalid packet [%04x]", *(u16*)&Packet[3]);
		return false;
	}
	
	return true;
}

bool PPatchServer::HandleFileRequests(PClient *Client, PPatchState *State, const u8 *Packet, int PacketSize)
{
	static u8 STARTPATCH[13]={0xfe, 0x0a, 0x00, 0x38, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static u8 STARTFILE[13]={0xfe, 0x0a, 0x00, 0x3b, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	static u8 FILEERROR[9]={0xfe, 0x06, 0x00, 0x3d, 0x02, 0x00, 0x00, 0x00, 0x00};

	PGameSocket *Socket = Client->GetPatchSocket();
	// request patch
	if(PacketSize==13 && *(u16*)&Packet[3]==0x007c)
	{
		int nmax = Config->GetOptionInt("max file xfers");
		if(mNumFileTransfers>=nmax)
		{
			Console->Print("Patchserver: max file xfers exceed, killing client %i", Client->GetIndex());
			return false;
		}
		if(State->mPatchFile)
		{
			std::fclose(State->mPatchFile);
			State->mPatchFile=0;
			--mNumFileTransfers;
		}
		if(State->mSendFile)
		{
			Filesystem->Close(State->mSendFile);
			State->mSendFile=0;
			--mNumFileTransfers;
		}
		State->mSerial = *(u16*)&Packet[7];
		State->mCurrentPatch = *(u32*)&Packet[9];
		Console->Print("Patchserver: Patch request from client %i (v%i)", Client->GetIndex(), State->mCurrentPatch);
		if((bool)(State->mPatchSize = StartPatch(Client, State)))
		{
			Console->Print("Patchserver: Patch is available, %d bytes", State->mPatchSize);
			*(u16*)&STARTPATCH[7]=State->mSerial;
			*(u32*)&STARTPATCH[9]=State->mPatchSize;
			Socket->Write(STARTPATCH, 13);
			State->mState = PPatchState::PS_SENDPATCH;
		} else
		{
			Console->Print("Patchserver: Patch not available");
			*(u16*)&FILEERROR[7]=State->mSerial;
			Socket->Write(FILEERROR, 9);
			FinalizeClientDelayed(Client, State);
			State->mState=PPatchState::PS_UNKNOWN;
			return true;
		}
	} else
	// request file
	if(PacketSize > 9 && *(u16*)&Packet[3]==0x004d)
	{
		int nmax = Config->GetOptionInt("max file xfers");
		if(mNumFileTransfers>=nmax)
		{
			Console->Print("Patchserver: max file xfers exceed, killing client %i", Client->GetIndex());
			return false;
		}
		if(State->mPatchFile)
		{
			std::fclose(State->mPatchFile);
			State->mPatchFile=0;
			--mNumFileTransfers;
		}
		if(State->mSendFile)
		{
			Filesystem->Close(State->mSendFile);
			State->mSendFile=0;
			--mNumFileTransfers;
		}
		// request file
		State->mSerial = *(u16*)&Packet[7];
		char fn[256];
		strncpy(fn, (const char*)&Packet[10], Packet[9]);
		fn[Packet[9]]=0;
		State->mCurrentFile = fn;

		Console->Print("Patchserver: File request from client %i (%s)", Client->GetIndex(), fn);
		if((bool)(State->mFileSize = StartFile(Client, State)))
		{
			Console->Print("Patchserver: File %s is available, %d bytes", State->mCurrentFile.c_str(), State->mFileSize);
			*(u16*)&STARTFILE[7]=State->mSerial;
			*(u32*)&STARTFILE[9]=State->mFileSize;
			Socket->Write(STARTFILE, 13);
			State->mState = PPatchState::PS_SENDFILE;
		} else
		{
			Console->Print("Patchserver: Requested file %s not available", State->mCurrentFile.c_str());
			*(u16*)&FILEERROR[7]=State->mSerial;
			Socket->Write(FILEERROR, 9);
			FinalizeClientDelayed(Client, State);
			State->mState=PPatchState::PS_UNKNOWN;
			return true;
		}
	} else
	// send patch data
	if(PacketSize==17 && *(u16*)&Packet[3]==0x007d)
	{
		State->mSerial = *(u16*)&Packet[7];
		State->mCurrentPatch = *(u32*)&Packet[9];
		State->mPatchOffset = *(u32*)&Packet[13];
		if(!SendPatchData(Client, State))
		{
			Console->Print("Patchserver: SendPatchData failed on client %i", Client->GetIndex());
			Console->Print("Patchserver: (probably due to garbage packets)");
			// state is undefined now, kill this client
			return false;
		}
	} else
	// send file data
	if(PacketSize > 13 && *(u16*)&Packet[3]==0x00037)
	{
		State->mSerial = *(u16*)&Packet[7];
		State->mFileOffset = *(u32*)&Packet[9];
		if(!SendFileData(Client, State))
		{
			Console->Print("Patchserver: SendFileData failed on client %i", Client->GetIndex());
			Console->Print("Patchserver: (probably due to garbage packets)");
			// state is undefined now, kill this client
			return false;
		}
	} else
	{
		Console->Print("Patchserver protocol error (PS_GETPATCHORFILE): unknown packet");
		return false;
	}
				
	return true;
}

u32 PPatchServer::StartPatch(PClient *Client, PPatchState *State)
{
	/*std::*/stringstream path;
	char patchname[13];
	sprintf(patchname, "sp%06d.pat", State->mCurrentPatch);
	path << Config->GetOption("patches path") << "/" << patchname << '\0';
	State->mPatchFile = std::fopen(path.str().c_str(), "rb");
	if(State->mPatchFile)
	{
		++mNumFileTransfers;
		fseek(State->mPatchFile, 0, SEEK_END);
		u32 size = ftell(State->mPatchFile);
		fseek(State->mPatchFile, 0, SEEK_SET);
		return size;
	}
	return 0;
}

bool PPatchServer::SendPatchData(PClient *Client, PPatchState *State) const
{
	if(!State->mPatchFile)
		return false;

	u16 size = Config->GetOptionInt("patch packet size");
	
	const int BUFFERSIZE = 4082;
	
	size = std::min(BUFFERSIZE, std::max((s32)size, 64));
	static u8 Buffer[BUFFERSIZE+13];
	
	if(fseek(State->mPatchFile, State->mPatchOffset, SEEK_SET)!=0)
		return false;
		
	size = fread(&Buffer[13], 1, size, State->mPatchFile);
	Buffer[0]=0xfe;
	*(u16*)&Buffer[1]=size+10;
	Buffer[3]=0x39;
	Buffer[4]=0x02;
	Buffer[5]=0x00;
	Buffer[6]=0x00;
	*(u16*)&Buffer[7]=State->mSerial;
	*(u32*)&Buffer[9]=size;
	return Client->GetPatchSocket()->Write(Buffer, size+13)==size+13;
}

u32 PPatchServer::StartFile(PClient *Client, PPatchState *State)
{
	// security checks: reject file paths containing ':', '..' or slashes/backslashes at the beginning
	if((State->mCurrentFile.find(':') != std::string::npos)
		|| (State->mCurrentFile.find("..") != std::string::npos)
		|| (State->mCurrentFile.find('/') == 0)
		|| (State->mCurrentFile.find('\\') == 0))
	{
		return 0;
	}
	
	std::stringstream path;
	path << Config->GetOption("file path") << "/" << State->mCurrentFile << '\0';
	State->mSendFile = Filesystem->Open("", State->mCurrentFile.c_str());
	if(State->mSendFile)
	{
		++mNumFileTransfers;
		u32 size = State->mSendFile->GetSize();
		return size;
	}
	return 0;
}

bool PPatchServer::SendFileData(PClient *Client, PPatchState *State) const
{
	if(!State->mSendFile)
		return false;
		
	u16 size = Config->GetOptionInt("patch packet size");

	const int BUFFERSIZE = 4082;

	size = std::min(BUFFERSIZE, std::max((s32)size, 1));
	static u8 Buffer[BUFFERSIZE+13];
	
	State->mSendFile->Seek(State->mFileOffset);
		
	size = State->mSendFile->Read(&Buffer[13], size);
	Buffer[0]=0xfe;
	*(u16*)&Buffer[1]=size+10;
	Buffer[3]=0x3c;
	Buffer[4]=0x02;
	Buffer[5]=0x00;
	Buffer[6]=0x00;
	*(u16*)&Buffer[7]=State->mSerial;
	*(u32*)&Buffer[9]=size;
	return Client->GetPatchSocket()->Write(Buffer, size+13)==size+13;
}

bool PPatchServer::ProcessClient(PClient *Client, PPatchState *State)
{
	static const u8 HANDSHAKE0A[6]={0xfe, 0x03, 0x00, 0x80, 0x03, 0x6b};
	
	if(!State)
	{
		PatchStateMap::iterator node = ClientStates.find(Client);
		if(node == ClientStates.end())
			return false;

		State = node->second;
	}

	PGameSocket *Socket = Client->GetPatchSocket();

	if(State->mWaitSend && Socket->GetSendBufferSize()==0)
		return false;
	
	if(State->mState==PPatchState::PS_CONNECTED)
	{
		Socket->Write(HANDSHAKE0A, sizeof(HANDSHAKE0A));
		State->mState = PPatchState::PS_HANDSHAKE0;
	}
	
	int PacketSize=0;
	const u8 *Packet = Socket->Read(&PacketSize);
	if(PacketSize > 0)
	{
		switch(State->mState)
		{		
			case PPatchState::PS_HANDSHAKE1 :
			case PPatchState::PS_HANDSHAKE0 :
				return HandleHandshake(Client, State, Packet, PacketSize);
						
			case PPatchState::PS_VERSIONREQUEST :
				return HandleVersionRequest(Client, State, Packet, PacketSize);
			
			case PPatchState::PS_GETPATCHORFILE :
			case PPatchState::PS_SENDPATCH :
			case PPatchState::PS_SENDFILE :
				return HandleFileRequests(Client, State, Packet, PacketSize);
			default:
				break;
		}
	}	
	return true;
}
