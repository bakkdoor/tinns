#pragma once

#include <iostream>
#include <map>
#include "common/regex++.h"

class PConfig
{
	private :
		typedef std::map<std::string, std::string> OptionsMap;
		OptionsMap mOptions;
		RegEx* mOptValRegEx;
		RegEx* mIncludeRegEx;
		
		bool LoadOptions(const char* nConfigTemplate[][2], const char* nConfigFile, int nDepth);
		
	public :
		PConfig();
		~PConfig();

		inline bool LoadOptions(const char* nConfigTemplate[][2], const char* nConfigFile)
		  { return LoadOptions(nConfigTemplate, nConfigFile, 0); }
		inline const std::string &GetOption(const char *Name) const { return GetOption((std::string) Name); }
		const std::string &GetOption(const std::string Name) const;
		int GetOptionInt(const char *Name) const { return GetOptionInt((std::string) Name); }
		int GetOptionInt(const std::string Name) const;
};

// Max nested includes
#define CONFIG_MAXDEPTH 4

/*
  The list of valid config options is now set in the array ConfigTemplate
  A default value can be set for each option, whiches makes the option optionnal in config file
  If no default value is set, the option is mandatory.
  Duplicate option entries in config file are also checked, and only the first value is kept
  Unkown options are rejected
  Duplicates, unkown and default use generate a warning in logs but don't break options loading
  Missing mandatory option generate an error in log and break option loading (imediate return false)
  
  The ConfigTemplate parameter must have the structure shown in the following exemple:

const char* ConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"info_sql_host", "127.0.0.1"},
  {"info_sql_port", "3306"},
  {"info_sql_username", ""},
  {"info_sql_password", ""},
  {"", ""} // do not change this line (end mark)
};

*/

extern class PConfig* Config;
