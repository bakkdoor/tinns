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
    MODIFIED: 27 Aug 2006 Hammag
    REASON: - Implemented shared Config class use and config template to load conf.
                Added gameserver configtemplate.h include,
                Added new required parameters to Config->LoadOptions()
            - Added AdditionnalConfigChecks() local function, called after config loading
                taken from inital infoserver's PConfig::VerifyValues
*/

#include "main.h"
#include "configtemplate.h"

#include "version.h"
const char ServerVersion[] = TINNS_INFO_VERSION;
const char SVNRevision[] = TINNS_SVN_REVISION;

ServerSocket* ServerSock = 0;
PMySQL *MySQL = 0;
PConsole *Console = 0;
PServer *Server = 0;
PConfig *Config = 0;
PInfoServer *InfoServer = 0;
//PAccounts *Accounts = 0;  // To be removed

bool Init()
{
	Console = new PConsole("log/infoserver.log"); // Make that from config file !!!
	Console->Print("Starting TinNS Infoserver");
	Console->Print(WHITE, BLUE, "/-------------------------------------------------------------------\\");
	Console->Print(WHITE, BLUE, "|               TinNS (TinNS is not a Neocron Server)               |");
  Console->Print(WHITE, BLUE, "|            Copyright (C) 2005 Linux Addicted Community            |");
	Console->Print(WHITE, BLUE, "|                  maintainer Akiko <akiko@gmx.org>                 |");
	Console->Print(WHITE, BLUE, "|             ==========================================            |");
	Console->Print(WHITE, BLUE, "|      Head coders:                   The packet analyzing team:    |");
	Console->Print(WHITE, BLUE, "|      - Akiko                         - MaxxJag                    |");
	Console->Print(WHITE, BLUE, "|      - bakkdoor                      - Sting                      |");
	Console->Print(WHITE, BLUE, "|      - Namikon                       - Balm                       |");
	Console->Print(WHITE, BLUE, "|      - Hammag                                                     |");
	Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
	Console->Print(WHITE, BLUE, "|  This project would'nt be at its current stage without the help   |");
  Console->Print(WHITE, BLUE, "|        from the NeoPolis team, special thanks to you guys!        |");
	Console->Print(WHITE, BLUE, "|-------------------------------------------------------------------|");
	Console->Print(WHITE, BLUE, "|  This project is under GPL, see any source file for more details  |");
	Console->Print(WHITE, BLUE, "\\-------------------------------------------------------------------/");

	//char svnrev[10];
	//GetSVNRev(svnrev);
	Console->LPrint("You are running TinNS Infoserver version");
	Console->LPrint(GREEN, BLACK, " %s", ServerVersion);
	Console->LPrint(WHITE, BLACK, " - SVN Rev");
	Console->LPrint(GREEN, BLACK, " %s", SVNRevision);
	Console->LClose();
	
	Config = new PConfig();
	if(!Config->LoadOptions(InfoConfigTemplate, "./conf/infoserver.conf"))
	    return false; //Shutdown();
  if(!AdditionnalConfigChecks())
    return false; //Shutdown();

	ServerSock = new ServerSocket();
	Server = new PServer();
	
	MySQL = new PMySQL();
    if(MySQL->Connect() == false)
	    return false; //Shutdown();

	InfoServer = new PInfoServer();
	//Accounts = new PAccounts(); // To be removed

	return true;
}

void Shutdown()
{
  if(Server) Server->Shutdown();
	if(InfoServer) delete InfoServer;
	if(MySQL) delete MySQL;
	if(Config) delete Config;
	if(Console) delete Console;
	if(ServerSock) delete ServerSock;
    exit(0);
}

bool AdditionnalConfigChecks()
{
  //NOTA: empty config values are never accepted by PConfig objects
  
    int numErr, numWarn;
    
    //std::string sqlhost = Config->GetOption("sql_host");
    std::string sqlusername = Config->GetOption("sql_username");
    //std::string sqlpassword = Config->GetOption("sql_password");
    //std::string global_sql_database = Config->GetOption("global_sql_database");
    int sqlport = Config->GetOptionInt("sql_port");
    int auto_accounts = Config->GetOptionInt("auto_accounts");
    int infoserver_port = Config->GetOptionInt("infoserver_port");
    int gameserver_livecheck = Config->GetOptionInt("gameserver_livecheck");
    int minlevel = Config->GetOptionInt("minlevel");
    int maxclients = Config->GetOptionInt("maxclients");

    numErr = numWarn = 0;

    /*if(strcmp(sqlhost.c_str(), "") == 0)
    {
        Console->Print("%s sql_host is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }*/

    /*if(strcmp(sqlusername.c_str(), "") == 0)
    {
        Console->Print("%s sql_username is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }
    else*/
      if(strcmp(sqlusername.c_str(), "root") ==0)
    {
        Console->Print("%s Config: sqlusername is \"root\", which is pretty insecure.", Console->ColorText(YELLOW, BLACK, "[Warning]"));
        numWarn++;
    }

    /*if(strcmp(sqlpassword.c_str(), "") == 0);
    {
        Console->Print("%s sql_password is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }

    if(strcmp(global_sql_database.c_str(), "") == 0);
    {
        Console->Print("%s global_sql_database is empty", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }*/

    if(sqlport < 1025 || sqlport > 65535)
    {
        Console->Print("%s Config: sql_port is usually between 1025 and 65535!", Console->ColorText(YELLOW, BLACK, "[Warning]"));
        numWarn++;
    }

    if(auto_accounts != 0 && auto_accounts != 1)
    {
        Console->Print("%s Config: auto_accounts has to be either 0 or 1!", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }

    if(infoserver_port < 1025 || infoserver_port > 65535)
    {
        Console->Print("%s Config: infoserver_port has to be between 1025 and 65535!", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }
    // This assertion is wrong, as sql server and info server can be on differents computers
    /*else if(infoserver_port == sqlport)
    {
        Console->Print("%s infoserver_port has to be different from sql_port!", Console->ColorText(RED, BLACK, "Config error:"));
        numErr++;
    }*/

    if(gameserver_livecheck <= 0)
    {
        Console->Print("%s Config: gameserver_livecheck cannot equal or less than 0", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }
    else if(gameserver_livecheck < 2)
    {
        Console->Print("%s Config: gameserver_livecheck is pretty low, this may cause increased mysql load", Console->ColorText(YELLOW, BLACK, "[Warning]"));
        numWarn++;
    }
    else if(gameserver_livecheck > 120 && gameserver_livecheck < 600)
    {
        Console->Print("%s Config: gameserver_livecheck is pretty high, this may cause offline servers shown as online and a wrong usercount", Console->ColorText(YELLOW, BLACK, "[Warning]"));
        numWarn++;
    }
    else if(gameserver_livecheck > 600)
    {
        Console->Print("%s Config: gameserver_livecheck is too high. Use a value between 0 and 600", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }

    if(minlevel < 0 || minlevel > 255)
    {
        Console->Print("%s Config: minlevel has to be between 0 and 255", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }
    
    if(maxclients < 1 )
    {
        Console->Print("%s Config: maxclients has to be higher or equal to 1", Console->ColorText(RED, BLACK, "[Error]"));
        numErr++;
    }
    
    if(numErr == 0 /*&& numWarn == 0*/)
        return true;
    else
        return false;
}


