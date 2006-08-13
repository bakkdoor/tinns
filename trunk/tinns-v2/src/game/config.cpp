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
        config.cpp

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown

        MODIFIED: 23 Dec 2005 Namikon
        REASON: - Added GPL
        MODIFIED: 07 Jan 2006 Namikon
        REASON: - Started to replace XML with CFG files
        
        MODIFIED: 05 Aug 2006 Hammag
        REASON: - changed LoadOptions() implementation.
                  This should make addition of new options really easier, as well as config syntax error tracking
                  The list of valid config options is now set in the array ConfigTemplate
                  A default value can be set for each option, whiches makes the option optionnal in config file
                  If no default value is set, the option is mandatory.
                  Duplicate option entries in config file are also checked, and only the first value is kept
                  Unkown options are rejected
                  Duplicates, unkown and default use generate a warning in logs but don't break options loading
                  Missing mandatory option generate an error in log and break option loading (imediate return false)
                  
        TODO: make ConfigTemplate[][2] a parameter of PConfig::LoadOptions() so that the PConfig class can be shared among all servers
*/

#include "main.h"

char* ConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"info_sql_host", "127.0.0.1"},
  {"info_sql_port", "3306"},
  {"info_sql_username", ""},
  {"info_sql_password", ""},
  {"info_sql_database", "infoserver"},
  {"game_sql_host", "127.0.0.1"},
  {"game_sql_port", "3306"},
  {"gameserver_udpport_min", "5000"},
  {"gameserver_udpport_max", "5099"},
  {"game_sql_username", ""},
  {"game_sql_password", ""},
  {"game_sql_database", "gameserver"},
  {"server_name", "TinNS"},
  {"server_ip", "127.0.0.1"}, //IP address used with clients reached without NAT (see localnet)
  {"no_nat_net", "0"}, // 0 is for "don't bother with NAT", else format is like 192.168.1. (with trailing dot)
  {"server_nat_ip", "127.0.0.1"}, //IP address used with clients reached through NAT 
  {"gameserver_port", "12000"},
  {"server_version", ""},
  {"maxclients", "128"},
  {"gm_slots", "10"},
  {"defs_path", "./defs"},
  {"isc_method", "0"},
  {"isc_update_intervall", "10"},
  {"isc_infoserverip", "127.0.0.1"},
  {"isc_infoserverport", "9991"},
  {"isc_connect_pw", "change_me"}, // make default value empty when isc ready
  
// For futur use:
//  {"max_chars_per_account", "4"},
//  {"multiple_logins_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody
//  {"multiple_ingames_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody (needs multiple logins ...)

//  {"local_chat_range", "500"},

//  {"account_name_regex", "[a-zA-Z0-9_-]{3-20}"}, // for account name sytnax control. Better be used in infoserver
//  {"char_name_regex", "[a-zA-Z]\\([ -][a-zA-Z]\\)?\\([ -][a-zA-Z]\\)?"}, // for character name sytnax control. Default is "human-like" names in 3 parts max

  {"", ""} // do not change this line (end mark)
};

PConfig::PConfig()
{
}

PConfig::~PConfig()
{
}

