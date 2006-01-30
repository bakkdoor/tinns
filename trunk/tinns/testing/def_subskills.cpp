#include "tinns.h"

// subskill.def, names and parameters of all subskills

PDefSubSkill::PDefSubSkill()
{
	mActionModifiers = 0;
	mNumActionModifiers = 0;
}

PDefSubSkill::~PDefSubSkill()
{
	delete [] mActionModifiers;
}

bool PDefSubSkill::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 :	// setentry
				continue;
			
			case 1 :
				mIndex = std::atoi(i->c_str()); break;
			
			case 2 :
				mName = *i; break;
			
			case 3 :
				mShortName = *i; break;

			case 4 :
				mStrengthenFactor = static_cast<float>(std::atof(i->c_str())); break;
			
			case 5 :
			{
				mNumActionModifiers = std::atoi(i->c_str());
				mActionModifiers = new int[mNumActionModifiers];
				std::memset(mActionModifiers, 0, sizeof(int)*mNumActionModifiers);
				break;
			}
		}
		
		if(Idx>=6)
			mActionModifiers[Idx-6]=std::atoi(i->c_str());
	}
	
	return true;
}

