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
	gamedefs.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL
*/

#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#ifdef _MSC_VER
	#pragma once
#endif

typedef std::map<int, PDefCharacter*> PDefCharacterMap;
typedef std::map<int, PDefSkill*> PDefSkillMap;
typedef std::map<int, PDefSubSkill*> PDefSubSkillMap;
typedef std::map<int, PDefCharKind*> PDefCharKindMap;
typedef std::map<int, PDefWorld*> PDefWorldMap;
typedef std::map<int, PDefFaction*> PDefFactionMap;

/**************************///NEW
typedef std::map<int, PDefHack*> PDefHackMap;
typedef std::map<int, PDefItems*> PDefItemsMap;
/************************///NEW

class PGameDefs
{
	private :
		PDefCharacterMap mCharDefs;
		PDefSkillMap mSkillDefs;
		PDefSubSkillMap mSubSkillDefs;
		PDefCharKindMap mCharKindDefs;
		PDefWorldMap mWorldDefs;
		PDefFactionMap mFactionDefs;

		/**************************///NEW
		PDefHackMap mHackDefs;
		PDefItemsMap mItemsDefs;
		/**************************///ENDNEW

		bool LoadCharDefs();
		bool LoadSkillDefs();
		bool LoadSubSkillDefs();
		bool LoadCharKindDefs();
		bool LoadWorldDefs();
		bool LoadFactionDefs();

		/**************************///NEW
		bool LoadHackDefs();
		bool LoadItemsDefs();
		/**************************///END NEW



	public :
		PGameDefs();
		~PGameDefs();

		inline int GetNumCharDefs() const { return mCharDefs.size(); }
		const PDefCharacter *GetCharDef(int Index) const;
		inline int GetNumSkillDefs() const { return mSkillDefs.size(); }
		const PDefSkill *GetSkillDef(int Index) const;
		inline int GetNumSubSkillDefs() const { return mSubSkillDefs.size(); }
		const PDefSubSkill *GetSubSkillDef(int Index) const;

		inline int GetNumCharKindDefs() const { return mCharKindDefs.size(); }
		const PDefCharKind *GetCharKindDef(int Index) const;

		inline int GetNumWorldDefs() const { return mWorldDefs.size(); }
		const PDefWorld *GetWorldDef(int Index) const;
		inline int GetNumFactionDefs() const { return mFactionDefs.size(); }
		const PDefFaction *GetFactionDef(int Index) const;

		/**************************///NEW
		inline int GetNumHackDefs() const { return mHackDefs.size(); }
		const PDefHack *GetHackDef(int Index) const;

		inline int GetNumItemsDefs() const { return mItemsDefs.size(); }
		const PDefItems *GetItemsDef(int Index) const;
		/**************************///END NEW

		void Init();
};

#endif

