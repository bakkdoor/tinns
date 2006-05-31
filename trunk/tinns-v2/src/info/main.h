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

#ifndef MAIN_H
#define MAIN_H

#include "../version.h"

#define INFO_PORT 7000
#define MAX_INFO_CLIENTS 50

//basic includes
#include <cstdio>
#include <ctime>
#include <cstring>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <list>
#include <unistd.h>
#include <iostream>
#include <stdarg.h>
#include <string.h>
#include <zlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <stdlib.h>
#include <csignal>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//tinns includes
#include "types.h"

// MySQL Support
#include "mysql.h"
#include "sql.h"
#include "../netcode/connection-tcp.h"
#include "../netcode/connection-udp.h"
#include "../netcode/serversocket.h"
#include "console.h"
#include "config.h"
#include "accounts.h"
#include "client.h"
#include "server.h"
#include "infoserver.h"
#include "misc.h"
#include "globals.h"


using namespace std;

#endif
