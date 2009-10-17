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
  {"item_price", "70"},
  {"require_validation", "0"},
  {"minlevel", "0"},
  {"username_filter", "^[a-z][\\w\\-]{2,14}$"},
  {"password_filter", "^[[:graph:]]{3,15}$"},
  {"charname_filter", "^[a-z]+([\\-\\ ]?[a-z]+){0,2}$"},
  {"clanname_filter", "^[a-z][\\w\\-\\ ]{2,14}$"},

// For futur use:
//  {"max_chars_per_account", "4"},
//  {"multiple_logins_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody
//  {"multiple_ingames_per_account", "0"}, // 0=nobody, 1=gm+ only, 2=everybody (needs multiple logins ...)

//  {"local_chat_range", "500"},

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

    {"weather", "50"},     // Set weather in a zone
    {"setmainskill", "50"},     // Set mainskill (INT,PSI,DEX,CON,STR) of own char or someone else
    {"setsubskill", "50"},     // Set subskill (BRT,HCK,PPU,...) of own char or someone else
    {"npc", "50"},     // do actions with NPCs


    {"", ""} // do not change this line (end mark)
};
