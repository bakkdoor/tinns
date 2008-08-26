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
        MODIFIED: 25 Jun 2007 Hammag
        REASON: - Added include support
                - Now use PCRE RegEx instead of "strtok", enabling rentrance and removing
                  potential issues.
                - Added GetOption & GetOptionInt with const std::string parameter
                - Fixed a bug in EOF detection in the main file reading loop
*/

#include "main.h"


PConfig::PConfig()
{
  mOptValRegEx = new RegEx("^\\s*([[:graph:]]+)\\s*=\\s*(.+?)\\s*$", PCRE_CASELESS);
  mIncludeRegEx = new RegEx("^\\s*include\\s+([[:graph:]]+)\\s*$", PCRE_CASELESS);
}

PConfig::~PConfig()
{
  delete mOptValRegEx;
  delete mIncludeRegEx;
}

bool PConfig::LoadOptions(const char* nConfigTemplate[][2], const char* nConfigFile, int nDepth)
{
	FILE *ConfigFile;
	char line[255];
	std::string Opt, Val;
	//int numOptions = 0;
	int i;
	bool Found;
	bool NoError = true;
  
	ConfigFile = fopen(nConfigFile,"r");

	if(!ConfigFile)
	{
	    Console->Print("%s Cant open file \"%s\"", Console->ColorText(RED, BLACK, "[Error]"), nConfigFile);
        return false;
	}

  if(nDepth)
    Console->Print("%s Including file %s (nesting level %d)", Console->ColorText(GREEN, BLACK, "[Info]"), nConfigFile, nDepth);
  else
	  Console->Print("%s Loading configuration file %s", Console->ColorText(GREEN, BLACK, "[Info]"), nConfigFile);
 
	while(!feof(ConfigFile) && fgets(line, 255, ConfigFile))
	{
	    //Console->Print("LINE: '%s'", line);
	    if (line[0] == '/' && line[1] == '/')
        continue;
	    
	    if(mOptValRegEx->Search(line))
      {
        Opt = mOptValRegEx->Match(1);
        Val = mOptValRegEx->Match(2);
        //Console->Print("'%s'='%s'", Opt.c_str(), Val.c_str());

        Trim(&Opt);
        Trim(&Val);
        //Console->Print("'%s'='%s'", Opt.c_str(), Val.c_str());
      }
      else if(mIncludeRegEx->Search(line))
      {
        if(nDepth < CONFIG_MAXDEPTH)
        {
          std::string TargetName = mIncludeRegEx->Match(1);
          std::string StartName = nConfigFile;
          int BasenamePos = StartName.rfind("/");
          if(BasenamePos > 0)
            TargetName = StartName.substr(0, BasenamePos) + "/" + TargetName;
         
          NoError = LoadOptions(nConfigTemplate, TargetName.c_str(), nDepth+1) && NoError;
          continue;
        }
        else
        {
          Console->Print("%s Max nesting level (%d) reached. Can't include configuration file %s", Console->ColorText(RED, BLACK, "[Error]"), CONFIG_MAXDEPTH,mIncludeRegEx->Match(1));
          NoError = false;
        }
      }
      else
      {
          continue;
      }

      i = 0;
      Found = false;      
      while(!Found && (nConfigTemplate[i][0][0] != '\0'))
      {
        if(!strcmp(Opt.c_str(), nConfigTemplate[i++][0]))
        {
          Found = true;
          break;
        }
      }

	    if(!Opt.empty() && !Val.empty())
	    {
	        //Console->Print("#%d [%s] [%s]", numOptions, Opt.c_str(), Val.c_str());
	        if (Found)
	        {
	          if (GetOption(Opt) != "")
	          {
	            Console->Print("%s trying to set option %s more than once (old value '%s' - new value '%s' ignored)", Console->ColorText(YELLOW, BLACK, "[Warning]"), Opt.c_str(), GetOption(Opt).c_str(), Val.c_str());
	          }
	          else
	          {
	            //Console->Print(GREEN, BLACK, "New option %s set to %s", Opt.c_str(), Val.c_str());
              mOptions.insert(std::make_pair(Opt, Val));
            }
          }
          else
          {
             Console->Print("%s option %s unknow and ignored (value %s)", Console->ColorText(YELLOW, BLACK, "[Warning]"), Opt.c_str(), Val.c_str());
          }
	    }
	}

  // Now check for completeness and set default values when needed and available
  if(!nDepth) // Check only if we're at top level (ie. not in an included file)
  {
    i = 0;      
    while(nConfigTemplate[i][0][0] != '\0')
    {
      if (GetOption(nConfigTemplate[i][0]) == "")
      {
        if(nConfigTemplate[i][1][0] == '\0')
        {
          Console->Print("%s required option %s missing from config file", Console->ColorText(RED, BLACK, "[Error]"), nConfigTemplate[i][0]);
          NoError = false;
        }
        else
        {
          Console->Print("%s using default value %s for option %s", Console->ColorText(GREEN, BLACK, "[Info]"), nConfigTemplate[i][1], nConfigTemplate[i][0]);
          mOptions.insert(std::make_pair(nConfigTemplate[i][0], nConfigTemplate[i][1]));
        }
      }
      ++i;
    }
  }

  if (NoError)
  {
	  if(nDepth)
	    Console->Print("%s file included", Console->ColorText(GREEN, BLACK, "[Info]") );
	  else
      Console->Print("%s Configuration file loaded", Console->ColorText(GREEN, BLACK, "[Success]") );
  }
  else
    Console->Print(RED, BLACK, "[Error] Fatal errors found in configuration file");
	
	return (NoError);
}

const std::string &PConfig::GetOption(const std::string Name) const
{
	static std::string NullString = "";
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return i->second;

	return NullString;
}

int PConfig::GetOptionInt(const std::string Name) const
{
	OptionsMap::const_iterator i = mOptions.find(Name);
	if(i!=mOptions.end())
		return std::atoi(i->second.c_str());

	return 0;
}
/*  
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
*/
