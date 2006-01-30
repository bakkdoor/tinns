#include "tinns.h"

PDefWorld::PDefWorld()
{
	mIndex = 0;
}

PDefWorld::~PDefWorld()
{

}

bool PDefWorld::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
					continue;
			case 3: // music
				continue;
			
			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;
			
			case 2: // name
				mName = *i; break;
			
			case 4: // datfile
				mDatFile = *i; break;
			
			case 5:
				mFlags = std::atoi(i->c_str()); break;
		}
	}
	
	return true;
}

