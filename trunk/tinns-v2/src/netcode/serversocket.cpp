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
	serversocket.h - a serversocket class managing all connections (udp/tcp)

	Authors:
	- bakkdoor

	MODIFIED: 09 Feb 2006 bakkdoor
	REASON: - introduced
*/

#include "../patch/tinns.h"

ServerSocket::ServerSocket()
{
    FD_ZERO(&m_MainSetTCP);    // clear the master and temp sets
    FD_ZERO(&m_ReadSetTCP);

    m_LastUDPPort = 5000;
}

ServerSocket::~ServerSocket()
{
    close(m_ListenerTCP);
}

bool ServerSocket::open(int port)
{
    // get the tcp listener
    if ((m_ListenerTCP = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("tcp-socket");
        return false;
    }

    int yes=1;

    // lose the pesky "address already in use" error message
    if (setsockopt(m_ListenerTCP, SOL_SOCKET, SO_REUSEADDR, &yes,
                                                        sizeof(int)) == -1) {
        perror("tcp-setsockopt");
        return false;
    }

    // bind
    memset(&m_ServerAddr,0,sizeof(struct sockaddr_in));
    m_ServerAddr.sin_family = AF_INET;
    m_ServerAddr.sin_addr.s_addr = INADDR_ANY;
    m_ServerAddr.sin_port = htons(port);

    // bind with tcp and udp sockfd
    if (bind(m_ListenerTCP, (struct sockaddr *)&m_ServerAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("tcp-bind");
        return false;
    }

    // set listening-socket to non-blocking
    fcntl(m_ListenerTCP, F_SETFL, O_NONBLOCK);

    // listen
    if (listen(m_ListenerTCP, 10) == -1)
    {
        perror("tcp-listen");
        return false;
    }

    FD_ZERO(&m_MainSetTCP);
    FD_ZERO(&m_ReadSetTCP);

    // add the listener to the master set
    FD_SET(m_ListenerTCP, &m_MainSetTCP);

    // keep track of the biggest file descriptor
    m_FdMaxTCP = m_ListenerTCP; // so far, it's this one

    Console->Print("Server running on port %d", port);

    m_TimeOut.tv_sec = 0;
    m_TimeOut.tv_usec = 30;

    return true;
}

void ServerSocket::update()
{

    // copy fd_sets from main-set to temp. read-set
    m_ReadSetTCP = m_MainSetTCP;

    //FD_ZERO(&m_ReadSetTCP);
	//FD_SET (m_ListenerTCP, &m_ReadSetTCP);

    // select incoming data for tcp & udp
    if (select(m_FdMaxTCP+1, &m_ReadSetTCP, NULL, NULL, &m_TimeOut) == -1)
    {
        perror("tcp-select");
    }

    // check for new tcp connections
    if (FD_ISSET(m_ListenerTCP, &m_ReadSetTCP)) // we got one!!
    {
        m_bNewTCPConnection = true;
    }
    else
    {
        m_bNewTCPConnection = false;
    }

}

bool ServerSocket::newConnection()
{
    return (m_bNewTCPConnection);
}

ConnectionTCP* ServerSocket::getTCPConnection()
{
    if(m_bNewTCPConnection)
    {
        struct sockaddr_in  RemoteAddr; // new incoming client address
        socklen_t addrlen = sizeof(RemoteAddr);
        int NewSockfd; // newly accept()ed socket descriptor

        // handle new connections
        if ((NewSockfd = accept(m_ListenerTCP, (struct sockaddr *)&RemoteAddr, &addrlen)) == -1)
        {
            m_bNewTCPConnection = false; // set back to normal
            perror("tcp-accept");
            return 0;
        }
        else
        {
            FD_SET(NewSockfd, &m_MainSetTCP); // add to master set
            if (NewSockfd > m_FdMaxTCP) // keep track of the maximum
            {
                m_FdMaxTCP = NewSockfd;
            }

            // set new socket to non-blocking
            fcntl(NewSockfd, F_SETFL, O_NONBLOCK);

            ConnectionTCP* tcpConn = new ConnectionTCP(NewSockfd, RemoteAddr, this);

            m_bNewTCPConnection = false; // set back to normal

            return tcpConn;
        }
    }

    return 0;
}

ConnectionUDP* ServerSocket::getUDPConnection()
{
    int udpSockfd;

    if ((udpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("udp-socket");
        return 0;
    }

    struct sockaddr_in my_addr;

    int Port = m_LastUDPPort;

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(Port);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    if (bind(udpSockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
        perror("udp-bind");
        return 0;
    }

        ConnectionUDP* udpConn = new ConnectionUDP(udpSockfd, Port);

	m_LastUDPPort++;

        return udpConn;
}

bool ServerSocket::isDataAvailable(int sockfd)
{
    if(FD_ISSET(sockfd, &m_ReadSetTCP))
    {
        return true;
    }

    return false;
}

void ServerSocket::delSocketFromSet(int sockfd)
{
    FD_CLR(sockfd, &m_MainSetTCP);
}

void ServerSocket::closeServer()
{
    close(m_ListenerTCP);
}
