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
	Zoning - makes changing of maps possible ingame

	Authors:
	- initial release by unknown person
	- Sting
	- Akiko <akiko@gmx.org>
	- Namikon

	MODIFIED: 23 August 2005 by Sting
        REASON: - TO match up with defs/pak_worldinfo.def from patch 200

	MODIFIED: 27 Aug 2005 by Akiko
	REASON: - reformated for better reading
		- thinking about a way to optimize the case statements
	MODIFIED: 28 Aug 2005 by Akiko
	REASON: - continued reformating
	MODIFIED: 26 Oct 2005 Akiko
	REASON: - fixed a bug -> loc is a int, and the location is a 16 bit value
		- added GPL
	MODIFIED: 27 Nov 2005 Akiko
	REASON: - fixed a very stupid bug (loc)
	MODIFIED: 04 Dec 2005 Akiko
	REASON: - copying int to char array (loc) correctly now
	MODIFIED: 07 Jan 2006 Namikon
	REASON: - Fixed zoning to / from wastelands to / from city sectors (packet1 was wrong, thanks Maxx!!)
	MODIFIED: 08 Jan 2006 Namikon
	REASON: - Added appartment handling
		- Fixed minimap
	MODIFIED: 17 Jan 2006 Namikon
	REASON: - File rewritten. Now, only 1 packet is send, like the real servers (that one fixed subway)
		- Fixed several worldnames
	MODIFIED: 26 Jul 2006 Hammag
	REASON: - Fixed world 1086 (area mc5) worldname (from NeoX source)

	MODIFIED: 28 Sep 2006 Hammag
	REASON: - Zone filename in now taken from appartments.def (for app zoning) or from worlds.ini (general case)
		    rather than hardcoded.

	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for new buildsystem

	TODO: Put get the fallback world from config
	      Check for file existence before sending info to client to avoid client crash and bad location in char info
	      
*/


#include "main.h"

#include "include/worlds.h"
#include "include/appartements.h"
#include "include/msgbuilder.h"


void SendZone(PClient *Client, u32 loc)
{
	//TODO : FIX case for worldroute and 5 missing id 505
  std::string worldName;
	ConnectionTCP *Socket = Client->getTCPConn();
	//PChar *Char = Chars->GetChar(Client->GetCharID());

	Socket->SetTimeOutValue(0xffff);
  
  PWorld* CurrentWorld = Worlds->GetWorld(loc);
  if (CurrentWorld)
  {
    worldName = CurrentWorld->GetBspName();
    if (worldName.empty())
    {
      worldName = "plaza/plaza_p1"; // Should be a config entry
      Console->Print("Client %d: Empty name for world %d. Redirecting to %s", Console->ColorText(RED, BLACK, "Warning"), Client->GetID(), loc, worldName.c_str());
      loc = 1;
    }
  }
  else
  {
    worldName = "plaza/plaza_p1"; // Should be a config entry
    Console->Print("Client %d: Invalid or not loaded world %d. Redirecting to %s", Console->ColorText(YELLOW, BLACK, "Warning"), Client->GetID(), loc, worldName.c_str());
    loc = 1;
  }
  
  PMessage* cMsg = MsgBuilder->BuildSendZoneTCPMsg(loc, &worldName);
	Client->SendTCPMessage(cMsg);
}
