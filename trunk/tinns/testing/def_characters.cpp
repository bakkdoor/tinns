#include "tinns.h"

// characters.def contains all ingame characters (player types, npc's, monsters)

PDefCharacter::PDefCharacter()
{
	mIndex = -1;
	mName = "unknown";
	mModel = -1;
}

PDefCharacter::~PDefCharacter()
{
}

bool PDefCharacter::LoadFromDef(PTokenList *Tokens)
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
				mModel = std::atoi(i->c_str()); break;
		}
		
		if(Idx==4)
			return true;
	}
	
	return false;
}

