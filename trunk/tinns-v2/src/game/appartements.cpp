/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  appartements.cpp - appartements class

	MODIFIED: 20 Nov 2006 Hammag
	REASON: - creation
	
*/

#include "main.h"

#include "appartements.h"
#include "gamedefs.h"
#include "def_appartements.h"

PAppartements::PAppartements()
{
}

//PAppartements::~PAppartements() {};

u32 PAppartements::CreateBaseAppartement(u32 nCharID, std::string nPassword, u8 nFactionID)
{
  typedef std::vector< std::pair<int, int> > PAppVector; //<AppType, AppPlace>
    
  PAppVector CandidateApts;  
  CandidateApts.reserve(32);
  u16 j;
  int AppType;
  
  for (PDefAppartementMap::const_iterator i=GameDefs->GetAppartementDefsConstIteratorBegin(); i!=GameDefs->GetAppartementDefsConstIteratorEnd(); i++)
  {
    if (i->second->GetFaction() == nFactionID)
    {
      for (j = 0; j < i->second->GetPlaceCount(); j++)
      {
        AppType = i->second->GetID();
        if ((AppType < 220) || (AppType >= 250))
        {
          CandidateApts.push_back(std::make_pair(AppType,i->second->GetPlace(j)));
        }
      }
    }
  }
  
  if(CandidateApts.size())
  {
    j = GameServer->GetRandom(CandidateApts.size(),1);
Console->Print("Apt n° %d chosen in %d for faction %d", j, CandidateApts.size(), nFactionID);
    
    char query[256];
    snprintf(query, 256, "INSERT INTO apartments (apt_id,apt_location,apt_type,apt_password, apt_owner) VALUES (NULL,'%d','%d','%s','%u');", CandidateApts[j].second, CandidateApts[j].first, nPassword.c_str(), nCharID);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "PAppartements::CreateBaseAppartement could not add some appartement entry in the database");
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
        return 0;
    }
    return (MySQL->GetLastGameInsertId());
  }
  else
    return 0;
}

void PAppartements::DeleteCharAppartements(u32 nCharID)
{
    char query[256];
    snprintf(query, 256, "DELETE FROM apartments WHERE (apt_owner='%u');", nCharID);
    if ( MySQL->GameQuery(query) )
    {
        Console->Print(RED, BLACK, "PAppartements::DeleteCharAppartements could not delete some char %d appartement(s) entry from the database", nCharID);
        Console->Print("Query was:");
        Console->Print("%s", query);
        MySQL->ShowGameSQLError();
    }
    else
if (gDevDebug) Console->Print(YELLOW, BLACK, "PAppartements::DeleteCharAppartements does not delete appartements content yet");      
}
