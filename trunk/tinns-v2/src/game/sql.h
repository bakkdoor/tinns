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
	sql.h - handles all mysql interactions

	Authors:
	- Namikon
	- bakkdoor

        MODIFIED: 24 Dec 2005 Namikon
        REASON: - initial release by Namikon
        MODIFIED: 26 Dec 2005 Namikon
        REASON: - Added GetWorldDoorType() for door interaction
                - Added GetWorldItemType() for item interaction
                - Added FmtTxt() for printf() like parsing of text
                - Added GetWorldItemOption() for item interaction
        MODIFIED: 30 Dec 2005 bakkdoor
        REASON: - changed mysql_init() parameter to null to prevent segfault
                - added checking for dbHandle to prevent segfault
*/

#ifndef MYSQL_H
#define MYSQL_H

class PMySQL
{
    private:
        int info_port;
        char info_host[100];
    	char info_userName[100];
        char info_password[100];
        char info_database[100];
        MYSQL *info_dbHandle;

        int game_port;
        char game_host[100];
    	char game_userName[100];
        char game_password[100];
        char game_database[100];
        MYSQL *game_dbHandle;

        int GameDBInuse;
        int InfoDBInuse;

    public:
        PMySQL();
        ~PMySQL();

        inline MYSQL *GetInfoHandle() { return info_dbHandle; };
        inline MYSQL *GetGameHandle() { return game_dbHandle; };

        bool Connect();

        int InfoQuery(const char *query);
        MYSQL_RES *InfoResQuery(const char *query);
        int GameQuery(const char *query);
        MYSQL_RES *GameResQuery(const char *query);

        int GetWorldItemType(unsigned short ID, int Location);
        int GetWorldItemOption(unsigned short ID, int Location, int option);
        int GetWorldDoorType(unsigned int ID, int Location);
        int GetAptID(unsigned int AptLoc, const u8 *pass);
        int GetAptType(int AptID);
        int GetAptOwner(int loc);
        void ShowInfoSQLError();
        void ShowGameSQLError();
        void FreeGameSQLResult(MYSQL_RES *res);
        void FreeInfoSQLResult(MYSQL_RES *res);
};
#endif
