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
	def_charkinds.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef DEF_CHARKINDS_H
#define DEF_CHARKINDS_H

#include "def.h"

struct PSkillInfo
{
	int mStart;
	int mMax;
	int mGrow;

	inline PSkillInfo()
	{
		mStart = mMax = mGrow = 0;
	}
};

struct PSkillPtsInfo
{
	int mSkill;
	int mPoints;

	inline PSkillPtsInfo()
	{
		mSkill = mPoints = 0;
	}
};

struct PSubSkillPtsInfo
{
	int mSubSkill;
	int mPoints;

	inline PSubSkillPtsInfo()
	{
		mSubSkill = mPoints = 0;
	}
};

struct PStartLevelInfo
{
	int mSubSkill;
	int mLevel;

	inline PStartLevelInfo()
	{
		mSubSkill = mLevel = 0;
	}

};

class PDefCharKind : public PDef
{
	private :
		typedef std::map<int, PSkillPtsInfo*> PSkillPtsMap;
		typedef std::map<int, PSubSkillPtsInfo*> PSubSkillPtsMap;
		typedef std::map<int, PStartLevelInfo*> PStartLevelMap;

		//int mIndex;
		std::string mName;
		int mType;

		// TODO: shouldnt this be a map?
		PSkillInfo *mSkillInfo;

		PSkillPtsMap mSkillPts;
		PSubSkillPtsMap mSubSkillPts;
		PStartLevelMap mStartLevels;
		int mMoney;
		u32 mInventory[8];
	public :
		PDefCharKind();
		~PDefCharKind();

		bool LoadFromDef(PTokenList *Tokens);

		inline const std::string &GetName() const { return mName; }
		inline int GetType() const { return mType; }
		inline const PSkillInfo &GetSkillInfo(int Skill) const { return mSkillInfo[Skill-1]; }

    inline int GetStartMoney() const { return mMoney; }
    inline u32 GetStartInventory(u8 Index) const { return ((Index < 7) ?  mInventory[Index] : 0); }
		// TODO: mission get() functions
};

#endif

