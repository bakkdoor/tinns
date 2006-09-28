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
	connection-udp.h - a connection class for udp

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
	
	MODIFIED: 01 Jul 2006 hammag
	REASON: - added private member m_ServerSocket
	            and added corresponding parameter in constructor;
	            
  MODIFIED: 24 Jul 2006 hammag
	REASON: - changed member data prefix from "m_" to "m" in for homogeneity with the reste of TinNS code
	        - added private members data mQueueIn and mQueueOut
	        - added public members methods SendMessage(), GetMessage(), DeleteOutgoingMessages() and modified code accordingly
	        - removed old read/write member data, and added a compatibility mSendBufferMsg* member

	MODIFIED: 05 Aug 2006 hammag
	REASON: - renamed "getLocalAddress()" to "getRemoteAddress()" as it is ... what it does !	        
	        
	TODO:   - remove old read/write compatibility methods when not needed anymore
	        - see .cpp for current implementation limits
	
*/

#ifndef CONNECTIONUDP_H
#define CONNECTIONUDP_H

class ServerSocket;

class ConnectionUDP
{
    private:
            int                 mSockfd;
            struct sockaddr_in  mRemoteAddr;

//            u8                  mReceiveBuffer[RECVBUFFERSIZE];
//            u8                  mSendBuffer[SENDBUFFERSIZE];

//            int                 mSendSize;
//            int                 mRecvSize;
//            int                 mRecvRewind;

            std::time_t        mLastActive;
            std::time_t        mTimeOutValue;

            int                 mPort;
            ServerSocket*       mServerSocket; // pointer to the serversocket
            
            std::queue<PMessage*> mQueueIn;
            std::queue<PMessage*> mQueueOut;
            
    public:
            ConnectionUDP(int sockfd, int port, int remoteadress, int remoteport, ServerSocket* server);
            ~ConnectionUDP();

            bool                update();

            int                 getPort() { return mPort; }
            struct sockaddr_in  getAddr() { return mRemoteAddr; }
            int                 getSockfd() { return mSockfd; }
            char*               getRemoteAddress();
            
            bool                timeOut() const;
            inline time_t       getTimeOutValue() const { return mTimeOutValue; }
            inline void         setTimeOutValue(time_t Value) { mTimeOutValue = Value; }
            
            inline void SendMessage(PMessage* nMessage) { if (nMessage) mQueueOut.push(nMessage); }
            inline int GetReadyMessagesNumber() { return mQueueIn.size(); }
            PMessage* GetMessage(); // returns NULL if no message available
            void DeleteOutgoingMessages();
            
/**************** Old I/F compatibility stuff ******************/
    private:
            PMessage* mSendBufferMsg;    
              
    public:
            int                 getRecvBufferSize();
            int                 getSendBufferSize();
            void                flushSendBuffer();
            // returns a pointer to the internal receive buffer
            // Size contains the number of octets to read (or 0 to read entire buffer)
            // number of octets available is returned in Size
            const u8*           read(int* size);

            int                 write(const void* data, int size);
            int                 write(u8 data);
            int                 write(u16 data);
            int                 write(u32 data);
            int                 write(float data);
            int                 write(double data);
            int                 write(const char* string);


};

#endif
