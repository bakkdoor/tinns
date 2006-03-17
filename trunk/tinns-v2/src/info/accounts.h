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

#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#ifdef _MSC_VER
	#pragma once
#endif
/*
0 = unregistered user
1 = Registered user
30 = volunteer
50 = GM
100 = Admin
*/
enum PAccountLevel
{
	PAL_BANNED = -1,
	PAL_UNREGPLAYER = 0,
	PAL_REGPLAYER = 1,
	PAL_VOLUNTEER = 30,
	PAL_GM = 50,
	PAL_ADMIN = 100
};

/*
0 = Offline
1 = Online
2 = Banned
*/

enum PAccountStatus
{
    PAS_OFFLINE = 0,
    PAS_ONLINE = 1,
    PAS_BANNED = 2
};

class PAccounts
{
	private :
        // SQL Layout
        enum {
            a_id,
            a_username,
            a_password,
            a_priv,
            a_status,
            a_bandate
        };

	public :
		PAccounts();
		~PAccounts();

		int Authenticate(const char *User, const u8* Password, int PassLen, const u8 *Key, int *accID);
		int GetAccesslevel(int AccountID);
        std::string GetBannedTime(const char *username);
};

#endif

