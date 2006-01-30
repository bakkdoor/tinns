#ifndef DEF_WORLDS_H
#define DEF_WORLDS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefWorld
{
	private :
		int mIndex;
		std::string mName;
		std::string mDatFile;
		int mFlags;
	public :
		PDefWorld();
		~PDefWorld();

		bool LoadFromDef(PTokenList *Tokens);
		
		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetDatFile() const { return mName; }
		inline int GetFlags() const { return mFlags; }
};

#endif

