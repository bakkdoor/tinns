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
	main.cpp - this is the main file with the main function

	MODIFIED: 12 Sep 2005 Akiko
	REASON: - removed some of the Windows specific code
		- replaced SleepEx method by the Linux equivalent
	MODIFIED: 26 Sep 2005 Akiko
	REASON:	- added GPL
*/

#include "tinns.h"

int main()
{
	if(!InitTinNS())
		while(1)
			sleep(1);
	
	RemoteConsole->Start();
	InfoServer->Start();
	PatchServer->Start();
	GameServer->Start();
	Console->Print("Waiting for clients");
	
	while(1)
	{
		Server->Update();
		Database->Update();
		PatchServer->Update();
		InfoServer->Update();
		GameServer->Update();
		Console->Update();
		RemoteConsole->Update();
		// in release mode, we just relinquish our remaining time slice to other processes
		//SleepEx(0, true);
		sched_yield();
	}
	
	ShutdownTinNS();

	return 0;
}

