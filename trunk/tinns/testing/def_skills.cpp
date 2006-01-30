#include "tinns.h"

// skill.def includes skill names and skill/subskill mapping

PDefSkill::PDefSkill()
{
	mSubSkills = 0;
	mNumSubSkills = 0;
}

PDefSkill::~PDefSkill()
{
	delete [] mSubSkills;
}

bool PDefSkill::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 :	// setentry
				continue;
			
			case 1 :
				mIndex = std::atol(i->c_str()); break;
			
			case 2 :
				mName = *i; break;
			
			case 3 :
				mShortName = *i; break;
				
			case 4 :
			{
				mNumSubSkills = std::atol(i->c_str()); 
				mSubSkills = new int[mNumSubSkills];
				std::memset(mSubSkills, 0, sizeof(int)*mNumSubSkills);
				break;
			}
		}
		
		if(Idx >= 5)
		{
			mSubSkills[Idx-5]=std::atoi(i->c_str());
			if(!GameDefs->GetSubSkillDef(mSubSkills[Idx-5]))
			{
				Console->Print("Skill def error: skill %s refers to nonexistant subskill %i", mShortName.c_str(), mSubSkills[Idx-5]);
				return false;
			}
		}
	}
	
	if(Idx-5 != mNumSubSkills)
	{
		Console->Print("Warning: skill %s has incorrect number of subskills", mShortName.c_str());
		Console->Print("  Expected %i, available %i", mNumSubSkills, Idx-5);
	}
	return true;
}
