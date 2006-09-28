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
	def_WorldModels.h

	Created: 21 Sep 2006 Hammag
	REASON: -
*/

#ifndef DEF_WorldModelS_H
#define DEF_WorldModelS_H


class PDefWorldModel
{
	private :
		int mIndex;
		std::string mName;
		int mUseFlags;
		int mFunctionType;
		int mFunctionValue;
		int mHackDifficulty;
		int mHackPenalty;
		
	public :
		PDefWorldModel();
		~PDefWorldModel();

		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline int GetUseFlags() const { return mUseFlags; }
		inline int GetFunctionType() const { return mFunctionType; }
		inline int GetFunctionValue() const { return mFunctionValue; }
		inline int GetHackDifficulty() const { return mHackDifficulty; }
		inline int GetHackPenalty() const { return mHackPenalty; }

};

#endif
