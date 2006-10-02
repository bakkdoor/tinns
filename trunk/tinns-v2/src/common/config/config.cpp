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
                  See config.h for info
        MODIFIED: 27 Aug 2006 Hammag
        REASON: - Modified LoadOption() methode to make it generic,
                    with an options template and the config file as arguments
                - Removed the ConfigTemplate that was used for gameserver only.
                - Removed old unused code
*/

#include "main.h"



PConfig::PConfig()
{
}

PConfig::~PConfig()
{
}

bool PConfig::LoadOptions(const char* nConfigTemplate[][2], const char* nConfigFile)
{
	FILE *ConfigFile;
	char line[255];
	char *Opt, *Val;
	//int numOptions = 0;
	int i;
	bool Found;
	bool FatalError;
  
	ConfigFile = fopen(nConfigFile,"r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file \"%s\"", Console->ColorText(RED, BLACK, "[Error]"), nConfigFile);
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
      while(!Found && (nConfigTemplate[i][0][0] != '\0'))
      {
        if(!strcmp(Opt, nConfigTemplate[i++][0]))
        {
          Found = true;
          break;
        }
      }

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
  while(nConfigTemplate[i][0][0] != '\0')
  {
    if (GetOption(nConfigTemplate[i][0]) == "")
    {
      if(nConfigTemplate[i][1][0] == '\0')
      {
        Console->Print("%s required option %s missing from config file", Console->ColorText(RED, BLACK, "[Error]"), nConfigTemplate[i][0]);
        FatalError = true;
      }
      else
      {
        Console->Print("%s using default value %s for option %s", Console->ColorText(GREEN, BLACK, "[Info]"), nConfigTemplate[i][1], nConfigTemplate[i][0]);
        mOptions.insert(std::make_pair(nConfigTemplate[i][0], nConfigTemplate[i][1]));
      }
    }
    ++i;
  }

  if (FatalError)
    Console->Print(RED, BLACK, "[Error] Fatal errors found in configuration file");
	else
    Console->Print("%s Configuration file loaded", Console->ColorText(GREEN, BLACK, "[Success]") );
	
	return (!FatalError);
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
