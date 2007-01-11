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
	terminal.h - Management class for Terminal actions (Citycom, keys, vehicledepot, ...)

	MODIFIED: 08 Jan 2007 Namikon
	REASON: - Created

*/

#ifndef TERMINAL_H
#define TERMINAL_H

class PTerminal
{
    private:
        void SendNewMailNotice(PClient* nClient, u8 nNewMails = 0);

    public:
        // Check accesslevel of Player for various Terminal actions
        bool CheckAccess(PClient* nClient, char *nArea);
        u8 GetNewEmailCount(PClient* nClient);
};

#endif
