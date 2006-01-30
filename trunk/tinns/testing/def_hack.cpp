#include "tinns.h"

PDefHack::PDefHack()
{
	mIndex = 0;
}

PDefHack::~PDefHack()
{

}

bool PDefHack::LoadFromDef(PTokenList *Tokens)
{
	int Idx=0;
	for(PTokenList::iterator i=Tokens->begin(); i!=Tokens->end(); i++, Idx++)
	{
		switch(Idx)
		{
			case 0:	// setentry
				continue ;
			
			
			case 1:	// index
				mIndex = std::atoi(i->c_str()); break;
			
			
		}
	}
	
	return true;
}

