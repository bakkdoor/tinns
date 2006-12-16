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
    Configuration template for infoserver
    Used to set available/optional/required options when loading config
      with a PConfig object.
*/

const char* InfoConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"sql_host", "127.0.0.1"}, // should be renanmed to info_sql_host
  {"sql_port", "3306"}, // should be renanmed to info_sql_port
  {"sql_username", ""}, // should be renanmed to info_sql_username
  {"sql_password", ""}, // should be renanmed to info_sql_password
  {"global_sql_database", "infoserver"}, // should be renanmed to info_sql_database
  {"mysql_wait_timeout", "28800"}, // value of the wait_timout system variable from the MySQL server (same for game & info DB atm). 0 to disable keepalive.
  {"auto_accounts", "0"},
  {"infoserver_port", "7000"},
  {"gameserver_livecheck", "60"},
  {"minlevel", "0"},
  {"maxclients", "50"},
  {"require_validation", "0"},
// For futur use:
//  {"max_chars_per_account", "4"},
//  {"account_name_regex", "[a-zA-Z0-9_-]{3-20}"}, // for account name sytnax control. Better be used in infoserver
//  {"char_name_regex", "[a-zA-Z]\\([ -][a-zA-Z]\\)?\\([ -][a-zA-Z]\\)?"}, // for character name sytnax control. Default is "human-like" names in 3 parts max

  {"", ""} // do not change this line (end mark)
};
