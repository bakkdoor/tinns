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
        config.h

        Authors:
        - Akiko
        - Namikon
        - someone else?

        MODIFIED: Unknown date / Unknown author
        REASON: - initial release by unknown
        MODIFIED: 23 Dec 2005 Namikon
        REASON: - Added GPL
        MODIFIED: 27 Aug 2006 Hammag
        REASON: - Modified LoadOption() methode to make it generic,
                    with a config template and the config file as arguments
                - Removed method LoadOptions1() that wasn't implemented        
        
*/

#ifndef CONFIG_H
#define CONFIG_H

class PConfig
{
	private :
		typedef std::map<std::string, std::string> OptionsMap;
		OptionsMap mOptions;
	public :
		PConfig();
		~PConfig();

		bool LoadOptions(const char* nConfigTemplate[][2], const char* nConfigFile);
		const std::string &GetOption(const char *Name) const;
		int GetOptionInt(const char *Name) const;
};


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
#endif

