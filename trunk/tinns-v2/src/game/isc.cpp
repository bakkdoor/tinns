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
  isc.cpp

  MODIFIED: Unknown date / Namikon
  REASON: - initial release by Namikon
	MODIFIED: 13 Oct 2006 Hammag
	REASON: - Implemented MySQL isc method

  TODO:
    - implement ISC protocol
*/

#include "main.h"

#include "isc.h"

PISC::PISC()
{
  use_isc = false;
  use_mysql = false;
}

PISC::~PISC()
{
}

void PISC::Start()
{
  isc_method = Config->GetOptionInt("isc_method");

  if(isc_method & 1)
  {
      Console->Print("%s Infoserver updating over MySQL is enabled", Console->ColorText(GREEN, BLACK, "[Info]"));
      use_mysql = true;
  }
  if(isc_method & 2)
  {
      Console->Print("%s Infoserver updating over ISC is enabled, update intervall: infoserver: %s", Console->ColorText(GREEN, BLACK, "[Info]"), "?");
      use_isc = true;
      Console->Print("%s Infoserver updating over ISC not implemented yet. DISABLED", Console->ColorText(YELLOW, BLACK, "[Notice]"));
      use_isc = false;
  }
  if(!(use_isc || use_mysql))
  {
      Console->Print("%s ISC and MySQL are both disabled, infoserver will NOT be updated", Console->ColorText(YELLOW, BLACK, "[Notice]"));
  }

  if (use_isc)
    Start_isc();
  
  if (use_mysql)
    Start_mysql();

}

void PISC::Update()
{
  if (use_isc)
    Update_isc();
  
  if (use_mysql)
    Update_mysql();     
}

void PISC::Shutdown()
{
  if (use_isc)
  {
    use_isc = false;
    Shutdown_isc();
  }
  
  if (use_mysql)
  {
    use_mysql = false;
    Shutdown_mysql();
  }
}

/*** ISC protocol mode specific ***/

void PISC::Start_isc()
{
  u16 Port = Config->GetOptionInt("isc_infoserverport");
	if (Port == 0)
	{
		Console->Print("%s Unable to start ISC, isc_infoserverport setting is invalid");
		use_isc = false;
		return;
	}
	Console->Print("Trying to connect to infoserver...", Port);
	// Add function for initial connect here and store the connectioninfo somewhere
}

void PISC::Update_isc()
{
  
}

void PISC::Shutdown_isc()
{
  
}


/*** IMySQL mode specific ***/

void PISC::Start_mysql()
{
  mysql_update_intervall = Config->GetOptionInt("isc_update_intervall");
  if (mysql_update_intervall < 10)
  {
    Console->Print("%s value for 'isc_update_intervall' too low (%d). Set to 10 sec.", Console->ColorText(YELLOW, BLACK, "[Notice]"), mysql_update_intervall);
    mysql_update_intervall = 10;
  }
  
  mysql_delayed_update_intervall = Config->GetOptionInt("isc_delayed_update_intervall");
  if (mysql_delayed_update_intervall < 0)
  {
    Console->Print("%s value for 'isc_delayed_update_intervall' is invalid (%d). Set to 0 sec (immediate update)", Console->ColorText(YELLOW, BLACK, "[Notice]"), mysql_delayed_update_intervall);
    mysql_delayed_update_intervall = 0;
  }
    
  mysql_last_update_time = 0;
  mysql_last_client_count = 0;
  mysql_last_count_decrease_time = 0;
  
  Update_mysql();
}

void PISC::Update_mysql()
{
  bool do_update = false;
  std::time_t t = std::time(NULL);
  int client_count = Server->GetNumClients();
  
  if (((t - mysql_last_update_time) >= mysql_update_intervall) || (client_count > mysql_last_client_count))
  {
    do_update = true;
//if(gDevDebug) Console->Print("%s ISC : time or more clients", Console->ColorText(GREEN, BLACK, "[Debug]"));
  }
  else if (client_count < mysql_last_client_count)
  {
    if(mysql_last_count_decrease_time && ((t - mysql_last_count_decrease_time) >= mysql_delayed_update_intervall))
    {
      do_update = true;
//if(gDevDebug) Console->Print("%s ISC : less clients timed out", Console->ColorText(GREEN, BLACK, "[Debug]"));
    }
    else if (!mysql_last_count_decrease_time)
    {
      mysql_last_count_decrease_time = t;
//if(gDevDebug) Console->Print("%s ISC : Starting less clients time count", Console->ColorText(GREEN, BLACK, "[Debug]"));
    }
  }
  else if (mysql_last_count_decrease_time && (client_count == mysql_last_client_count))
  {
    mysql_last_count_decrease_time = 0;
//if(gDevDebug) Console->Print("%s ISC : Canceling less clients time count", Console->ColorText(GREEN, BLACK, "[Debug]"));    
  }
  
  if (do_update)
  {
    do_mysql_db_update(client_count);
    mysql_last_client_count = client_count;
    mysql_last_count_decrease_time = 0;
    mysql_last_update_time = std::time(NULL);
  }
  
}

void PISC::Shutdown_mysql()
{
  use_mysql = false;
  do_mysql_db_update(0,true);
}

bool PISC::do_mysql_db_update(int players, bool shutdown)
{
  int server_id = 2; // temp
  char query[256];
  snprintf (query, 256, "UPDATE `server_list` SET `s_players` = '%d', `s_lastupdate` = %s WHERE `s_id` = %d LIMIT 1;", players, (shutdown ? "'0'" : "NOW()"), server_id);

//if(gDevDebug) Console->Print("%s Updating Infoserver over MySQL.", Console->ColorText(GREEN, BLACK, "[Debug]"));
  if(MySQL->InfoQuery(query))
  {
    use_mysql = false;
    Console->Print("%s Could not update Infoserver over MySQL. Updating DISABLED. Error was:", Console->ColorText(RED, BLACK, "[ERROR]"));
    MySQL->ShowInfoSQLError();
    return false;
  }
  else
    return true;
}
