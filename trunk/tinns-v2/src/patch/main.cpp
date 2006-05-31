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

#include "main.h"

// for handling strg-c signal to shutdown in correct way
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        Shutdown();
    }
    else
    {
        psignal(signal, "Unkown signal: ");
    }
}

int main()
{
    // Connect signal with handlerfunction
    signal(SIGINT, signal_handler);


	if(!InitTinNS())
		while(1)
			sleep(1);

	PatchServer->Start();
	Console->Print("Patchserver is now %s. Waiting for clients...", Console->ColorText(GREEN, BLACK, "Online"));

	while(1)
	{
	    ServerSock->update();
	    Server->Update();
		PatchServer->Update();
		sched_yield();
	}


	return 0;
}

