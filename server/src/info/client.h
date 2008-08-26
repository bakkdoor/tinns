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

#ifndef CLIENT_H
#define CLIENT_H

#ifdef _MSC_VER
	#pragma once
#endif

enum PClientConnection
{
	PCC_NONE = 0,
	PCC_INFO = 1
};

class PClient
{
	private :
    ConnectionTCP* m_TCPConnection;
		int mIndex;
		int mConnection;
		u32 mAccountID;

	protected :
	public :
		PClient(int Index);
		~PClient();

		inline int GetIndex() const { return mIndex; }

		void setTCPConnection(ConnectionTCP* conn) { m_TCPConnection = conn; mConnection = PCC_INFO; }
		inline ConnectionTCP* getTCPConn() const { return m_TCPConnection; }

		inline int GetConnection() const { return mConnection; }
		inline const char *GetAddress() const { return m_TCPConnection->getRemoteAddress(); }

    inline void setAccountID(u32 nAccountID) { mAccountID = nAccountID; }
    inline u32 getAccountID() { return mAccountID; }
    
		void InfoDisconnect();

		void Update();
};

#endif

