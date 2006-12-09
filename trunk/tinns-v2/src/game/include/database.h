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
    database.h

	MODIFIED: 28 Sep 2005 Akiko
	REASON: - added this header
		- added GPL (is about time :D)
		- removed some of the Windows specific code
*/


#ifndef DATABASE_H
#define DATABASE_H


class PDatabase
{
	private :
		PAccounts *mAccounts;
		PChars *mChars;

	public :
		PDatabase();
		~PDatabase();

		void Init();

		inline PAccounts *GetAccounts() const {
			return(mAccounts);
		}
		inline PAccount *GetAccount(u32 AccountID) const {
			return(mAccounts->GetAccount(AccountID));
		}
		inline PAccount *GetAccount(const std::string &Name) const {
			return(mAccounts->GetAccount(Name));
		}

		inline PChars *GetChars() const {
			return(mChars);
		}
		inline PChar *GetChar(u32 CharID) const {
			return(mChars->GetChar(CharID));
		}
		inline PChar *GetChar(const std::string &Name) const {
			return(mChars->GetChar(Name));
		}
		//PChar *CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model);
		PChar *CreateChar(u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
      u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot);

		void Update();
		void Rehash();
};

#endif

