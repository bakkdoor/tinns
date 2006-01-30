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

