#ifndef DEF_CHARACTERS_H
#define DEF_CHARACTERS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefCharacter
{
	private :
		int mIndex;
		std::string mName;
		int mModel;
	public :
		PDefCharacter();
		~PDefCharacter();
		
		bool LoadFromDef(PTokenList *Tokens);
		
		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline int GetModel() const { return mModel; }
};

#endif

