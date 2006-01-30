#ifndef DEF_SUBSKILLS_H
#define DEF_SUBSKILLS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefSubSkill
{
	private :
		int mIndex;
		std::string mName;
		std::string mShortName;
		float mStrengthenFactor;
		int mNumActionModifiers;
		int *mActionModifiers;
	public :
		PDefSubSkill();
		~PDefSubSkill();

		bool LoadFromDef(PTokenList *Tokens);
		
		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetShortName() const { return mShortName; }
		inline float GetStrengthenFactor() const { return mStrengthenFactor; }
		inline int GetNumActionModifiers() const { return mNumActionModifiers; }
		inline int GetActionModifier(int Index) const { return mActionModifiers[Index]; }
};

#endif

