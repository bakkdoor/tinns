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
	def_characters.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
	
	MODIFIED: 29 Jul 2006 Hammag
	REASON:  - Added skin modifiers fields
*/

#ifndef DEF_CHARACTERS_H
#define DEF_CHARACTERS_H

#include "def.h"

class PDefCharacter : public PDef
{
	private :
		//int mIndex;
		std::string mName;
		int mModel;
		int mHead;
		int mTorso;
		int mLegs;
		int mColor;
		int mBrightness;
		
	public :
		PDefCharacter();
		//~PDefCharacter();

		bool LoadFromDef(PTokenList *Tokens);

		inline const std::string &GetName() const { return mName; }
		inline int GetModel() const { return mModel; }
		inline int GetHead() const { return mHead; }
		inline int GetTorso() const { return mTorso; }
		inline int GetLegs() const { return mLegs; }
		inline int GetColor() const { return mColor; }
		inline int GetBrightness() const { return mBrightness; }
};

#endif

