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

#ifndef SQL_H
#define SQL_H

#ifdef _MSC_VER
	#pragma once
#endif

class PMySQL
{
    private:
        int port;
        char host[100];
    	  char userName[100];
        char password[100];
        char database[100];
        MYSQL *dbHandle;
        std::time_t mKeepaliveDelay;
        std::time_t mLastKeepaliveSent;

    public:
        PMySQL();
        ~PMySQL();

        void Update();
        inline MYSQL *GetHandle() { return dbHandle; };

        bool Connect();
        int Query(const char *query);
        MYSQL_RES *ResQuery(const char *query);
        void ShowSQLError();
        void FreeSQLResult(MYSQL_RES *res);
        inline u32 GetLastInsertId() { return mysql_insert_id(dbHandle); };
};
#endif
