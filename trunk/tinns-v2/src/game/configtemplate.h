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
    Configuration template for gameserver
    Used to set available/optional/required options when loading config
      with a PConfig object.

	MODIFIED: 28 Sep 2006 Hammag
	REASON: - added worlds_path entry
  MODIFIED: 02 Oct 2006 Hammag
	REASON: - added dev_debug entry

  TODO: put a single data_directory entry as the root directory for all NC data

*/

const char* GameConfigTemplate[][2] = {
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
  {"mysql_wait_timeout", "28800"}, // value of the wait_timout system variable from the MySQL server (same for game & info DB atm). 0 to disable keepalive.
  {"server_name", "TinNS"},
  {"server_ip", "127.0.0.1"}, //IP address used with clients reached without NAT (see localnet)
  {"no_nat_net", "0"}, // 0 is for "don't bother with NAT", else format is like 192.168.1. (with trailing dot)
  {"server_nat_ip", "127.0.0.1"}, //IP address used with clients reached through NAT
  {"gameserver_port", "12000"},
  {"server_version", ""},
  {"maxclients", "128"},
  {"gm_slots", "10"},
  //{"defs_path", "./defs"},
  //{"worlds_path", "./worlds"},
  {"nc_data_path", "."},
  {"isc_method", "1"},
  {"isc_server_id", ""},
  {"isc_update_intervall", "60"},
  {"isc_delayed_update_intervall", "6"}, // the purpose of this option is to avoid multiple infoDB update when client just disconnects/reconnects during login phases
  {"isc_infoserverip", "127.0.0.1"},
  {"isc_infoserverport", "9991"},
  {"isc_connect_pw", "change_me"}, // make default value empty when isc ready
  {"dev_debug", "0"}, // set to non-zero to activate development outputs
  {"auto_save_period", "300"},
  {"new_char_location", "1085"},
  {"broadcast_new", "0"},
  {"broadcast_new_hidestaff", "0"},
  {"max_cash", "20000000"},
  {"require_validation", "0"},

// For futur use:
//  {"max_chars_per_account", "4"},
//  {"multiple_logins_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody
//  {"multiple_ingames_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody (needs multiple logins ...)

//  {"local_chat_range", "500"},

//  {"account_name_regex", "[a-zA-Z0-9_-]{3-20}"}, // for account name sytnax control. Better be used in infoserver
//  {"char_name_regex", "[a-zA-Z]\\([ -][a-zA-Z]\\)?\\([ -][a-zA-Z]\\)?"}, // for character name sytnax control. Default is "human-like" names in 3 parts max

  {"", ""} // do not change this line (end mark)
};

const char* CommandsTemplate[][2] = {
    {"debug", "100"},
    {"settime", "100"},
    {"warp", "0"},
//    {"delworlditem", "100"},
//    {"addworlditem", "100"},
//    {"adddoor", "100"},
    {"online", "0"},
    {"sendchat", "50"},
    {"skin", "50"},
    {"effect", "0"},
    {"speed", "0"},
    {"color", "0"},
    {"brightness", "0"},
    {"remove", "50"},
    {"rehash", "100"},
    {"uptime", "0"},
    {"version", "0"},
    {"broadcast", "100"},
    {"kick", "50"},
    {"info", "50"},
    {"setlevel", "100"},
    {"warpto", "50"},
    {"recall", "50"},

    {"ban", "50"},          // Set ban
    {"unban", "50"},        // Remove ban
    {"listbans", "50"},     // Show list of all banned accounts
    {"shun", "50"},         // Quiet all chat
    {"unshun", "50"},       // UnQuiet all chat
    {"jail", "50"},         // "Jail" someone (Regants ^^)
    {"unjail", "50"},       // Move him out
    {"teleport", "50"},     // Warp targetplayer XX to zone YY
    {"givemoney", "50"},     // Warp targetplayer XX to zone YY
    {"takemoney", "50"},     // Warp targetplayer XX to zone YY
    {"spawnactor", "50"},     // Spawn given actor next to player

    {"", ""} // do not change this line (end mark)
};
