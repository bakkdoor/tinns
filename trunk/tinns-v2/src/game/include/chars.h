/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community
	maintainer Akiko <akiko@gmx.org>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
	02110-1301, USA.
*/



/*
	chars.h - this is the main file with the main function

	Authors:
	- Namikon

	MODIFIED: 22 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Added SQLLoad() function to PChar and PChars
            - Added enum for charlist SQL Database
            - Added IsOnline var and inline func for Onlinestatus of char
  MODIFIED: 26 Jul 2006 Namikon
	REASON:   - Added Health/MaxHealth, idem for Mana & Stamin
  TODO:     - fix real current Health vs MaxHealth, also in char load/save

  MODIFIED: 03 Oct 2006 Hammag
	REASON:   - PChar::CreateNewChar() and moved effective char creation from PChars to PChar
	          - added PChar::SQLDelete()
	            This method is put here because we want the char to be loaded when deleted from DB to avoid
	            any player to use it at the same time.
	          - added use of auto_save_period config option in PChars::update()

*/

#ifndef CHARS_H
#define CHARS_H

#ifdef _MSC_VER
	#pragma once
#endif

#include "inventory.h"
#include "buddylist.h"
#include "genreplist.h"

class PChar
{
	private :
	// SQL Layout
        enum
        {
            c_id,
            c_name,
            c_str_lvl,
            c_str_pts,
            c_int_lvl,
            c_int_pts,
            c_dex_lvl,
            c_dex_pts,
            c_con_lvl,
            c_con_pts,
            c_psi_lvl,
            c_psi_pts,
            a_id,
            c_class,
            c_profession,
            c_sex,
            c_location,
            c_mc,
            c_hc,
            c_tra,
            c_pc,
            c_rc,
            c_tc,
            c_vhc,
            c_agl,
            c_rep,
            c_rec,
            c_rcl,
            c_atl,
            c_end,
            c_for,
            c_fir,
            c_enr,
            c_xrr,
            c_por,
            c_hlt, // In SQL, this is STILL c_htl. Maxx hasnt changed it yet
            c_hck,
            c_brt,
            c_psu,
            c_wep,
            c_cst,
            c_res,
            c_imp,
            c_ppu,
            c_apu,
            c_mst,
            c_ppw,
            c_psr,
            c_wpw,
            c_apt,
            c_cash,
            c_head,
            c_torso,
            c_legs,
            c_str_xp,
            c_int_xp,
            c_dex_xp,
            c_psi_xp,
            c_con_xp,
            c_pos_x,
            c_pos_y,
            c_pos_z,
            c_angle_ud,
            c_angle_lr,
            c_faction,
            c_slot
        };

		u32 mID;
		u32 mAccount;
		u8 mSlot;
		std::string mName;
		u32 mGender;
		u32 mClass;
		//u32 mType; // Removed that and only keep GetType()
		u32 mProfession;
		u32 mFaction;
  	u32 mRealHead;  // Base Skin elements, in complement of (computed) mType
  	u32 mRealTorso; // " Head shouldn't be changeable, except in case of surgery !!!
  	u32 mRealLegs;  // "
  	u32 mSkin;  // Current Skin elements. *** Not saved in DB atm ***
  	u32 mHead;  // "
  	u32 mTorso; // "
  	u32 mLegs;  // "
  	u8 mHeadColor;  // "
  	u8 mTorsoColor; // "
  	u8 mLegsColor;  // "
  	u8 mHeadDarkness;  // " // 0=Bright, 255=dark
  	u8 mTorsoDarkness; // "
  	u8 mLegsDarkness;  // "
		            // Skin scale factor setting remain to discover, provided they are somewhere for player chars ...

		bool mLocationLeased; // temp until char on-demand load/unload
		u32 mLocation;
		u32 mCash;
		u32 mStartApt; // set same as PrimaryApt atm
		u32 mPrimaryApt;
    u32 mChairInUse; // not saved in DB atm

		u16 mHealth;
		u16 mMana;
		u16 mStamina;

		s8 mSoullight; // *** Not got/saved from DB atm ***
		u8 mCombatRank; // *** Not got/saved from DB atm ***
		u8 mSynaptic; // *** Not got/saved from DB atm ***
		bool mIsDead; // *** Not got/saved from DB atm ***

