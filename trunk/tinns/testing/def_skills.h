#ifndef DEF_SKILLS_H
#define DEF_SKILLS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefSkill
{
	private :
		int mIndex;
		std::string mName;
		std::string mShortName;
		int mNumSubSkills;
		int *mSubSkills;
	public :
		PDefSkill();
		~PDefSkill();
		
		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetShortName() const { return mShortName; }
		inline int GetNumSubSkills() const { return mNumSubSkills; }
		inline int GetSubSkill(int Index) const { return mSubSkills[Index]; }
};

#endif

