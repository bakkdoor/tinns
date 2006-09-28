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
	def_respawn.h

	MODIFIED: 22 Sep 2006 Hammag
	REASON: - Creation
	
*/

#ifndef DEF_RESPAWN_H
#define DEF_RESPAWN_H

class PDefRespawn
{
	private :
		int mIndex;
		int mWorldID;
		int mEntityID; // Station ID
		int mHazardLevel;
		std::string mName; // Description
		std::string mFlag; // ???

	public :
		PDefRespawn();
		~PDefRespawn();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline int GetWorldID() const { return mWorldID; }
		inline int GetEntityID() const { return mEntityID; }
		inline int GetHazardLevel() const { return mHazardLevel; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetFlag() const { return mFlag; }

};

#endif
