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
    MODIFIED: 23 Dec 2005 bakkdoor
	  REASON: - Added <csignal> & signalHandler -> catch strg-c and shutdown nicely
    MODIFIED: 25 Dec 2005 Namikon
	  REASON: - Fixed shutdown procedure, wont cause segfault anymore
    MODIFIED: 06 Jan 2006 Namikon
    REASON: - Added color to console outputs    
    MODIFIED: 01 Jul 2006 Hammag
	  REASON: - commented out sched_yield() in main loop, as it is
	            not needed anymore with a right timeout for ReadSetTCP select
    MODIFIED: 26 Jul 2006 Hammag
	  REASON: - added memory leak check in the main loop for unreleased DB Ressources and messages          

*/

#include "main.h"

#include "worlddatatemplate.h" // temp
#include "worlds.h" // temp

#include "isc.h"

// for handling strg-c signal to shutdown in correct way
void signal_handler(int signal)
{
    if (signal == SIGINT)
    {
        //cout << "Shutting down TinNS" << endl;

        //exit(0);
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
	
	/*** Test ***/
	PWorldDataTemplate tmpWorld;
	std::string tWorldName = "terrain/j_12.dat";
  //std::string tWorldName = "./worlds/apps/plaza_app_1.dat";
  //std::string tWorldName = "./worlds/plaza/plaza_p1.dat";
  //std::string tWorldName = "./worlds/military/military_pstairs.dat";
	tmpWorld.LoadDatFile(tWorldName);
	/*** End Test ***/
  
  PWorlds tWorlds;
  tWorlds.LoadWorlds();
  //Filesystem->ClearCache(); // release memory if World templates preload activated, this cache that won't be used anymore
	
	//RemoteConsole->Start();
	GameServer->Start();
	//GameServer->SetGameTime(0);
	ISC->Start();
	
	Console->Print("Gameserver is now %s. Waiting for clients...", Console->ColorText(GREEN, BLACK, "Online"));

	while(1)
	{
	  ServerSock->update();
		Server->Update();
		Database->Update();
		GameServer->Update();
		PMessage::CheckMsgCount(); // Memory leak check
		MySQL->Update(); // Memory leak check	and MySQL keepalive
		ISC->Update();
		Console->Update();
		// in release mode, we just relinquish our remaining time slice to other processes
		//SleepEx(0, true);
		//sched_yield();
	}

	return 0;
}
