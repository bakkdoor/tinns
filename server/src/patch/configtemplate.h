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
    Configuration template for patchserver
    Used to set available/optional/required options when loading config
      with a PConfig object.
*/

const char* PatchConfigTemplate[][2] = {
  // {option_name, default_value} if default_value is empty string, it means option is mandatory
  // List ends with empty string for option_name
  {"server_version", "200"},
  {"patchserver_port", "8040"},
  {"patches_path", "./patches"},
  {"file_path", "./files"},
  {"max_file_xfers", "5"},
  {"patch_packet_size", "512"},
  {"maxclients", "5"},
  {"gm_slots", "2"},
  
  {"", ""} // do not change this line (end mark)
};
