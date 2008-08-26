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
	def_appplaces.h

	Created: 21 Sep 2006 Hammag
	REASON: -
*/

#ifndef DEF_APPPLACES_H
#define DEF_APPPLACES_H


class PDefAppPlace
{
	private :
		int mIndex;
		std::string mName;
		int mExitWorldID;
		int mExitWorldEntity;
		int mSewerLevel;
		
	public :
		PDefAppPlace();
		~PDefAppPlace();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline int GetExitWorldID() const { return mExitWorldID; }
		inline int GetExitWorldEntity() const { return mExitWorldEntity; }
		inline int GetSewerLevel() const { return mSewerLevel; }

};

#endif
