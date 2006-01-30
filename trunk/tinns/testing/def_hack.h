#ifndef DEF_HACK_H
#define DEF_HACK_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefHack
{
	private :
		int mIndex;
		//qui aggiungere valori
	public :
		PDefHack();
		~PDefHack();

		bool LoadFromDef(PTokenList *Tokens);
		
		inline int GetIndex() const { return mIndex; }
		//qui aggiungere funzioni per i gets
};

#endif

