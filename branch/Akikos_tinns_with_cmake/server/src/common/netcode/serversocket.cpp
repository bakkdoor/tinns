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
	
  MODIFIED: 27 Jun 2006 hammag
	REASON: - added modif to keep m_TimeOut value in update() as select (may) modifie this
	          parameter in Linux
	
	MODIFIED: 01 Jul 2006 hammag
	REASON: - ServerSocket() : moved m_TimeOut initialization in constructor
          - added settimeout() method to permit various behaviors
          - ServerSocket(), open() : added m_MainSetUDP and m_MainSetGlobal init
          - open() : add m_ListenerTCP to m_MainSetGlobal too
          - update() : now m_MainSetGlobal is copied to m_ReadSetTCP set,
	            and added a local fdMax for select
          - getUDPConnection(): add new UDP socket to m_MainSetUDP and m_MainSetGlobal, 
	            and keep track in m_FdMaxUDP
          - getTCPConnection(): add new TCP socket to m_MainSetGlobal too
          - delSocketFromSet() : added removal of sockfd from m_MainSetUDP and m_MainSetGlobal too
          - getTCPConnection() : moved non-blocking setting to ConnectionTCP::ConnectionTCP() for better class coherency
          - getUDPConnection(): added this to new ConnectionUDP creation parameters
	
	MODIFIED: 05 Aug 2006 hammag
	REASON: - now use UDP port in selected range for client connection (see .h)         
	
  TODO:   - delSocketFromSet() : Could now be improved with two separte methodes for TCP and UDP removal
          - in all 3 servers: put select timeout setting in config file.
	        - don't access Config object from netcode's objects. Use parameter members
              set by object owner.
*/

#include "main.h"

ServerSocket::ServerSocket()
{
    FD_ZERO(&m_MainSetTCP);    // clear the master and temp sets
    FD_ZERO(&m_ReadSetTCP);
    FD_ZERO(&m_MainSetUDP);
    FD_ZERO(&m_MainSetGlobal);
    
    m_FdMaxUDP=0;
    
    m_TimeOut.tv_sec = 0;
    m_TimeOut.tv_usec = 30;
    
    //m_LastUDPPort = 5000;
}

ServerSocket::~ServerSocket()
{
    close(m_ListenerTCP);
}

void ServerSocket::settimeout(long timeout_sec, long timeout_usec)
{
  if ((timeout_sec >= 0) && (timeout_usec >= 0))
  {
    m_TimeOut.tv_sec = timeout_sec;
    m_TimeOut.tv_usec = timeout_usec;
  }
    
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

    // bind with tcp sockfd
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
    FD_ZERO(&m_MainSetUDP);
    FD_ZERO(&m_MainSetGlobal);
    
    m_FdMaxUDP=0;
    
    // add the listener to the master sets
    FD_SET(m_ListenerTCP, &m_MainSetTCP);
    FD_SET(m_ListenerTCP, &m_MainSetGlobal);

    // keep track of the biggest file descriptor
    m_FdMaxTCP = m_ListenerTCP; // so far, it's this one

    //Console->Print("Server running on port %d", port);

    return true;
}

void ServerSocket::update()
{
  struct timeval tmp_TimeOut;
  int fdMax;
  
    // copy fd_sets from main-set to temp. read-set
    m_ReadSetTCP = m_MainSetGlobal;

    fdMax = std::max(m_FdMaxTCP, m_FdMaxUDP);
    
    //FD_ZERO(&m_ReadSetTCP);
	//FD_SET (m_ListenerTCP, &m_ReadSetTCP);

    // select incoming data for tcp & udp
    tmp_TimeOut = m_TimeOut; //save m_TimeOut... will be modified by select
    if (select(fdMax+1, &m_ReadSetTCP, NULL, NULL, &tmp_TimeOut) == -1)
    {
        perror("select");
    }
    
    //Select exit condition logging
    /*
    if ((tmp_TimeOut.tv_sec == 0) && (tmp_TimeOut.tv_usec == 0))
    {
      Console->LPrint(GREEN, BLACK, "[Idle]");
      Console->LPrint(" Exiting select on timeout (remains %d sec and %d usec)", tmp_TimeOut.tv_sec, tmp_TimeOut.tv_usec);
      Console->LClose();
    }
    else     
    {
      Console->LPrint(YELLOW, BLACK, "[Active]");
      Console->LPrint(" Exiting select with remaining time %d sec and %d usec", tmp_TimeOut.tv_sec, tmp_TimeOut.tv_usec);
      Console->LClose();
    }
	  */

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
            FD_SET(NewSockfd, &m_MainSetTCP); // add to master TCP set
            FD_SET(NewSockfd, &m_MainSetGlobal); // add to master global set
            if (NewSockfd > m_FdMaxTCP) // keep track of the maximum
            {
                m_FdMaxTCP = NewSockfd;
            }

            ConnectionTCP* tcpConn = new ConnectionTCP(NewSockfd, RemoteAddr, this);

            m_bNewTCPConnection = false; // set back to normal

            return tcpConn;
        }
    }

    return 0;
}

ConnectionUDP* ServerSocket::getUDPConnection(long remoteadress, int remoteport)
{
    int udpSockfd;

    if ((udpSockfd = socket(PF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("udp-socket");
        return NULL;
    }

    struct sockaddr_in my_addr;

    u16 Port = Config->GetOptionInt("gameserver_udpport_min");
    u16 maxPort = Config->GetOptionInt("gameserver_udpport_max");

    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(Port);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(&(my_addr.sin_zero), '\0', 8); // zero the rest of the struct

    //Search a free udp port to use (could be optimised for faster port allocation)
    while (bind(udpSockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1)
    {
      if (errno == EADDRINUSE)
      {       
        my_addr.sin_port = htons(++Port);
        if (Port > maxPort)
        {
          Console->Print(RED, BLACK,"No more free UDP port in configured range");
          return NULL;
        }
      }
      else
      {  
        perror("udp-bind");
        return NULL;
      }
    }

    FD_SET(udpSockfd, &m_MainSetUDP); // add to master UDP set
    FD_SET(udpSockfd, &m_MainSetGlobal); // add to master global set
    if (udpSockfd > m_FdMaxUDP) // keep track of the maximum
    {
        m_FdMaxUDP = udpSockfd;
    }
            
    ConnectionUDP* udpConn = new ConnectionUDP(udpSockfd, Port, remoteadress, remoteport, this);

	  //m_LastUDPPort++;

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
    FD_CLR(sockfd, &m_MainSetUDP);
    FD_CLR(sockfd, &m_MainSetGlobal);
}

void ServerSocket::closeServer()
{
    close(m_ListenerTCP);
}
