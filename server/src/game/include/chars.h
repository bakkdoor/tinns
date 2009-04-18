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

#define MAXCASH 1000000000

#include "inventory.h"
#include "buddylist.h"
#include "genreplist.h"
#include "regex++.h"
class PVhcAccessRequestList;

enum PSeatType
{
  seat_none = 0,
  seat_chair,
  seat_subway,
  seat_vhc
};

class PCharCoordinates
{
  public:
    u16 mY;     // Y-Position in world
    u16 mZ;     // Z-Position in world
    u16 mX;     // X-Position in world
    u8 mUD;     // Up - Mid - Down (d6 - 80 - 2a)
    u8 mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-179])
    u8 mAct;    // Last user action state
    u8 mUnknown;// sometime sent by client with value != 0 (usual case)
    // mAct:
    // 0x00 NC has no focus (player alt+tab'ed out)
    // 0x04 Char on ground/dead   00000100
    // 0x20 Char does nothing     00100000
    // 0x22 kneeing               00100010
    // 0x28 left step             00101000
    // 0x30 right step            00110000
    // 0x40 walking (not running) 01000000 // Seems to mean Running ? - to be verfied, with default walk/run mode !!!
    // 0x60 forward               01100000
    // 0xA0 backward              10100000
    // bits:                      BFWRL.K.

    u8 mJumpingState;

    inline PCharCoordinates() { mX = mY = mZ = mUD = mLR = mAct = mUnknown = mJumpingState = 0;}
    void SetPosition( u16 nY, u16 nZ, u16 nX, u8 nUD = 0x80, u8 nLR = 0 );
    void SetInterpolate( PCharCoordinates& Pos1, PCharCoordinates& Pos2, f32 nCoef );
};

class PChar
{
  private :
    // static members
    static RegEx* mCharnameRegexFilter;

    // instance members
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

    // not saved in DB atm
    PSeatType mSeatInUseType;
    u32 mSeatInUseObjectId;
    u8 mSeatInUseSeatId;
    PVhcAccessRequestList* mVhcAccessRequestList;

    PContainer* mContainerInExclusiveUse;

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

    u8 mQuickBeltActiveSlot; // QB SlotID of item "in hand", or INV_WORN_QB_HAND or INV_WORN_QB_NONE

    u16 mLookingAt;  // Zone charID of currently targeted player
    std::time_t mLookAtTimestamp; // Lifetimer of lookat var

    bool mIsOnline;
    bool mDirtyFlag;

    bool mShunned;
    bool mJailed;

    class PInventory mInventory;

  protected :
    friend class PChars;
    inline void SetID( u32 ID ) { mID = ID; }
    inline void SetAccount( u32 Account ) { mAccount = Account; }
    inline void SetCharSlot( u8 Slot ) { if ( Slot < 4 ) mSlot = Slot;} // TODO: set max slot according to server config
    inline void SetName( const std::string &Name ) { mName = Name; }
    inline void SetGender( u32 Gender ) { mGender = Gender; }
    void SetProfession( u32 Profession );
    //inline void SetClass(u32 nClass) { mClass = nClass; } // mClass is defined by setting Profession
    //inline void SetType(u32 Type) { mType = Type; } // Removed. Type is computed from Gender & Profession (??? is it not Gender + Class ???)
    inline void SetFaction( u32 Faction ) { mFaction = Faction; }
    //inline void SetModel(u32 Model) { mModel = Model; } // Inhibited for the moment. Base model is deduced from from Gender & Class (Profession)
    void SetRealLook( u32 nHead, u32 nTorso, u32 nLegs );
    void SetBaseSkills();
    void SetBaseSubskills( u8 NZSNb, const char* NonZeroSubskills );
    void SetBaseInventory();

    bool SQLCreate();

  public :
    PChar();
    ~PChar();

    static bool SetCharnameRegexFilter( const char* RegexStr );
    static bool IsCharnameWellFormed( const char *Username );

    PSkillHandler *Skill;
    PCharCoordinates Coords;

    void SetCurrentLook( u32 nSkin, u32 nHead = 0, u32 nTorso = 0, u32 nLegs = 0 );
    void SetCurrentLookFromCharType( u32 nType );
    void ResetCurrentLook();

    void SetCurrentBodyColor( u8 nHeadColor, u8 nTorsoColor, u8 nLegsColor, u8 nHeadDarkness = 0, u8 nTorsoDarkness = 0, u8 nLegsDarkness = 0 );
    inline void SetBodyEffect( u8 nEffect, u8 nDensity = 0 ) { mBodyEffect = nEffect; mBodyEffectDensity = nDensity; }
    inline void SetSpeedOverride( u8 nSpeed = 255 ) { mSpeedOverride = nSpeed; }

    void SetLookingAt( u16 nCharID );
    u16 GetLookingAt( u16 nMaxDelaySec = 1 );

    inline PInventory* GetInventory() { return &mInventory; }
    inline u32 GetID() const { return mID; }
    inline u32 GetAccount() const { return mAccount; }
    inline const std::string &GetName() const { return mName; }
    inline u32 GetGender() const { return mGender; }
    inline u32 GetClass() const { return mClass; }
    inline u32 GetType() const { return 2 * mClass + mGender; }
    u32 GetSkinFromCharType( u32 nType );
    void GetRealLook( u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs );
    void GetCurrentLook( u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs );
    inline void GetBodyEffect( u8 &nEffect, u8 &nDensity ) { nEffect = mBodyEffect; nDensity = mBodyEffectDensity; }

