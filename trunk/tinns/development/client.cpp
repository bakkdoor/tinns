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
	client.cpp

	Authors:
	- v00d00
	- Akiko
	- Namikon

	MODIFIED: 30 Nov 2005 Akiko
	REASON: - added GPL
            - added modifications by Namikon
*/

#include "tinns.h"

PClient::PClient(int Index)
{
	mAccount = 0;
	mIndex = Index;
	mCharID = 0;
	mLevel = PCL_BANNED;
	mPatchSocket = mInfoSocket = mGameSocket = 0;
	mUDPSocket = 0;
	mConnection = PCC_NONE;
	mUDP_ID = 0;
	mSessionID = 37917;

	//m_PositionVec = new vector3df(0.0f, 0.0f, 0.0f, 0.0f, 0.0f); <- Useless here ..?
}

PClient::~PClient()
{
	delete mPatchSocket;
	delete mInfoSocket;
	delete mGameSocket;
	delete mUDPSocket;
}

bool PClient::PatchConnect(const NLsocket &Socket)
{
	mPatchSocket = new PGameSocket(Socket);
	mConnection |= PCC_PATCH;

	nlGetRemoteAddr(Socket, &mAddress);
	nlAddrToString(&mAddress, mAddrString);
	return true;
}

void PClient::PatchDisconnect()
{
	delete mPatchSocket;
	mPatchSocket = 0;
	mConnection &= ~PCC_PATCH;
}

bool PClient::InfoConnect(const NLsocket &Socket)
{
	mInfoSocket = new PGameSocket(Socket);
	mConnection |= PCC_INFO;

	nlGetRemoteAddr(Socket, &mAddress);
	nlAddrToString(&mAddress, mAddrString);
	return true;
}

void PClient::InfoDisconnect()
{
	mAccount = 0;
	delete mInfoSocket;
	mInfoSocket = 0;
	mConnection &= ~PCC_INFO;
}

bool PClient::GameConnect(const NLsocket &Socket)
{
	mGameSocket = new PGameSocket(Socket);
	mConnection |= PCC_GAME;

	nlGetRemoteAddr(Socket, &mAddress);
	nlAddrToString(&mAddress, mAddrString);
	return true;
}

void PClient::GameDisconnect()
{
	mAccount = 0;
	delete mGameSocket;
	mGameSocket = 0;
	mConnection &= ~PCC_GAME;
}

void PClient::SetRemoteUDPAddr(u32 Addr, u16 Port)
{
	char ip[64];
	std::sprintf(ip, "%d.%d.%d.%d:%d", (Addr&0x000000ff), (Addr&0x0000ff00)>>8, (Addr&0x00ff0000)>>16, (Addr&0xff000000)>>24, Port);
	nlStringToAddr(ip, &mRemoteUDPAddr);
	nlSetAddrPort(&mRemoteUDPAddr, Port);
}

u16 PClient::OpenUDP()
{
	//NLsocket UDP = nlOpen(0, NL_UDP_BROADCAST);
	if(nlHint(NL_REUSE_ADDRESS, NL_TRUE) == NL_FALSE)
	{
        Console->Print("NL error: [%s] NL sys_error: [%s]", nlGetErrorStr(nlGetError()), nlGetSystemErrorStr(nlGetError()));
	}
	NLsocket UDP = nlOpen(0, NL_UNRELIABLE);

	if(UDP != NL_INVALID)
	{
		nlGetLocalAddr(UDP, &mLocalUDPAddr);
		u16 Local = nlGetPortFromAddr(&mLocalUDPAddr);
		mUDPSocket = new PSocket(UDP, &mLocalUDPAddr, &mRemoteUDPAddr);
		return Local;
	}

	return 0;
}

void PClient::LoggedIn(PAccount *Account)
{
	mAccount = Account;
}

void PClient::Update()
{
	if(mPatchSocket)
	{
		if(mPatchSocket->TimeOut())
		{
			Console->Print("Patchsocket: Client %i: timeout", mIndex);
			PatchServer->ClientDisconnected(this);
		} else
		if(!mPatchSocket->Update())
			PatchServer->ClientDisconnected(this);
	}

	if(mInfoSocket)
	{
		if(mInfoSocket->TimeOut())
		{
			Console->Print("InfoSocket: Client %i: timeout", mIndex);
			InfoServer->ClientDisconnected(this);
		} else
		if(!mInfoSocket->Update())
			InfoServer->ClientDisconnected(this);
	}

	if(mGameSocket)
	{
		if(mGameSocket->TimeOut())
		{
			Console->Print("GameSocket: Client %i: timeout", mIndex);
			GameServer->ClientDisconnected(this);
		} else
		if(!mGameSocket->Update())
			GameServer->ClientDisconnected(this);
	}

	if(mUDPSocket)
	{
		if(mUDPSocket->TimeOut())
		{
			Console->Print("Game UDP: Client %i: timeout", mIndex);
			GameServer->UDPStreamClosed(this);
		} else
		if(!mUDPSocket->Update())
			GameServer->UDPStreamClosed(this);
	}
}