		// Only one body effect supported atm. Should be extended later to multiple effects
		u8 mBodyEffect; // *** Not got/saved from DB atm ***
		u8 mBodyEffectDensity; // *** Not got/saved from DB atm ***

		u8 mSpeedOverride; // a hack to control move speed. Not saved in DB

		u32 mDirectCharID; // for Direct Chat // *** Not got/saved from DB atm ***
		PBuddyList* mBuddyList; // For Buddy list Chat
		u32 mActiveChatChannels; // Active chat channels flags // *** Not got/saved from DB atm ***

		PGenrepList* mGenrepList; // Character's GR list

		struct PCharCoordinates {
		    u16 mY;     // Y-Position in world
        u16 mZ;     // Z-Position in world
		    u16 mX;     // X-Position in world
        u8 mUD;     // Up - Mid - Down (d6 - 80 - 2a)
        u8 mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-180])
        u8 mAct;    // Last user action state
        // mAct:
            // 0x00 NC has no focus (player alt+tab'ed out)
            // 0x20 Char does nothing     00100000
            // 0x22 kneeing               00100010
            // 0x28 left step             00101000
            // 0x30 right step            00110000
            // 0x40 walking (not running) 01000000 // Seems to mean Running ? - to be verfied, with default walk/run mode !!!
            // 0x60 forward               01100000
            // 0xA0 backward              10100000
            // bits:                      BFWRL.K.

            inline PCharCoordinates()
            {
                mX = mY = mZ = mUD = mLR = mAct = 0;
            }
		};

    bool mIsOnline;
		bool mDirtyFlag;

		class PInventory mInventory;

	protected :
		friend class PChars;
		inline void SetID(u32 ID) { mID = ID; }
		inline void SetAccount(u32 Account) { mAccount = Account; }
    inline void SetCharSlot(u8 Slot) { if (Slot < 4) mSlot = Slot;} // TODO: set max slot according to server config
		inline void SetName(const std::string &Name) { mName = Name; }
		inline void SetGender(u32 Gender) { mGender = Gender; }
		void SetProfession(u32 Profession);
		//inline void SetClass(u32 nClass) { mClass = nClass; } // mClass is defined by setting Profession
		//inline void SetType(u32 Type) { mType = Type; } // Removed. Type is computed from Gender & Profession (??? is it not Gender + Class ???)
		inline void SetFaction(u32 Faction) { mFaction = Faction; }
		//inline void SetModel(u32 Model) { mModel = Model; } // Inhibited for the moment. Base model is deduced from from Gender & Class (Profession)
    void SetRealLook(u32 nHead, u32 nTorso, u32 nLegs);
    void SetBaseSkills();
    void SetBaseSubskills(u8 NZSNb, const char* NonZeroSubskills);
	  void SetBaseInventory();

    void FillinCharDetails(u8 *Packet);
    bool SQLCreate();

	public :
		PChar();
		~PChar();

    PSkillHandler *Skill;
    PCharCoordinates Coords;

    void SetCurrentLook(u32 nSkin, u32 nHead = 0, u32 nTorso = 0, u32 nLegs = 0);
    void SetCurrentLookFromCharType(u32 nType);
    void ResetCurrentLook();

    void SetCurrentBodyColor(u8 nHeadColor, u8 nTorsoColor, u8 nLegsColor, u8 nHeadDarkness = 0, u8 nTorsoDarkness = 0, u8 nLegsDarkness = 0);
    inline void SetBodyEffect(u8 nEffect, u8 nDensity = 0) { mBodyEffect = nEffect; mBodyEffectDensity = nDensity; }
    inline void SetSpeedOverride(u8 nSpeed = 255) { mSpeedOverride = nSpeed; }

		inline u32 GetID() const { return mID; }
		inline u32 GetAccount() const { return mAccount; }
		inline const std::string &GetName() const { return mName; }
		inline u32 GetGender() const { return mGender; }
		inline u32 GetClass() const { return mClass; }
		inline u32 GetType() const { return 2 * mClass + mGender; }
		u32 GetSkinFromCharType(u32 nType);
		void GetRealLook (u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs);
		void GetCurrentLook (u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs);
		inline void GetBodyEffect(u8 &nEffect, u8 &nDensity) { nEffect = mBodyEffect; nDensity = mBodyEffectDensity; }
		void GetCurrentBodyColor(u8 &nHeadColor, u8 &nTorsoColor, u8 &nLegsColor, u8 &nHeadDarkness, u8 &nTorsoDarkness, u8 &nLegsDarkness);
		inline u8 GetSpeedOverride() { return mSpeedOverride; }
		inline u32 GetBaseModel();
	  inline u32 GetProfession() const { return mProfession; }
		inline u16 GetMaxHealth() { return mHealth; }
		inline u16 GetMaxMana() { return mMana; }
		inline u16 GetMaxStamina() { return mStamina; }
		inline u16 GetHealth() { return mHealth; }
		inline u16 GetMana() { return mMana; }
		inline u16 GetStamina() { return mStamina; }
		inline u32 GetFaction() const { return mFaction; }
		inline u32 GetLocation() const { return mLocation; }
		inline u32 GetCash() const { return mCash; }
		inline u32 GetBaseApartment() const { return mPrimaryApt; }

    inline s8 GetSoullight() const { return mSoullight; }
    u8 GetMainRank();
		inline u8 GetCombatRank() const { return mCombatRank; }
		inline u8 GetSynaptic() const { return mSynaptic; }
		inline bool IsDead() const { return mIsDead; }

		inline bool SetDirectChat(u32 nBuddyCharID) { mDirectCharID = nBuddyCharID; return true; }
		inline u32 GetDirectChat() { return mDirectCharID; }
		inline void SetActiveChannels(u32 nChannels) { mActiveChatChannels = nChannels; }
		inline u32 GetActiveChannels() { return mActiveChatChannels; }
		
    inline bool AddBuddy(u32 nBuddyCharID) { return mBuddyList->AddChar(nBuddyCharID); }
    inline bool RemoveBuddy(u32 nBuddyCharID) { return mBuddyList->RemoveChar(nBuddyCharID); }
    inline u16 GetBuddyListDataSize() { return mBuddyList->GetListDataSize(); }
    inline const void* GetBuddyListData() { return mBuddyList->GetListData(); }
    inline u8 GetBuddyCount() { return mBuddyList->Count(); }
    inline bool IsBuddy(u32 CharID) { return mBuddyList->IsInBuddy(CharID); };

    inline bool AddGenrep(u16 nWorldID, u16 nStationID) { return mGenrepList->AddGenrep(nWorldID, nStationID); }
    inline u16 GetGenrepListDataSize() { return mGenrepList->GetListDataSize(); }
    inline const void* GetGenrepListData() { return mGenrepList->GetListData(); }
    inline u8 GetGenrepCount() { return mGenrepList->Count(); }

		inline bool IsDirty() const { return mDirtyFlag; }
    inline bool IsOnline() { return mIsOnline; }
    void SetOnlineStatus(bool IsOnline);

		bool CreateNewChar(u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
      u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot);
		bool SQLLoad(int CharID);
		bool SQLSave();
		bool SQLDelete(); // not implemented yet

		inline void SetLocation(u32 Location) { mLocation = Location; }
		inline void SetDirtyFlag(bool Dirty = true) { mDirtyFlag = Dirty; }

		// temp until char on-demand load/unload
		inline void SetLocationLeased(bool nState = true) { mLocationLeased = nState; }
		inline bool GetLocationLeased() { return mLocationLeased; }

		inline u32 GetChairInUse() { return mChairInUse; }
		inline void SetChairInUse(u32 nItemID) { mChairInUse = nItemID; }

};

class PChars
{
	private :
		typedef std::map<u32, PChar*> CharMap;
		CharMap mChars;
		u32 mLastID;

		std::time_t mAutoSavePeriod;
		std::time_t mLastSave;


	public :
		PChars();
		~PChars();

		void SQLLoad();

		void SQLSave();

		PChar *GetChar(u32 CharID) const;
		PChar *GetChar(const std::string &Name) const;
		PChar *CreateChar(u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
      u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot);

		void Update();
};

#endif
