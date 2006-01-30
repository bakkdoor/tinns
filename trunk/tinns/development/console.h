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
	console.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
    MODIFIED: 06 Jan 2006 Namikon
    REASON: - Added Print() function for colored console output
            - Added ColorText() to give selectable parts of an output another color
            - Added LPrint() to print like eAthena does
              Dont forget to use LClose() after using LPrint :)
*/
#ifndef CONSOLE_H
#define CONSOLE_H

enum COLORS
{
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE
};

class PConsole
{
	private :

		std::ofstream mLogFile;
		std::time_t mLastLogTime;
	public :
		PConsole();
		~PConsole();
		void Print(const char *Fmt_, ...);
		void Print(COLORS foreground, COLORS background, const char *Fmt_, ...);
		char *ColorText(COLORS foreground, COLORS background, const char *Fmt, ...);

		void LPrint(const char *Fmt_, ...);
		void LPrint(COLORS foreground, COLORS background, const char *Fmt_, ...);
		void LClose();

		void Update();
};

#endif

