#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#ifdef _MSC_VER
	#pragma once
#endif

enum PAccountLevel
{
	PAL_BANNED,
	PAL_PLAYER,
	PAL_VOLUNTEER,
	PAL_GM,
	PAL_ADMIN
};

class PAccount
{
	private :
		enum { MAXCHARSPERACC = 4 };
		u32 mID;
		std::string mName;
		std::string mPassword;
		PAccountLevel mLevel;
		u32 mChars[MAXCHARSPERACC];
		bool mConsoleAllowed;
		bool mDirty;
	protected :
		friend class PAccounts;
		void SetID(u32 ID);
		void SetName(const std::string &Name);
		void SetPassword(const std::string &Pass);
		void SetLevel(PAccountLevel Level);
		void SetConsoleAllowed(bool Allowed);
		inline void SetDirtyFlag() { mDirty = true; }
		inline void ClearDirtyFlag() { mDirty = false; }
	public :
		PAccount();
		
		inline u32 GetID() const { return mID; }
		inline const std::string &GetName() const { return mName; }
		inline const std::string &GetPassword() const { return mPassword; }
		inline PAccountLevel GetLevel() const { return mLevel; }
		std::string GetLevelString() const;
		inline bool IsConsoleAllowed() const { return mConsoleAllowed; }
		inline bool IsDirty() const { return mDirty; }
		
		bool AddChar(u32 CharID);
		void RemoveChar(u32 CharID);
		void RemoveChar(int CharIndex);
		u32 GetChar(int Num) const;
		
		bool Load(const TiXmlElement *Node);
		void Save(TiXmlElement *Node);
};

class PAccounts 
{
	private :
		typedef std::map<u32, PAccount*> AccountMap;
		AccountMap mAccounts;
		u32 mLastID;
	public :
		PAccounts();
		~PAccounts();
		
		PAccount *Authenticate(const char *User, const u8* Password, int PassLen, const u8 *Key, bool UseAutoAccounts=false);
		PAccount *Authenticate(const char *User, const char *Password) const;
		void Load();
		void Update();
		
		PAccount *GetAccount(u32 AccountID) const;
		PAccount *GetAccount(const std::string &Name) const;
		PAccount *CreateAccount(const std::string &Name, const std::string &Password);
};

#endif

