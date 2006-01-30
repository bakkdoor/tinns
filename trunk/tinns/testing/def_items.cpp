#include "tinns.h"

PDefItems::PDefItems()
{
	mIndex = 0;
}

PDefItems::~PDefItems()
{

}

bool PDefItems::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
					continue;
			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;

			case 2: // name
				mName = *i; break;
			
		}
	}
	
	return true;
}

