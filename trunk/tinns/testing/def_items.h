#ifndef DEF_ITEMS_H
#define DEF_ITEMS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PDefItems
{
	private :

		int mIndex;//1
        std::string mName;//2
		int mUnknow03;
		int mUnknow04;
		int mUnknow05;
		int mUnknow06;
		int mUnknow07;
		int mUnknow08;
		int mUnknow09;
		int mUnknow10;
		int mUnknow11;
		int mUnknow12;
		int mUnknow13;
		int mUnknow14;
		int mUnknow15;
		int mUnknow16;
        int mUnknow17;
		int mUnknow18;
		int mUnknow19;
		int mUnknow20;
		int mUnknow21;
		int mUnknow22;
		int mUnknow23;

	public :
		PDefItems();
		~PDefItems();

		bool LoadFromDef(PTokenList *Tokens);
		
		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }

};

#endif

