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
	connection-udp.cpp - a connection class for udp

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
*/

#include "../patch/main.h"

ConnectionUDP::ConnectionUDP(int sockfd, int port, int adress, int tmpport)
{
    m_Sockfd = sockfd;
    m_Port = port;

    m_TimeOutValue = DEFAULT_TIMEOUT;
    m_LastActive = std::clock();

    m_RemoteAddr.sin_family = AF_INET;       // host byte order
    m_RemoteAddr.sin_port = htons(tmpport);     // short, network byte order
    m_RemoteAddr.sin_addr.s_addr = adress;   // TODO: Get IP of client

    Console->Print("Peer IP address: %s", inet_ntoa(m_RemoteAddr.sin_addr));
    Console->Print("Peer port      : %d", ntohs(m_RemoteAddr.sin_port));



//    m_RemoteAddr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
//    memset(&(m_RemoteAddr.sin_zero), '\0', 8); // zero the rest of the struct
}


ConnectionUDP::~ConnectionUDP()
{
    close(m_Sockfd);
}


void ConnectionUDP::flushSendBuffer()
{
    int MaxWrite = m_SendSize;
    if(MaxWrite > 0)
    {
        int written = sendto(m_Sockfd, m_SendBuffer, MaxWrite, 0, (struct sockaddr *)&m_RemoteAddr, sizeof(struct sockaddr));
        if(written == -1)
        {
            written = 0;
            perror("udp-send1");
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


bool ConnectionUDP::update()
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

    int MaxRead = RECVBUFFERSIZE-m_RecvSize;
    if(MaxRead > 0)
    {
        int numBytes;
        socklen_t addrlen;
        //struct sockaddr_in tempAddr;  // need to built in check,
                                        // if the incoming data is coming from the client or someone else!

        numBytes = recvfrom(m_Sockfd, m_ReceiveBuffer, sizeof(m_ReceiveBuffer), 0,
            (struct sockaddr *)&m_RemoteAddr, &addrlen); // get the data

        if(numBytes == -1) // an error has occured -> output it to the console
        {
            perror("udp-receive");
        }
        else if(numBytes > 0)
        {
            m_RecvSize += numBytes;
            m_LastActive = std::clock();
        }
    }

    // send data from sendbuffer
	int MaxWrite = m_SendSize;
	if(MaxWrite > 0)
	{
		int written = sendto(m_Sockfd, m_SendBuffer, MaxWrite, 0, (struct sockaddr *)&m_RemoteAddr, sizeof(struct sockaddr));
		if(written == -1) // error while sending data -> output error-msg to console
		{
		    perror("udp-send2");

		    //close(m_Sockfd);
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


const u8* ConnectionUDP::read(int* size)
{
    if(m_RecvSize<=0 || !size)
    {
		return 0;
    }

	u16 _size = m_RecvSize;
	if(*size==0)
	{
		*size=_size;
	}
	else
	{
		*size=min(*size, (s32)_size);
	}

	u8* ptr = m_ReceiveBuffer;
	m_RecvRewind = *size;
	return ptr;
}


int ConnectionUDP::write(const void* data, int size)
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

int ConnectionUDP::write(const char *String)
{
	if(!String)
		return 0;

	return write(String, strlen(String));
}

int ConnectionUDP::write(u8 Data)
{
	return write(&Data, sizeof(u8));
}

int ConnectionUDP::write(u16 Data)
{
	return write(&Data, sizeof(u16));
}

int ConnectionUDP::write(u32 Data)
{
	return write(&Data, sizeof(u32));
}

int ConnectionUDP::write(float Data)
{
	return write(&Data, sizeof(float));
}

int ConnectionUDP::write(double Data)
{
	return write(&Data, sizeof(double));
}

bool ConnectionUDP::timeOut() const
{
	clock_t now = std::clock();
	if((now-m_LastActive)/ /*std::*/CLOCKS_PER_SEC >= m_TimeOutValue)
		return true;

	return false;
}

char* ConnectionUDP::getLocalAddress()
{

    return inet_ntoa(m_RemoteAddr.sin_addr);
}

