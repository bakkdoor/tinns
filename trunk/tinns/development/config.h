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
*/

#ifndef CONFIG_H
#define CONFIG_H

#ifdef _MSC_VER
	#pragma once
#endif


class PConfig
{
	private :
		typedef std::map<std::string, std::string> OptionsMap;
		OptionsMap mOptions;
	public :
		PConfig();
		~PConfig();

		bool LoadOptions();
		bool LoadOptions1();
		const std::string &GetOption(const char *Name) const;
		int GetOptionInt(const char *Name) const;
};

#endif

