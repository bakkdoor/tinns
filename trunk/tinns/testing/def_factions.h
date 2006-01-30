#ifndef DEF_FACTIONS_H
#define DEF_FACTIONS_H

#ifdef _MSC_VER
	#pragma once
#endif

static const int NUMFACTIONS = 19;	// for faction relations only

class PDefFaction
{
	private :
		int mIndex;
		std::string mName;
		int mStartValue;
		bool mAffected;
		int mRelations[NUMFACTIONS];
	public :
		PDefFaction();
		~PDefFaction();
		
		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline int GetStartValue() const { return mStartValue; }
		inline bool GetAffected() const { return mAffected; }
		int GetRelation(int Faction) const;
};

#endif

