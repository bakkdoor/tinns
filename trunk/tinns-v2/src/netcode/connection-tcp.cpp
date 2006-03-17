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
	connection-tcp.cpp - a connection class for tcp

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
*/

#include "../patch/tinns.h"

ConnectionTCP::ConnectionTCP(int sockfd, struct sockaddr_in addr, ServerSocket* server)
{
    m_Sockfd = sockfd;
    m_RemoteAddr = addr;
    if(server)
    {
        m_ServerSocket = server;
    }

    m_bConnected = true; // client obviously is connected at creation...

    m_TimeOutValue = DEFAULT_TIMEOUT;
    m_LastActive = std::clock();
}

ConnectionTCP::~ConnectionTCP()
{
    m_ServerSocket->delSocketFromSet(m_Sockfd);
    close(m_Sockfd);
}

void ConnectionTCP::flushSendBuffer()
{
    int MaxWrite = m_SendSize;
    if(MaxWrite > 0)
    {
        int written = send(m_Sockfd, m_SendBuffer, MaxWrite, 0);
        if(written == -1)
        {
            written = 0;
            perror("send");
            close(m_Sockfd);
        }
        else if(written > 0)
        {
            m_LastActive = std::clock();
            int Rewind = min(m_SendSize, SENDBUFFERSIZE-written);
            memmove(m_SendBuffer, &m_SendBuffer[written], Rewind);
            m_SendSize -= written;
        }
    }
}

char* ConnectionTCP::getLocalAddress()
{

    return inet_ntoa(m_RemoteAddr.sin_addr);
}

bool ConnectionTCP::update()
{
	// read() returns a pointer to the local receive buffer and sets the rewind value
	// here the rewind takes place, ie. previously read data is removed from the buffer
	if(m_RecvRewind > 0)
	{
		int Rewind = min(m_RecvRewind, RECVBUFFERSIZE-m_RecvRewind);
		memmove(m_ReceiveBuffer, &m_ReceiveBuffer[m_RecvRewind], Rewind);
		m_RecvSize -= m_RecvRewind;
		m_RecvRewind = 0;
	}

    //check if data is available for this socket and if yes, read into m_ReceiveBuffer
    if(m_ServerSocket->isDataAvailable(m_Sockfd))
    {
        int MaxRead = RECVBUFFERSIZE-m_RecvSize;
        if(MaxRead > 0)
        {
            int numBytes;

            numBytes = recv(m_Sockfd, m_ReceiveBuffer, sizeof(m_ReceiveBuffer), 0); // get the data
            if(numBytes == 0) // disconnected !!
            {
                m_bConnected = false; // set to not-connected and close the socket
                m_ServerSocket->delSocketFromSet(m_Sockfd);
                close(m_Sockfd);
                return false;
            }
            else if(numBytes == -1) // an error has occured -> output it to the console
            {
                perror("receive");
            }
            else if(numBytes > 0)
            {
                m_bConnected = true;
                m_RecvSize += numBytes;
                m_LastActive = std::clock();
            }
        }
    }

    // send data from sendbuffer
	int MaxWrite = m_SendSize;
	if(MaxWrite > 0)
	{
		int written = send(m_Sockfd, m_SendBuffer, MaxWrite, 0);
		if(written == -1) // error while sending data -> output error-msg to console
		{
		    perror("send");

            return false;
        }
        else if(written > 0)
		{
		    m_LastActive = std::clock();
			int Rewind = min(m_SendSize, SENDBUFFERSIZE-written);
			memmove(m_SendBuffer, &m_SendBuffer[written], Rewind);
			m_SendSize -= written;
		}
	}

	return true;
}

const u8* ConnectionTCP::read(int* Size)
{
	if(!m_Sockfd || m_RecvSize<3)
		return 0;

	u16 size = *(u16*)&m_ReceiveBuffer[1]+3;
	if(m_RecvSize < size)
		return 0;// packet not complete

	if(Size)
		*Size=size;

	m_RecvRewind = size;
	u8* ptr = m_ReceiveBuffer;
	return ptr;
}

int ConnectionTCP::write(const void* data, int size)
{
    // data is stored in sendbuffer. gets sent in next update()
	int MaxWrite = SENDBUFFERSIZE-m_SendSize;
	int written = 0;
	if(MaxWrite > 0)
	{
		written = min(size, MaxWrite);
		memcpy(&m_SendBuffer[m_SendSize], data, written);
		m_SendSize += written;
	}

	return written;
}

int ConnectionTCP::write(const char *String)
{
	if(!String)
		return 0;

	return write(String, strlen(String));
}

int ConnectionTCP::write(u8 Data)
{
	return write(&Data, sizeof(u8));
}

int ConnectionTCP::write(u16 Data)
{
	return write(&Data, sizeof(u16));
}

int ConnectionTCP::write(u32 Data)
{
	return write(&Data, sizeof(u32));
}

int ConnectionTCP::write(float Data)
{
	return write(&Data, sizeof(float));
}

int ConnectionTCP::write(double Data)
{
	return write(&Data, sizeof(double));
}

bool ConnectionTCP::timeOut() const
{
	clock_t now = std::clock();
	if((now-m_LastActive)/ /*std::*/CLOCKS_PER_SEC >= m_TimeOutValue)
		return true;

	return false;
}
