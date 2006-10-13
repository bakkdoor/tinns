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
  isc.h

  MODIFIED: Unknown date / Namikon
  REASON: - initial release by Namikon
	MODIFIED: 13 Oct 2006 Hammag
	REASON: - Implemented MySQL isc method

*/

#ifndef ISC_H
#define ISC_H

class PISC
{
	private:
        int isc_method;
        
        bool use_isc;
        char isc_pwd;
        
        bool use_mysql;
        std::time_t mysql_update_intervall;
        std::time_t mysql_delayed_update_intervall; 
        std::time_t mysql_last_update_time;
        int mysql_last_client_count;
        std::time_t mysql_last_count_decrease_time;
        
        void Start_isc();
        void Update_isc();
        void Shutdown_isc();
              
        void Start_mysql();
        void Update_mysql();
        bool do_mysql_db_update(int players, bool shutdown = false);
        void Shutdown_mysql();
        
	public:
		PISC();
		~PISC();

		void Start();
		void Update();
		void Shutdown();
};

#endif