    inline u8 GetQuickBeltActiveSlot() { return mQuickBeltActiveSlot; }
    bool SetQuickBeltActiveSlot( u8 nSlotID );

    void GetCurrentBodyColor( u8 &nHeadColor, u8 &nTorsoColor, u8 &nLegsColor, u8 &nHeadDarkness, u8 &nTorsoDarkness, u8 &nLegsDarkness );
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
    u32 SetCash( u32 nCash );  // Does return the new cashvalue, NO udpmessage is sent out!!

    inline u32 GetBaseApartment() const { return mPrimaryApt; }

    inline void SetJail( bool val ) { mJailed = val; };
    inline void SetShun( bool val ) { mShunned = val; };

    inline bool IsJailed() { return mJailed; };
    inline bool IsShunned() { return mShunned; };

    inline s8 GetSoullight() const { return mSoullight; }
    u8 GetMainRank();
    inline u8 GetCombatRank() const { return mCombatRank; }
    inline u8 GetSynaptic() const { return mSynaptic; }
    inline bool IsDead() const { return mIsDead; }

    inline bool SetDirectChat( u32 nBuddyCharID ) { mDirectCharID = nBuddyCharID; return true; }
    inline u32 GetDirectChat() { return mDirectCharID; }
    inline void SetActiveChannels( u32 nChannels ) { mActiveChatChannels = nChannels; }
    inline u32 GetActiveChannels() { return mActiveChatChannels; }

    inline bool AddBuddy( u32 nBuddyCharID ) { return mBuddyList->AddChar( nBuddyCharID ); }
    inline bool RemoveBuddy( u32 nBuddyCharID ) { return mBuddyList->RemoveChar( nBuddyCharID ); }
    inline u16 GetBuddyListDataSize() { return mBuddyList->GetListDataSize(); }
    inline const void* GetBuddyListData() { return mBuddyList->GetListData(); }
    inline u8 GetBuddyCount() { return mBuddyList->Count(); }
    inline bool IsBuddy( u32 CharID ) { return mBuddyList->IsInBuddy( CharID ); };

    inline bool AddGenrep( u16 nWorldID, u16 nStationID ) { return mGenrepList->AddGenrep( nWorldID, nStationID ); }
    inline u16 GetGenrepListDataSize() { return mGenrepList->GetListDataSize(); }
    inline const void* GetGenrepListData() { return mGenrepList->GetListData(); }
    inline u8 GetGenrepCount() { return mGenrepList->Count(); }

    inline bool IsDirty() const { return mDirtyFlag; }
    inline bool IsAnyDirty() const { return mDirtyFlag || mInventory.IsDirty(); }
    inline bool IsOnline() { return mIsOnline; }
    void SetOnlineStatus( bool IsOnline );

    bool CreateNewChar( u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
                        u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot );
    bool SQLLoad( int CharID );
    bool SQLSave();
    inline bool SQLSaveFull() { return SQLSave() && mInventory.SQLSave(); }
    bool SQLDelete(); // not implemented yet

    inline void SetLocation( u32 Location ) { mLocation = Location; }
    inline void SetDirtyFlag( bool Dirty = true ) { mDirtyFlag = Dirty; }

    // temp until char on-demand load/unload
    inline void SetLocationLeased( bool nState = true ) { mLocationLeased = nState; }
    inline bool GetLocationLeased() { return mLocationLeased; }

    PSeatType GetSeatInUse( u32* nObjectId = NULL, u8* nSeatId = NULL );
    void SetSeatInUse( PSeatType nSeatType, u32 nObjectId = 0, u8 nSeatId = 0 );

    PVhcAccessRequestList* GetVhcAccessRequestList();

    inline PContainer* GetContainerInExclusiveUse() { return mContainerInExclusiveUse; }
    inline void SetContainerInExclusiveUse( PContainer* nContainer ) { mContainerInExclusiveUse = nContainer; }
};

struct PCharProfile
{
  u32 CharID;
  u16 Type;
  u16 Color0;
  u16 Unknown1;
  u8 Head;
  u8 Torso;
  u8 Legs;
  u32 Location;
  u8 NameLen;
  u8 Unknown3;
  u8 Unknown4;
  u8 Unknown5;
  u8 Unknown6;
  u8 Unknown7;
  u8 Unknown8;
  u8 Unknown9;
  u8 Unknown10;
  u8 Unknown11;
  u8 Unknown12;
  std::string Name;
  bool in_use;
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

    bool LoadChar( u32 CharID );
    bool AddChar( PChar* nChar );
    PChar* RemoveChar( u32 CharID );

    PChar* GetChar( u32 CharID ) const;
    PChar* GetChar( const std::string &Name ) const;
    bool CharExist( const std::string &Name ) const;

    void SQLSave();
    void Update();

    int GetCharProfiles( const u32 AccountID, PCharProfile* CharSlotsArray, const u8 ArraySize ); // return effective entries nb
};

#endif
