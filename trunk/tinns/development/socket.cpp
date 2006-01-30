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
        socket.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 25 Dec 2005 Namikon
        REASON: - Added GPL
*/

#include "tinns.h"

static const clock_t DEFAULT_TIMEOUT = 60;

PSocket::PSocket(const NLsocket &Socket, const NLaddress *LocalAddr, const NLaddress *RemoteAddr) : mSocket(Socket)
{
	if(Socket==NL_INVALID)
		Console->Print("PSocket: invalid socket supplied");
	memset(mReceiveBuffer, 0, RECVBUFFERSIZE);
	memset(mSendBuffer, 0, SENDBUFFERSIZE);
	mTimeOutValue = DEFAULT_TIMEOUT;
	mRecvSize = mRecvRewind = 0;
	mSendSize = 0;
	mLastActive = std::clock();
	if(LocalAddr)
	{
		nlAddrToString(LocalAddr, mLocalIP);
		nlSetLocalAddr(LocalAddr);
	} else
	{
		NLaddress nladr;
		nlGetLocalAddr(mSocket, &nladr);
		nlAddrToString(&nladr, mLocalIP);
	}

	if(RemoteAddr)
		memcpy(&mRemoteAddr, RemoteAddr, sizeof(NLaddress));
	else
		nlGetRemoteAddr(mSocket, &mRemoteAddr);
}

PSocket::~PSocket()
{
	if(mSocket != NL_INVALID)
		nlClose(mSocket);
}

void PSocket::FlushSendBuffer()
{
	int MaxWrite = mSendSize;
	if(MaxWrite > 0)
	{
		nlSetRemoteAddr(mSocket, &mRemoteAddr);
		int written = nlWrite(mSocket, mSendBuffer, MaxWrite);
		if(written == NL_INVALID)
		{
			written = 0;
			int err = nlGetError();
			if(err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
			{
				nlClose(mSocket);
				mSocket = NL_INVALID;
			} else
			{
				if(err==NL_SYSTEM_ERROR)
					Console->Print("Socket error (%08x): %s", mSocket, nlGetSystemErrorStr(nlGetSystemError()));
				else
					Console->Print("Socket error (%08x): %s", mSocket, nlGetErrorStr(err));
			}
		}

		if(written > 0)
		{
			mLastActive = std::clock();
			int Rewind = min(mSendSize, SENDBUFFERSIZE-written);
			memmove(mSendBuffer, &mSendBuffer[written], Rewind);
			mSendSize -= written;
		}
	}
}

bool PSocket::Update()
{
	if(mSocket==NL_INVALID)
		return false;

	// read() returns a pointer to the local receive buffer and sets the rewind value
	// here the rewind takes place, ie. previously read data is removed from the buffer
	if(mRecvRewind > 0)
	{
		int Rewind = min(mRecvRewind, RECVBUFFERSIZE-mRecvRewind);
		memmove(mReceiveBuffer, &mReceiveBuffer[mRecvRewind], Rewind);
		mRecvSize -= mRecvRewind;
		mRecvRewind = 0;
	}

	int MaxRead = RECVBUFFERSIZE-mRecvSize;
	if(MaxRead > 0)
	{
		int read = nlRead(mSocket, &mReceiveBuffer[mRecvSize], MaxRead);
		if(read == NL_INVALID)
		{
			read = 0;
			int err = nlGetError();
			if(err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
			{
				nlClose(mSocket);
				mSocket = NL_INVALID;
				return false;
			}
		}

		if(read > 0)
		{
			mRecvSize += read;
			mLastActive = std::clock();
		}
	}

	int MaxWrite = mSendSize;
	if(MaxWrite > 0)
	{
		nlSetRemoteAddr(mSocket, &mRemoteAddr);
		int written = nlWrite(mSocket, mSendBuffer, MaxWrite);
		if(written == NL_INVALID)
		{
			written = 0;
			int err = nlGetError();
			if(err == NL_MESSAGE_END || err == NL_SOCK_DISCONNECT)
			{
				nlClose(mSocket);
				mSocket = NL_INVALID;
				return false;
			} else
			{
				if(err==NL_SYSTEM_ERROR)
					Console->Print("Socket error (%08x): %s", mSocket, nlGetSystemErrorStr(nlGetSystemError()));
				else
					Console->Print("Socket error (%08x): %s", mSocket, nlGetErrorStr(err));
				return false;
			}
		}

		if(written > 0)
		{
			mLastActive = std::clock();
			int Rewind = min(mSendSize, SENDBUFFERSIZE-written);
			memmove(mSendBuffer, &mSendBuffer[written], Rewind);
			mSendSize -= written;
		}
	}

	return true;
}

const u8 *PSocket::Read(int *Size)
{
	if(mSocket == NL_INVALID || mRecvSize<=0 || !Size)
		return 0;

	u16 size = mRecvSize;
	if(*Size==0)
		*Size=size;
	else
		*Size=min(*Size, (s32)size);
	u8* ptr = mReceiveBuffer;
	mRecvRewind = *Size;
	return ptr;
}

int PSocket::Write(const void *Data, int Size)
{
	if(mSocket == NL_INVALID)
		return 0;

	// data is stored in sendbuffer. gets sent in next update()
	int MaxWrite = SENDBUFFERSIZE-mSendSize;
	int written = 0;
	if(MaxWrite > 0)
	{
		written = min(Size, MaxWrite);
		memcpy(&mSendBuffer[mSendSize], Data, written);
		mSendSize += written;
	}

	return written;
}

int PSocket::Write(const char *String)
{
	if(!String)
		return 0;

	return Write(String, strlen(String));
}

int PSocket::Write(u8 Data)
{
	return Write(&Data, sizeof(u8));
}

int PSocket::Write(u16 Data)
{
	return Write(&Data, sizeof(u16));
}

int PSocket::Write(u32 Data)
{
	return Write(&Data, sizeof(u32));
}

int PSocket::Write(float Data)
{
	return Write(&Data, sizeof(float));
}

int PSocket::Write(double Data)
{
	return Write(&Data, sizeof(double));
}

bool PSocket::TimeOut() const
{
	clock_t now = std::clock();
	if((now-mLastActive)/ /*std::*/CLOCKS_PER_SEC >= mTimeOutValue)
		return true;

	return false;
}

const char *PSocket::GetLocalAddress()
{
	return mLocalIP;
}

// ===========================================================

PGameSocket::PGameSocket(const NLsocket &Socket) : PSocket(Socket)
{
}


const u8 *PGameSocket::Read(int *Size)
{
	if(mSocket == NL_INVALID || mRecvSize<3)
		return 0;

	u16 size = *(u16*)&mReceiveBuffer[1]+3;
	if(mRecvSize < size)
		return 0;// packet not complete

	if(Size)
		*Size=size;

	mRecvRewind = size;
	u8* ptr = mReceiveBuffer;
	return ptr;
}