bool PConfig::LoadOptions()
{
	FILE *ConfigFile;
	char line[255];
	char *Opt, *Val;
	//int numOptions = 0;
	int i;
	bool Found;
	bool FatalError;
  
	ConfigFile = fopen("./conf/gameserver.conf","r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file \"gameserver.conf\"", Console->ColorText(RED, BLACK, "[Error]"));
        return false;
	}

	Console->Print("Loading configuration file");

	while(!feof(ConfigFile))
	{
	    fgets(line, 255, ConfigFile);
	    //Console->Print("LINE: [%s]", line);
	    Opt = strtok(line, "=");
	    Val = strtok(NULL, "\n\r");

      if(Opt && Val)
      {

      if (line[0] == '/' && line[1] == '/')
          continue;

        //  Console->Print("[%s] [%s]", Opt, Val);

      Trim(Opt);
      Trim(Val);

          //Console->Print("[%s] [%s]", Opt, Val);
      }
      else
      {
          continue;
      }

      i = 0;
      Found = false;      
      while(!Found && (ConfigTemplate[i][0][0] != '\0'))
      {
        if(!strcmp(Opt, ConfigTemplate[i++][0]))
        {
          Found = true;
          break;
        }
      }
/*
	    if(!strcmp(Opt, "info_sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "info_sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_host"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_port"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_username"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_password"))
            numOptions++;
	    else if(!strcmp(Opt, "game_sql_database"))
            numOptions++;
	    else if(!strcmp(Opt, "server_name"))
            numOptions++;
	    else if(!strcmp(Opt, "server_ip"))  // Siehe config file. Evtl weg
            numOptions++;
	    else if(!strcmp(Opt, "gameserver_port"))
            numOptions++;
	    else if(!strcmp(Opt, "server_version"))
            numOptions++;
	    else if(!strcmp(Opt, "maxclients"))
            numOptions++;
	    else if(!strcmp(Opt, "gm_slots"))
            numOptions++;
	    else if(!strcmp(Opt, "defs_path"))
            numOptions++;
	    else if(!strcmp(Opt, "isc_method"))
            numOptions++;
	    else if(!strcmp(Opt, "isc_update_intervall"))
            numOptions++;
	    else if(!strcmp(Opt, "isc_infoserverip"))
            numOptions++;
	    else if(!strcmp(Opt, "isc_infoserverport"))
            numOptions++;
	    else if(!strcmp(Opt, "isc_connect_pw"))
            numOptions++;
*/
	    if(Opt && Val)
	    {
	        //Console->Print("#%d [%s] [%s]", numOptions, Opt, Val);
	        if (Found)
	        {
	          if (GetOption(Opt) != "")
	          {
	            Console->Print("%s trying to set option %s more than once (new value %s ignored)", Console->ColorText(YELLOW, BLACK, "[Warning]"), Opt, Val);
	          }
	          else
	          {
	            //Console->Print(GREEN, BLACK, "New option %s set to %s", Opt, Val);
              mOptions.insert(std::make_pair(Opt, Val));
            }
          }
          else
          {
             Console->Print("%s option %s unknow and ignored (value %s)", Console->ColorText(YELLOW, BLACK, "[Warning]"), Opt, Val);
          }
	    }
	}

  // Now check for completeness and set default values when needed and available
  FatalError = false;
  i = 0;      
  while(ConfigTemplate[i][0][0] != '\0')
  {
    if (GetOption(ConfigTemplate[i][0]) == "")
    {
      if(ConfigTemplate[i][1][0] == '\0')
      {
        Console->Print("%s required option %s missing from config file", Console->ColorText(RED, BLACK, "[Error]"), ConfigTemplate[i][0]);
        FatalError = true;
      }
      else
      {
        Console->Print("[Info] using default value %s for option %s", ConfigTemplate[i][1], ConfigTemplate[i][0]);
        mOptions.insert(std::make_pair(ConfigTemplate[i][0], ConfigTemplate[i][1]));
      }
    }
    ++i;
  }

  if (FatalError)
    Console->Print(RED, BLACK, "[Error] Fatal errors found in configuration file");
	else
    Console->Print("%s Configuration file loaded", Console->ColorText(GREEN, BLACK, "[Success]") );
	
	return (!FatalError);
	
	/*  
	if(numOptions == 22) //corrected from 23 to match config parameters
	{
	    Console->LPrint(GREEN, BLACK, "Success");
	    Console->LClose();
        return true;
	}
    else // Namikon: I dont really like this way, there should be an detailed error output
    {
        Console->LPrint(YELLOW, BLACK, "Warning");
        Console->LClose();
        Console->Print("Configuration file is incomplete! %d settings found, 23 expected. Consider recreation from .example file", numOptions);
        return false;
    }
  */
}

const std::string &PConfig::GetOption(const char *Name) const
{
	static std::string NullString = "";
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return i->second;

	return NullString;
}

int PConfig::GetOptionInt(const char *Name) const
{
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return std::atoi(i->second.c_str());

	return 0;
}
