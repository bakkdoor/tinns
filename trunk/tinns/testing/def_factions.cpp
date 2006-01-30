#include "tinns.h"

PDefFaction::PDefFaction()
{
	mIndex = 0;
	memset(mRelations, 0, sizeof(mRelations));
}

PDefFaction::~PDefFaction()
{
}

bool PDefFaction::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0 : // setfrac
				continue;
			
			case 1 :
				mIndex = std::atol(i->c_str()); break;
			
			case 2 :
				mStartValue = std::atol(i->c_str()); break;
			
			case 3 :
				mAffected = std::atol(i->c_str())!=0; break;
			
			default :
			{
				if(Idx-4 < NUMFACTIONS)
					mRelations[Idx-4]=std::atol(i->c_str()); break;
			}
		}
	}
	
	return true;
}

int PDefFaction::GetRelation(int Faction) const
{
	// faction 0 has no relations
	if(Faction <= 0 || Faction > NUMFACTIONS)
		return 0;
		
	return mRelations[Faction-1];
}

