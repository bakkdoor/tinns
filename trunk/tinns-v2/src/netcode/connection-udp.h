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
*/

#ifndef CONNECTIONUDP_H
#define CONNECTIONUDP_H

//static const int RECVBUFFERSIZE = 4096;
//static const int SENDBUFFERSIZE = 4096;

//static const clock_t DEFAULT_TIMEOUT = 60;

class ServerSocket;

class ConnectionUDP
{
    private:
            int                 m_Sockfd;
            struct sockaddr_in  m_RemoteAddr;

            u8                  m_ReceiveBuffer[RECVBUFFERSIZE];
            u8                  m_SendBuffer[SENDBUFFERSIZE];

            int                 m_SendSize;
            int                 m_RecvSize;
            int                 m_RecvRewind;

            std::clock_t        m_LastActive;
            std::clock_t        m_TimeOutValue;

            int                 m_Port;
    public:
            ConnectionUDP(int sockfd, int port);
            ~ConnectionUDP();

            bool                update();
            void                flushSendBuffer();

            int                 getPort() { return m_Port; }

            struct sockaddr_in  getAddr() { return m_RemoteAddr; }
            int                 getSockfd() { return m_Sockfd; }

            bool                timeOut() const;
            inline clock_t      getTimeOutValue() const { return m_TimeOutValue; }
            inline void         setTimeOutValue(clock_t Value) { m_TimeOutValue = Value; }

            inline int          getRecvBufferSize() const { return m_RecvSize; }
            inline int          getSendBufferSize() const { return m_SendSize; }

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

            char*         	getLocalAddress();
};

#endif
