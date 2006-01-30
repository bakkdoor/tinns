#ifndef DEF_CHARKINDS_H
#define DEF_CHARKINDS_H

#ifdef _MSC_VER
	#pragma once
#endif

struct PSkillInfo
{
	int mStart;
	int mMax;
	int mGrow;
	
	inline PSkillInfo()
	{
		mStart = mMax = mGrow = 0;
	}
};

struct PSkillPtsInfo
{
	int mSkill;
	int mPoints;
	
	inline PSkillPtsInfo()
	{
		mSkill = mPoints = 0;
	}
};

struct PSubSkillPtsInfo
{
	int mSubSkill;
	int mPoints;
	
	inline PSubSkillPtsInfo()
	{
		mSubSkill = mPoints = 0;
	}
};

struct PStartLevelInfo
{
	int mSubSkill;
	int mLevel;
	
	inline PStartLevelInfo()
	{
		mSubSkill = mLevel = 0;
	}
	
};

class PDefCharKind
{
	private :
		typedef std::map<int, PSkillPtsInfo*> PSkillPtsMap;
		typedef std::map<int, PSubSkillPtsInfo*> PSubSkillPtsMap;
		typedef std::map<int, PStartLevelInfo*> PStartLevelMap;
		
		int mIndex;
		std::string mName;
		int mType;
		
		// TODO: shouldnt this be a map?
		PSkillInfo *mSkillInfo;
		
		PSkillPtsMap mSkillPts;
		PSubSkillPtsMap mSubSkillPts;
		PStartLevelMap mStartLevels;
		int mMoney;
		u32 mInventory[8];
	public :
		PDefCharKind();
		~PDefCharKind();
		
		bool LoadFromDef(PTokenList *Tokens);

		inline int GetIndex() const { return mIndex; }
		inline const std::string &GetName() const { return mName; }
		inline int GetType() const { return mType; }
		inline const PSkillInfo &GetSkillInfo(int Skill) { return mSkillInfo[Skill]; }
		
		// TODO: mission get() functions
};

#endif

