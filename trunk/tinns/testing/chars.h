#ifndef CHARS_H
#define CHARS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PChar
{
	private :
		u32 mID;
		u32 mAccount;
		std::string mName;
		u32 mType;
		u32 mProfession;
		u32 mFaction;
		u32 mModel;
		u32 mLocation;
		
		bool mDirtyFlag;
	protected :
		friend class PChars;
		inline void SetID(u32 ID) { mID = ID; }
		inline void SetAccount(u32 Account) { mAccount = Account; }
		inline void SetName(const std::string &Name) { mName = Name; }
		inline void SetType(u32 Type) { mType = Type; }
		inline void SetProfession(u32 Profession) { mProfession = Profession; }
		inline void SetFaction(u32 Faction) { mFaction = Faction; }
		inline void SetModel(u32 Model) { mModel = Model; }
		
		
		
	public :
		PChar();
		inline u32 GetID() const { return mID; }
		inline u32 GetAccount() const { return mAccount; }
		inline const std::string &GetName() const { return mName; }
		inline u32 GetType() const { return mType; }
		inline u32 GetProfession() const { return mProfession; }
		inline u32 GetFaction() const { return mFaction; }
		inline u32 GetModel() const { return mModel; }
		inline u32 GetLocation() const { return mLocation; }
		
		inline bool IsDirty() const { return mDirtyFlag; }
		
		bool Load(TiXmlElement *Node);
		void Save();
		inline void SetLocation(u32 Location) { mLocation = Location; }//WAS PROTECTED
		inline void SetDirtyFlag(bool Dirty) { mDirtyFlag = Dirty; }//WAS PROTECTED
};

class PChars
{
	private :
		typedef std::map<u32, PChar*> CharMap;
		CharMap mChars;
		u32 mLastID;
		std::clock_t mLastSave;
	public :
		PChars();
		~PChars();

		void Load();
		void Save();
		
		PChar *GetChar(u32 CharID) const;
		PChar *GetChar(const std::string &Name) const;
		PChar *CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model);
		
		void Update();
};

#endif

