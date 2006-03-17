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

#ifndef SERVERSOCKET_H
#define SERVERSOCKET_H

class Connection;

class ServerSocket
{
    private:
            fd_set              m_MainSetTCP; // master file descriptor list for tcp-connections
            fd_set              m_ReadSetTCP; // temp file descriptor list for select() for tcp-connections

            struct sockaddr_in  m_ServerAddr; // server address

            int                 m_ListenerTCP; // listening socket descriptor

            int                 m_FdMaxTCP; // highest current file-descriptor (tcp)
            int                 m_FdMaxUDP; // highest current file-descriptor (udp)

            bool                m_bNewTCPConnection;

            int                 m_LastUDPPort;

            struct timeval      m_TimeOut;

    public:
            ServerSocket();
            ~ServerSocket();

            bool                open(int port);
            void                update();
            bool                newConnection();
            ConnectionTCP*      getTCPConnection();
            ConnectionUDP*      getUDPConnection();

            bool                isDataAvailable(int sockfd);

            void                delSocketFromSet(int sockfd);

            void                closeServer();
};

#endif
