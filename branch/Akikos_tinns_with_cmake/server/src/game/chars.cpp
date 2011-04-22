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
	chars.cpp

	MODIFIED: 22 Dec 2005 Namikon
	REASON: - Added GPL
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Added SQLLoad() function to PChar and PChars
		- Changed FmtTxt() to sprintf(). It does... uhm, the same :D
		- Added IsOnline var and function
	MODIFIED: 06 Jan 2005 Namikon
	REASON: - Added colored console ouputs
		- Removed XM Load() and Save() functions
		- Rewrote parts of SQLLoad to work with the global NeoPolis / TinNS database
		- Added FillinCharDetails to fill the baseline up with the char details
	MODIFIED: 03 Oct 2006 Hammag
	REASON: - PChar::CreateNewChar() and moved effective char creation from PChars to PChar
		- added PChar::SQLDelete()
		  This method is put here because we want the char to be loaded when deleted from DB to avoid
		  any player to use it at the same time.
		- added use of auto_save_period config option in PChars::update()
		- removed old XML-storage related code
	MODIFIED: 09 Jun 2009 Akiko
	REASON: - adjusted includes for ne buildsystem
		- clean ups

	TODO:   - implement PChar::SQLDelete()
*/

#include "main.h"
#include "include/worlds.h"
#include "include/appartements.h"
#include "include/container.h"
#include "include/inventory.h"
#include "include/vhcaccessrequest.h"

// PCharCoordinates
void PCharCoordinates::SetInterpolate( PCharCoordinates& Pos1, PCharCoordinates& Pos2, f32 nCoef )
{
  if (( nCoef < 0 ) || ( nCoef > 1 ) )
  {
    Console->Print( RED, BLACK, "[Error] PCharCoordinates::Interpolate : Invalid nCoef value: %f", nCoef );
    nCoef = 0;
  }
  f32 rCoef = 1 - nCoef;

  mY = ( u16 )( rCoef * Pos1.mY + nCoef * Pos2.mY );
  mZ = ( u16 )( rCoef * Pos1.mZ + nCoef * Pos2.mZ );
  mX = ( u16 )( rCoef * Pos1.mX + nCoef * Pos2.mX );
  mUD = ( u8 )( rCoef * Pos1.mUD + nCoef * Pos2.mUD );
  if ( abs( Pos1.mLR - Pos2.mLR ) < 90 )
  {
    mLR = ( u8 )( rCoef * Pos1.mLR + nCoef * Pos2.mLR );
  }
  else
  {
    mLR = ( u8 )(( u16 )( rCoef * ( 180.0 + ( f32 )Pos1.mLR ) + nCoef * Pos2.mLR ) % 180 );
  }
}

u16 mY;     // Y-Position in world
u16 mZ;     // Z-Position in world
u16 mX;     // X-Position in world
u8 mUD;     // Up - Mid - Down (d6 - 80 - 2a)
u8 mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-179])
u8 mAct;    // Last user action state

void PCharCoordinates::SetPosition( u16 nY, u16 nZ, u16 nX, u8 nUD, u8 nLR )
{
  mY = nY;
  mZ = nZ;
  mX = nX;
  mUD = nUD;
  if ( mUD < 0x2a )
  {
    mUD = 0x2a;
  }
  else if ( mUD > 0xd6 )
  {
    mUD = 0xd6;
  }
  mLR = nLR % 180;
}


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
    c_hlt,
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
    c_slot,
    c_online,
    c_clan,
    c_soullight
};

RegEx* PChar::mCharnameRegexFilter = NULL;

PChar::PChar()
{
  mID = 0;
  mAccount = 0;
  mGender = 0;
  mClass = 0;
  mProfession = 1;
  mFaction = 1;
  //mModel = 0;
  //mType = 0;
  mRealHead = 0;  // Base Skin elements, in complement of (computed) mType
  mRealTorso = 0; // " Head shouldn't be changeable, except in case of surgery !!!
  mRealLegs = 0;  // "
  mSkin = 0;  // Current Skin elements
  mHead = 0;  // "
  mTorso = 0; // "
  mLegs = 0;  // "
  mHeadColor = mTorsoColor = mLegsColor = 0; // "
  mHeadDarkness = mTorsoDarkness = mLegsDarkness = 0; // "

  mBodyEffect = 0;
  mBodyEffectDensity = 0;
  mQuickBeltActiveSlot = INV_WORN_QB_NONE;

  mLookingAt = 0;
  mLookAtTimestamp = 0;
  mLastUsedWorldObjectId = 0;

  mSpeedOverride = 255; // means no override. Value 0 can be used to forbid any move.

  mLocationLeased = false; // temp until char on-demand load/unload
  mLocation = 0;
  mStartApt = 0;
  mPrimaryApt = 0;
  mCash = 0;

  mSeatInUseType = seat_none;
  mSeatInUseObjectId = 0;
  mSeatInUseSeatId = 0;
  mVhcAccessRequestList = NULL;
  
  mContainerInExclusiveUse = NULL;

  mIsOnline = false;
  mDirtyFlag = false;

  mShunned = false;
  mJailed = false;

  mDialogNPC = 0;
  mCurrentDialogNode = 0;

  Skill = new PSkillHandler();
  mBuddyList = NULL;
  mGenrepList = NULL;

  // Required for initial OOC Broadcast welcome message.
  //Gets overwritten as soon as the first PingPacket arrives
  mActiveChatChannels = 262144;
  mClanLevel = 0;
  mClanID = 0;
}

PChar::~PChar()
{
  // Addition; Set char's online status to OFFLINE
  char sqlqry[50];
  snprintf(sqlqry, 50, "UPDATE characters SET c_online = 0 WHERE c_id = %d", mID);
  MySQL->GameQuery(sqlqry);

  delete Skill;
  delete mBuddyList;
  delete mGenrepList;
  delete mVhcAccessRequestList;

  if ( mContainerInExclusiveUse )
  {
    mContainerInExclusiveUse->EndUse( mID );
    Console->Print( RED, BLACK, "Warning: PChar::~PChar : Char still had exclusive use of container. Now freed." );
  }

  if ( mSeatInUseType != seat_none )
  {
    Console->Print( RED, BLACK, "Warning: PChar::~PChar : Char still using seat %d of vhc or chair %d.", mSeatInUseSeatId, mSeatInUseObjectId );
  }
}

bool PChar::SetCharnameRegexFilter( const char* RegexStr )
{
  if ( mCharnameRegexFilter )
  {
    delete mCharnameRegexFilter;
    mCharnameRegexFilter = NULL;
  }

  if ( RegexStr )
  {
    try
    {
      mCharnameRegexFilter = new RegEx( RegexStr, PCRE_CASELESS );
    }
    catch ( ... )
    {
      return false;
    }
  }
  return true;
}

bool PChar::IsCharnameWellFormed( const char *Charname )
{
  if ( mCharnameRegexFilter )
  {
    return mCharnameRegexFilter->Search( Charname );
  }
  else
    return true;
}

void PChar::SetProfession( u32 Profession )
{
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( Profession );
  if ( def == NULL )
  {
    Console->Print( RED, BLACK, "Char %d: Invalid profession %d", mID, Profession );
    mProfession = 10;
    mClass = 0;
  }
  else
  {
    mProfession = Profession;
    mClass = def->GetType();
  }
  SetDirtyFlag();
}

u32 PChar::GetSkinFromCharType( u32 nType )
{
  const PDefCharacter* nDefCharacter = GameDefs->Chars()->GetDef( nType );
  if ( nDefCharacter )
  {
    return (( u32 )( nDefCharacter->GetModel() ) );
  }
  else
    return 0;
}

inline u32 PChar::GetBaseModel()
{
  return GetSkinFromCharType( GetType() );
}

void PChar::SetRealLook( u32 nHead, u32 nTorso, u32 nLegs )
{
  mRealHead = nHead;
  mRealTorso = nTorso;
  mRealLegs = nLegs;
  SetDirtyFlag();
  ResetCurrentLook();
}

void PChar::GetRealLook( u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs )
{
  nSkin = GetBaseModel();
  nHead = mRealHead;
  nTorso = mRealTorso;
  nLegs = mRealLegs;
}

void PChar::SetCurrentLookFromCharType( u32 nType )
{
  int iHead, iTorso, iLegs;
  u32 nSkin, nHead, nTorso, nLegs;
  u8 nColor, nBrightness;

  const PDefCharacter* nDefCharacter = GameDefs->Chars()->GetDef( nType );
  if ( nDefCharacter )
  {
    nSkin = ( u32 ) nDefCharacter->GetModel();
    iHead = nDefCharacter->GetHead();
    iTorso = nDefCharacter->GetTorso();
    iLegs = nDefCharacter->GetLegs();
    nColor = ( u8 )( 0xff & nDefCharacter->GetColor() );
    nBrightness = ( u8 )( 0xff & nDefCharacter->GetBrightness() );


    if (( iHead < 0 ) || ( iTorso < 0 ) || ( iLegs < 0 ) )
    {
      nHead = nTorso = nLegs = 0;
    }
    else
    {
      nHead = iHead % 10;
      nTorso = iTorso % 10;
      nLegs = iLegs % 10;
    }

    SetCurrentLook( nSkin, nHead, nTorso, nLegs );
    SetCurrentBodyColor( nColor, nColor, nColor, nBrightness, nBrightness, nBrightness );
  }
}

void PChar::SetCurrentLook( u32 nSkin, u32 nHead, u32 nTorso, u32 nLegs )
{
  mSkin = nSkin;
  mHead = nHead;
  mTorso = nTorso;
  mLegs = nLegs;
  // Ingame skin update will be done automagically in the normal update flow
  // A flag could also be set somewhere (preferably in Char si that we don't have to care if ingame or not)
  //    to request quicker ingame update
}

void PChar::ResetCurrentLook()
{
  SetCurrentLook( GetSkinFromCharType( GetType() ), mRealHead, mRealTorso, mRealLegs );
}

// GetCurrentLook will later have to take Power Armors and GM overrides into account
void PChar::GetCurrentLook( u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs )
{
  nSkin = mSkin;
  nHead = mHead;
  nTorso = mTorso;
  nLegs = mLegs;
}

void PChar::SetCurrentBodyColor( u8 nHeadColor, u8 nTorsoColor, u8 nLegsColor, u8 nHeadDarkness, u8 nTorsoDarkness, u8 nLegsDarkness )
{
  mHeadColor = nHeadColor;
  mTorsoColor = nTorsoColor;
  mLegsColor = nLegsColor;
  mHeadDarkness = nHeadDarkness;
  mTorsoDarkness = nTorsoDarkness;
  mLegsDarkness = nLegsDarkness;
}

void PChar::GetCurrentBodyColor( u8 &nHeadColor, u8 &nTorsoColor, u8 &nLegsColor, u8 &nHeadDarkness, u8 &nTorsoDarkness, u8 &nLegsDarkness )
{
  nHeadColor = mHeadColor;
  nTorsoColor = mTorsoColor;
  nLegsColor = mLegsColor;
  nHeadDarkness = mHeadDarkness;
  nTorsoDarkness = mTorsoDarkness;
  nLegsDarkness = mLegsDarkness;
}

void PChar::SetBaseSkills()
{
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( mProfession );
  //Console->Print(YELLOW, BLACK, "PChar::SetBaseSkills() Profession: %d",def->GetIndex());
  if ( def == NULL )
  {
    Console->Print( RED, BLACK, "PChar::SetBaseSkills: GetCharKindDef=NULL" );
    return;
  }
  Skill->SetMainSkill( MS_INT, def->GetSkillInfo( MS_INT ).mStart );
  Skill->SetMainSkill( MS_CON, def->GetSkillInfo( MS_CON ).mStart );
  Skill->SetMainSkill( MS_DEX, def->GetSkillInfo( MS_DEX ).mStart );
  Skill->SetMainSkill( MS_STR, def->GetSkillInfo( MS_STR ).mStart );
  Skill->SetMainSkill( MS_PSI, def->GetSkillInfo( MS_PSI ).mStart );
  // management of SP needed ? NC Client seem to calculate what remains ...
  // or put SP setting after subskill setting ?
  /* Skill->SetSP(MS_INT, (short) ??? ));
  Skill->SetSP(MS_CON, ((short) ??? ));
  Skill->SetSP(MS_DEX, (short) ??? ));
  Skill->SetSP(MS_STR, (short) ??? ));
  Skill->SetSP(MS_PSI, ((short) ??? )); */
  // what about XPs ?
  /* Skill->SetXP(MS_INT, ??? ));
  Skill->SetXP(MS_CON, ??? ));
  Skill->SetXP(MS_DEX, ??? ));
  Skill->SetXP(MS_STR, ??? ));
  Skill->SetXP(MS_PSI, ??? )); */
  Console->Print( YELLOW, BLACK, "PChar::SetBaseSkills() not fully functionnal - unused skill points will be lost" );
}

void PChar::SetBaseSubskills( u8 NSZNb, const char* NonZeroSubskills )
{
  int i;

  if ( NSZNb == 0 )
    return;

  for ( i = 0; i < NSZNb; i++ )
  {
    Skill->SetSubSkill(( SUB_SKILLS ) NonZeroSubskills[2 * i], ( int ) NonZeroSubskills[2 * i +1] );
  }
}

void PChar::SetBaseInventory()
{
  u8 i;
  u32 BaseItemID;
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( mProfession );

  //mCash = 5000;
  mCash = def->GetStartMoney();

  for ( i = 0; i < 8 ; i++ )
  {
    BaseItemID = def->GetStartInventory( i );
    if ( BaseItemID )
    {
      if ( gDevDebug ) Console->Print( GREEN, BLACK, "Adding item %d to base inventory", BaseItemID );
      PItem* NewItem = new PItem( BaseItemID );
      if ( NewItem->GetItemID() )
      {
        NewItem->MakeItemStandard( 120, 180 ); // global quality range
        mInventory.AddItem( NewItem );
      }
      else
      {
        Console->Print( RED, BLACK, "Invalid item ID %d in base inventory for profession %d", BaseItemID, mProfession );
      }
    }
  }
}

void PChar::LoadClanLevel()
{
    MYSQL_RES *result;
    char query[200];

    snprintf(query, 200, "SELECT cll_level FROM clanlevels WHERE cll_charid = %d AND cll_clanid = %d", mID, mClanID);
    result = MySQL->GameResQuery( query );
    if ( result == NULL )
    {
        mClanID = 0;
        Console->Print( RED, BLACK, "PChar::GetClanLevel could not load ClanLevel from the database" );
        Console->Print( "Query was:" );
        Console->Print( "%s", query );
        MySQL->ShowGameSQLError();
        return;
    }
    else if(mysql_num_rows(result) == 0)
    {
        mClanID = 0;
        Console->Print( RED, BLACK, "PChar::GetClanLevel No level entry found for clan!" );
        return;
    }
    mClanLevel = atoi(mysql_fetch_row(result)[0]);
    if (gDevDebug) Console->Print("Loaded ClanLevel %d for char %d, clan %d", mClanLevel, mID, mClanID);
}

bool PChar::SQLLoad( int CharID )
{
  MYSQL_RES *result;
  MYSQL_ROW row;
  char query[1024];

  snprintf( query, 1024, "SELECT * FROM characters WHERE c_id = %d LIMIT 1", CharID );
  result = MySQL->GameResQuery( query );
  if ( result == NULL )
  {
    Console->Print( RED, BLACK, "PChar::SQLLoad could not load Chardata from the database" );
    Console->Print( "Query was:" );
    Console->Print( "%s", query );
    MySQL->ShowGameSQLError();
    return false;
  }
  if (( row = mysql_fetch_row( result ) ) )
  {
    SetID( CharID );
    SetName( row[c_name] );
    SetAccount( atoi( row[a_id] ) );

    // Gender
    int genvalue = atoi( row[c_sex] );
    if (( genvalue == 0 ) || ( genvalue == 1 ) )
      mGender = static_cast<u32>( genvalue );
    else
    {
      Console->Print( RED, BLACK, "Bad gender value: %d (Char ID %d)", genvalue, mID );
      mGender = 0;
    }

    // Profession
    int profvalue = atoi( row[c_profession] );
    SetProfession( static_cast<u32>( profvalue ) );

    // Class
    //int classvalue = atoi(row[c_class]);
    //if(classvalue < 4)
    //    mClass = static_cast<u32>(classvalue);
    //else
    //{
    //  Console->Print(RED, BLACK, "Bad class value: %d (Char ID %d)", classvalue, mID);
    //  classvalue = 0;
    //}

    // Faction
    int facvalue = atoi( row[c_faction] );
    if ( GameDefs->Factions()->GetDef( facvalue ) )
      mFaction = static_cast<u32>( facvalue );
    else
      mFaction = 1;

    /* // Model
    int modvalue = atoi(row[c_model]);
    mModel = static_cast<u32>(modvalue);
    mModel = 10; */
    int headvalue = atoi( row[c_head] );
    int torsovalue = atoi( row[c_torso] );
    int legsvalue = atoi( row[c_legs] );
    SetRealLook( static_cast<u32>( headvalue ), static_cast<u32>( torsovalue ), static_cast<u32>( legsvalue ) );

    // Type
    /*
    int typevalue = std::atoi(row[c_type]);
    mType = static_cast<u32>(typevalue);
    //mType = 1; */

    // Location
    int locvalue = atoi( row[c_location] );
    mLocation = static_cast<u32>( locvalue );

//This assumption is not so good ... hardcoding jailed state linked with location ...
    if ( mLocation == 550 || mLocation == 551 )
      mJailed = true;

    int posvalue = atoi( row[c_pos_x] );
    Coords.mX = static_cast<u16>( posvalue );
    posvalue = atoi( row[c_pos_y] );
    Coords.mY = static_cast<u16>( posvalue );
    posvalue = atoi( row[c_pos_z] );
    Coords.mZ = static_cast<u16>( posvalue );
    posvalue = atoi( row[c_angle_ud] );
    Coords.mUD = static_cast<u8>( posvalue );
    posvalue = atoi( row[c_angle_lr] );
    Coords.mLR = static_cast<u8>( posvalue );

    int primapt = atoi( row[c_apt] );
    mPrimaryApt = static_cast<u32>( primapt );
    mStartApt = mPrimaryApt;

    mSoullight = atoi( row[c_soullight] );
    mClanID = atoi( row[c_clan] );
    if(mClanID > 0)
        LoadClanLevel();

    // Cash
    f32 cashvalue = atof( row[c_cash] );
    mCash = static_cast<u32>( cashvalue );

    // ---------------------------------------------
    // Loading skills --- MAIN Skills with SP and XP
    // ---------------------------------------------
    Skill->SetMainSkill( MS_INT, atoi( row[c_int_lvl] ) );
    Skill->SetMainSkill( MS_CON, atoi( row[c_con_lvl] ) );
    Skill->SetMainSkill( MS_DEX, atoi( row[c_dex_lvl] ) );
    Skill->SetMainSkill( MS_STR, atoi( row[c_str_lvl] ) );
    Skill->SetMainSkill( MS_PSI, atoi( row[c_psi_lvl] ) );
    // ---------------------------------------------
    Skill->SetSP( MS_INT, ( short )atoi( row[c_int_pts] ) );
    Skill->SetSP( MS_CON, ( short )atoi( row[c_con_pts] ) );
    Skill->SetSP( MS_DEX, ( short )atoi( row[c_dex_pts] ) );
    Skill->SetSP( MS_STR, ( short )atoi( row[c_str_pts] ) );
    Skill->SetSP( MS_PSI, ( short )atoi( row[c_psi_pts] ) );
    // ---------------------------------------------
    Skill->SetXP( MS_INT, atof( row[c_int_xp] ) );
    Skill->SetXP( MS_CON, atof( row[c_con_xp] ) );
    Skill->SetXP( MS_DEX, atof( row[c_dex_xp] ) );
    Skill->SetXP( MS_STR, atof( row[c_str_xp] ) );
    Skill->SetXP( MS_PSI, atof( row[c_psi_xp] ) );
    // ---------------------------------------------
    // SubSkills
    // ---------------------------------------------
    Skill->SetSubSkill( SK_MC, atoi( row[c_mc] ) );
    Skill->SetSubSkill( SK_HC, atoi( row[c_hc] ) );
    Skill->SetSubSkill( SK_TRA, atoi( row[c_tra] ) );
    Skill->SetSubSkill( SK_FOR, atoi( row[c_for] ) );
    Skill->SetSubSkill( SK_PC, atoi( row[c_pc] ) );
    Skill->SetSubSkill( SK_RC, atoi( row[c_rc] ) );
    Skill->SetSubSkill( SK_TC, atoi( row[c_tc] ) );
    Skill->SetSubSkill( SK_VHC, atoi( row[c_vhc] ) );
    Skill->SetSubSkill( SK_AGL, atoi( row[c_agl] ) );
    Skill->SetSubSkill( SK_REP, atoi( row[c_rep] ) );
    Skill->SetSubSkill( SK_REC, atoi( row[c_rec] ) );
    Skill->SetSubSkill( SK_RCL, atoi( row[c_rcl] ) );
    Skill->SetSubSkill( SK_ATL, atoi( row[c_atl] ) );
    Skill->SetSubSkill( SK_END, atoi( row[c_end] ) );
    Skill->SetSubSkill( SK_FIR, atoi( row[c_fir] ) );
    Skill->SetSubSkill( SK_ENR, atoi( row[c_enr] ) );
    Skill->SetSubSkill( SK_XRR, atoi( row[c_xrr] ) );
    Skill->SetSubSkill( SK_POR, atoi( row[c_por] ) );
    Skill->SetSubSkill( SK_HLT, atoi( row[c_hlt] ) );
    Skill->SetSubSkill( SK_HCK, atoi( row[c_hck] ) );
    Skill->SetSubSkill( SK_BRT, atoi( row[c_brt] ) );
    Skill->SetSubSkill( SK_PSU, atoi( row[c_psu] ) );
    Skill->SetSubSkill( SK_WEP, atoi( row[c_wep] ) );
    Skill->SetSubSkill( SK_CST, atoi( row[c_cst] ) );
    Skill->SetSubSkill( SK_RES, atoi( row[c_res] ) );
    Skill->SetSubSkill( SK_IMP, atoi( row[c_imp] ) );
    Skill->SetSubSkill( SK_PPU, atoi( row[c_ppu] ) );
    Skill->SetSubSkill( SK_APU, atoi( row[c_apu] ) );
    Skill->SetSubSkill( SK_MST, atoi( row[c_mst] ) );
    Skill->SetSubSkill( SK_PPW, atoi( row[c_ppw] ) );
    Skill->SetSubSkill( SK_PSR, atoi( row[c_psr] ) );
    Skill->SetSubSkill( SK_WPW, atoi( row[c_wpw] ) );
    // --------------------------------------------
    // Inventory
    // ---------------------------------------------
    mInventory.SetCharId( mID );
    mInventory.SQLLoad();

    // temp value forcing, not get/saved from DB atm
    mCombatRank = ( u8 )( random() % 127 ); // bad result there on randomness
    mSynaptic = 0;
    mIsDead = false;

    mDirectCharID = 0; // until saved/loaded with char
    mBuddyList = new PBuddyList( mID );
    if ( !mBuddyList->SQLLoad() )
    {
      Console->Print( RED, BLACK, "Char ID %d : Can't load buddy list", mID );
    }
    //to add: Chats settings

    mGenrepList = new PGenrepList( mID );
    if ( !mGenrepList->SQLLoad() )
    {
      Console->Print( RED, BLACK, "Char ID %d : Can't load genrep list", mID );
    }

  }
  MySQL->FreeGameSQLResult( result );

  // Addition; Set char's online status to ONLINE
  char sqlqry[50];
  snprintf(sqlqry, 50, "UPDATE characters SET c_online = 1 WHERE c_id = %d", mID);
  MySQL->GameQuery(sqlqry);

  return true;
}

bool PChar::SQLCreate() // Specific method for creation in order to avoid existence check with each save
{
  std::string query, queryv;

  query = "INSERT INTO characters (c_id";
  queryv = ") VALUES (NULL";

  query += ",c_name";
  queryv = queryv + ",'" + mName + "'";

  query += ",a_id";
  queryv += Ssprintf( ",'%u'", mAccount );
  query += ",c_class";
  queryv += Ssprintf( ",'%u'", mClass );
  query += ",c_sex";
  queryv += Ssprintf( ",'%u'", mGender );
  query += ",c_profession";
  queryv += Ssprintf( ",'%u'", mProfession );
  query += ",c_faction";
  queryv += Ssprintf( ",'%u'", mFaction );
  query += ",c_head";
  queryv += Ssprintf( ",'%u'", mRealHead );
  query += ",c_torso";
  queryv += Ssprintf( ",'%u'", mRealTorso );
  query += ",c_legs";
  queryv += Ssprintf( ",'%u'", mRealLegs );
  //query += ",c_model";
  //queryv += Ssprintf(",'%u'", mModel);
  //query += ",c_type";
  //queryv += Ssprintf(",'%u'", mType);
  query += ",c_location";
  queryv += Ssprintf( ",'%u'", mLocation );
  query += ",c_cash";
  queryv += Ssprintf( ",'%u'", mCash );
  query += ",c_slot";
  queryv += Ssprintf( ",'%u'", mSlot );

  query = query + queryv + ");";

  if ( MySQL->GameQuery( query.c_str() ) )
  {
    Console->Print( RED, BLACK, "PChar::SQLCreate could not add char %s to database", mName.c_str() );
    Console->Print( "Query was:" );
    Console->Print( "%s", query.c_str() );
    MySQL->ShowGameSQLError();
    return false;
  }
  else
  {
    mID = MySQL->GetLastGameInsertId();
//Console->Print(GREEN, BLACK, "New char %s got ID %d", mName.c_str(), mID);
    mDirtyFlag = true;
    return true;
  }
}

bool PChar::CreateNewChar( u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
                           u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot )
{
  SetName( Name );
  SetGender( Gender );
  SetProfession( Profession );
  SetFaction( Faction );
  SetRealLook( Head, Torso, Legs );
  SetBaseSkills();
  SetBaseSubskills( NZSNb, NonZeroSubskills );
  SetBaseInventory();
  SetAccount( Account );
  SetCharSlot( Slot );
  mLocation = Config->GetOptionInt( "new_char_location" );

  // This part will have to be rewritten with proper methods
  mSoullight = 10;
  mCombatRank = ( u8 )( random() % 80 ); // bad result there on randomness
  mSynaptic = 0;
  mIsDead = false;

  mDirectCharID = 0; // until saved/loaded with char

  SetDirtyFlag();

  if ( SQLCreate() ) // mID isn't defined before that
  {
    mBuddyList = new PBuddyList( mID );
    mGenrepList = new PGenrepList( mID );
    mStartApt = mPrimaryApt = Appartements->CreateBaseAppartement( mID, mName, mFaction );
    mInventory.SetCharId( mID );

    if ( mStartApt && SQLSave() && mInventory.SQLSave() )
    {
      return true;
    }
    else
    {
      Console->Print( YELLOW, BLACK, "New char %s (id %d) : creation aborted", mName.c_str(), mID );
      if ( mID )
      {
        SQLDelete();
        if ( mStartApt )
        {
          Appartements->DeleteCharAppartements( mID );
        }
      }
    }
  }
  return false;
}

bool PChar::SQLSave()
{
  std::string query;
  //std::string ts;

  /* TODO:
    - Mostly at creation/load :
              c_apt, (or when first GR to primary apt to avoid creation of unused apt?)
              (c_slot)
    - At save/load :
              SoulLight ???
              FactionSymp[] ???
              Chest change: style, brightness, color
              Legs change: style, brightness, color
              mHealt, mStamina, mMana (not in DB !!!)
              How to compute MaxHealth etc. ?
  */
  query = "UPDATE characters SET";

  query += Ssprintf( " c_location='%u'", mLocation );
  query += Ssprintf( ",c_pos_x='%u'", Coords.mX );
  query += Ssprintf( ",c_pos_y='%u'", Coords.mY );
  query += Ssprintf( ",c_pos_z='%u'", Coords.mZ );
  query += Ssprintf( ",c_angle_ud='%u'", Coords.mUD );
  query += Ssprintf( ",c_angle_lr='%u'", Coords.mLR );
  query += Ssprintf( ",c_cash='%u'", mCash );
  query += Ssprintf( ",c_apt='%u'", mPrimaryApt );

  query += Ssprintf( ",c_head='%u'", mRealHead );
  query += Ssprintf( ",c_torso='%u'", mRealTorso );
  query += Ssprintf( ",c_legs='%u'", mRealLegs );

  query += Ssprintf( ",c_faction='%u'", mFaction );

  /* This group of fiels shouldn't change in-game
  query = query + ",c_name='" + mName + "'";
  query += Ssprintf(",a_id='%u'", mAccount);
  query += Ssprintf(",c_class='%u'", mClass);
  query += Ssprintf(",c_sex='%u'", mGender);
  query += Ssprintf(",c_profession='%u'", mProfession);
  query += Ssprintf(",c_slot='%u'", mSlot);
  // query += Ssprintf(",c_model='%u'", mModel);
  // query += Ssprintf(",c_type='%u'", mType);
  */

  // ---------------------------------------------
  // Saving skills --- MAIN Skills with SP and XP
  // ---------------------------------------------
  query += Ssprintf( ",c_int_lvl='%u'", Skill->GetMainSkill( MS_INT ) );
  query += Ssprintf( ",c_con_lvl='%u'", Skill->GetMainSkill( MS_CON ) );
  query += Ssprintf( ",c_dex_lvl='%u'", Skill->GetMainSkill( MS_DEX ) );
  query += Ssprintf( ",c_str_lvl='%u'", Skill->GetMainSkill( MS_STR ) );
  query += Ssprintf( ",c_psi_lvl='%u'", Skill->GetMainSkill( MS_PSI ) );
  // ---------------------------------------------
  query += Ssprintf( ",c_int_pts='%u'", Skill->GetSP( MS_INT ) );
  query += Ssprintf( ",c_con_pts='%u'", Skill->GetSP( MS_CON ) );
  query += Ssprintf( ",c_dex_pts='%u'", Skill->GetSP( MS_DEX ) );
  query += Ssprintf( ",c_str_pts='%u'", Skill->GetSP( MS_STR ) );
  query += Ssprintf( ",c_psi_pts='%u'", Skill->GetSP( MS_PSI ) );
  // ---------------------------------------------
  query += Ssprintf( ",c_int_xp='%u'", Skill->GetXP( MS_INT ) );
  query += Ssprintf( ",c_con_xp='%u'", Skill->GetXP( MS_CON ) );
  query += Ssprintf( ",c_dex_xp='%u'", Skill->GetXP( MS_DEX ) );
  query += Ssprintf( ",c_str_xp='%u'", Skill->GetXP( MS_STR ) );
  query += Ssprintf( ",c_psi_xp='%u'", Skill->GetXP( MS_PSI ) );
  // ---------------------------------------------
  // SubSkills
  // ---------------------------------------------
  query += Ssprintf( ",c_mc='%u'", Skill->GetSubSkill( SK_MC ) );
  query += Ssprintf( ",c_hc='%u'", Skill->GetSubSkill( SK_HC ) );
  query += Ssprintf( ",c_tra='%u'", Skill->GetSubSkill( SK_TRA ) );
  query += Ssprintf( ",c_for='%u'", Skill->GetSubSkill( SK_FOR ) );
  query += Ssprintf( ",c_pc='%u'", Skill->GetSubSkill( SK_PC ) );
  query += Ssprintf( ",c_rc='%u'", Skill->GetSubSkill( SK_RC ) );
  query += Ssprintf( ",c_tc='%u'", Skill->GetSubSkill( SK_TC ) );
  query += Ssprintf( ",c_vhc='%u'", Skill->GetSubSkill( SK_VHC ) );
  query += Ssprintf( ",c_agl='%u'", Skill->GetSubSkill( SK_AGL ) );
  query += Ssprintf( ",c_rep='%u'", Skill->GetSubSkill( SK_REP ) );
  query += Ssprintf( ",c_rec='%u'", Skill->GetSubSkill( SK_REC ) );
  query += Ssprintf( ",c_rcl='%u'", Skill->GetSubSkill( SK_RCL ) );
  query += Ssprintf( ",c_atl='%u'", Skill->GetSubSkill( SK_ATL ) );
  query += Ssprintf( ",c_end='%u'", Skill->GetSubSkill( SK_END ) );
  query += Ssprintf( ",c_fir='%u'", Skill->GetSubSkill( SK_FIR ) );
  query += Ssprintf( ",c_enr='%u'", Skill->GetSubSkill( SK_ENR ) );
  query += Ssprintf( ",c_xrr='%u'", Skill->GetSubSkill( SK_XRR ) );
  query += Ssprintf( ",c_por='%u'", Skill->GetSubSkill( SK_POR ) );
  query += Ssprintf( ",c_htl='%u'", Skill->GetSubSkill( SK_HLT ) );
  query += Ssprintf( ",c_hck='%u'", Skill->GetSubSkill( SK_HCK ) );
  query += Ssprintf( ",c_brt='%u'", Skill->GetSubSkill( SK_BRT ) );
  query += Ssprintf( ",c_psu='%u'", Skill->GetSubSkill( SK_PSU ) );
  query += Ssprintf( ",c_wep='%u'", Skill->GetSubSkill( SK_WEP ) );
  query += Ssprintf( ",c_cst='%u'", Skill->GetSubSkill( SK_CST ) );
  query += Ssprintf( ",c_res='%u'", Skill->GetSubSkill( SK_RES ) );
  query += Ssprintf( ",c_imp='%u'", Skill->GetSubSkill( SK_IMP ) );
  query += Ssprintf( ",c_ppu='%u'", Skill->GetSubSkill( SK_PPU ) );
  query += Ssprintf( ",c_apu='%u'", Skill->GetSubSkill( SK_APU ) );
  query += Ssprintf( ",c_mst='%u'", Skill->GetSubSkill( SK_MST ) );
  query += Ssprintf( ",c_ppw='%u'", Skill->GetSubSkill( SK_PPW ) );
  query += Ssprintf( ",c_psr='%u'", Skill->GetSubSkill( SK_PSR ) );
  query += Ssprintf( ",c_wpw='%u'", Skill->GetSubSkill( SK_WPW ) );
  // ---------------------------------------------

  query += Ssprintf( " WHERE c_id='%u' LIMIT 1;", mID );

  if ( MySQL->GameQuery( query.c_str() ) )
  {
    Console->Print( RED, BLACK, "PChar::SQLSave could not save char %s (%u) to database", mName.c_str(), mID );
    Console->Print( "Query was:" );
    Console->Print( "%s", query.c_str() );
    MySQL->ShowGameSQLError();
    return false;
  }

  // Chats settings (?), directs

  mDirtyFlag = false;
  return true;
}

bool PChar::SQLDelete()
{
  return true;
}

void PChar::SetOnlineStatus( bool IsOnline )
{
  // Deactivated, until Maxx added c_isonline row to `characters`
  //char query[255];
  int onlinestatus = 0;

  if ( IsOnline )
  {
    onlinestatus = 0; // Strange ????
    mIsOnline = true;
  }
  else
  {
    onlinestatus = 1; // Strange ????
    mIsOnline = false;
  }

//    snprintf(query, 255, "UPDATE charlist SET c_isonline = %d WHERE a_id = %d AND c_id = %d", onlinestatus, mAccount, mID);
//    if(MySQL->Query(query))
//    {
//        Console->Print("Error: Cant set onlinestatus to '%d' for Account: %d, Char: %d", onlinestatus, mAccount, mID);
//        MySQL->ShowSQLError();
//        return;
//    }
  return;
}

u8 PChar::GetCombatRank()
{
    // Override for Special Account Levels
    PAccount Acc(mAccount);
    if(Acc.GetLevel() == PAL_ADMIN)
        return 127;
    else if(Acc.GetLevel() >= PAL_GM)
        return 120;
    else if(Acc.GetLevel() >= PAL_VOLUNTEER)
        return 50;
    else
        return mCombatRank;
}


u8 PChar::GetMainRank()
{
    // Override for Special Account Levels
    PAccount Acc(mAccount);
    if(Acc.GetLevel() == PAL_ADMIN)
        return 127;
    else if(Acc.GetLevel() >= PAL_GM)
        return 120;
    else if(Acc.GetLevel() >= PAL_VOLUNTEER)
        return 50;
    else
    {
      u16 total;
      total  = Skill->GetMainSkill( MS_STR ) + Skill->GetMainSkill( MS_DEX );
      total += Skill->GetMainSkill( MS_CON ) + Skill->GetMainSkill( MS_INT );
      total += Skill->GetMainSkill( MS_PSI );
      return (( u8 )( total / 5 ) );
    }
}

u32 PChar::AddCash( u32 nAmount )
{
    return SetCash(nAmount + mCash);
}

u32 PChar::TakeCash( u32 nAmount )
{
    if(nAmount > mCash)
    {
        //Tries to take away more cash that user has, set to zero
        return SetCash(0);
    }
    else
    {
        return SetCash(mCash-nAmount);
    }
}

u32 PChar::SetCash( u32 nCash )
{
  //Console->Print("Trying to set cash to nCash: %d", nCash);
  if (( int )nCash > Config->GetOptionInt( "max_cash" ) )
  {
    //Console->Print("Newcash would be more than dynamic maxcash, setting to maxcash");
    mCash = ( u32 )Config->GetOptionInt( "max_cash" );
  }
  else if ( nCash > MAXCASH )
  {
    //Console->Print("Newcash would be more than hardcoded maxcash, setting to maxcash");
    mCash = MAXCASH;
  }
  else
  {
    //Console->Print("Allright, setting to new value");
    mCash = nCash;
  }
  //Console->Print("Returning mCash: %d", mCash);
  return mCash;
}


bool PChar::SetQuickBeltActiveSlot( u8 nSlotID )
{
  if (( nSlotID == INV_WORN_QB_HAND ) || ( nSlotID == INV_WORN_QB_NONE ) )
  {
    mQuickBeltActiveSlot = nSlotID;
    return true;
  }
  else if ( nSlotID <= ( INV_WORN_QB_END - INV_WORN_QB_START ) )
  {
    PContainer* tWorn = mInventory.GetContainer( INV_LOC_WORN );

    if ( ! tWorn->IsSlotFree( nSlotID ) ) // => TODO: MUST ALSO CHECK that item is currently usable and can be held in hand
    {
      mQuickBeltActiveSlot = nSlotID;
      return true;
    }
    else
    {
      Console->Print( "SetQuickBeltActiveSlot: SlotID %d greater than %d or free (%d)", nSlotID, INV_WORN_QB_END - INV_WORN_QB_START, tWorn->IsSlotFree( nSlotID ) );
    }
  }
  return false;
}

PSeatType PChar::GetSeatInUse( u32* nObjectId, u8* nSeatId )
{
  if ( nObjectId )
  {
    *nObjectId = mSeatInUseObjectId;
  }
  if ( nSeatId )
  {
    *nSeatId = mSeatInUseSeatId;
  }

  return mSeatInUseType;
}

void PChar::SetSeatInUse( PSeatType nSeatType, u32 nObjectId, u8 nSeatId )
{
  mSeatInUseType = nSeatType;
  mSeatInUseObjectId = nObjectId;
  mSeatInUseSeatId = nSeatId;
}

PVhcAccessRequestList* PChar::GetVhcAccessRequestList()
{
  if( ! mVhcAccessRequestList )
    mVhcAccessRequestList = new PVhcAccessRequestList();

  return mVhcAccessRequestList;
}

void PChar::SetLookingAt( u16 nLocalCharID )
{
  mLookingAt = nLocalCharID;
  mLookAtTimestamp = std::time( NULL );
}

u16 PChar::GetLookingAt( u16 nMaxDelaySec )
{
  return ((( mLookAtTimestamp + nMaxDelaySec ) >= std::time( NULL ) ) ? mLookingAt : 0 );
}

// ===================================

PChars::PChars()
{
  mLastID = 0;
  mLastSave = std::time( NULL );

  mAutoSavePeriod = Config->GetOptionInt( "auto_save_period" );
  if ( mAutoSavePeriod < 0 )
  {
    Console->Print( "%s auto_save_period (%d) must be strict positive.", Console->ColorText( RED, BLACK, "[Error]" ), mAutoSavePeriod );
    mAutoSavePeriod = 0;
  }
  else if ( mAutoSavePeriod > 3600 )
  {
    Console->Print( "%s auto_save_period (%d) too high. Forced to 3600 sec.", Console->ColorText( YELLOW, BLACK, "[Warning]" ), mAutoSavePeriod );
    mAutoSavePeriod = 0;
  }

  if ( mAutoSavePeriod == 0 )
  {
    Console->Print( "%s Auto-save disabled.", Console->ColorText( YELLOW, BLACK, "[Info]" ) );
  }
  else if ( mAutoSavePeriod < 60 )
  {
    Console->Print( "%s auto_save_period (%d) is low and might lead to high server load.", Console->ColorText( YELLOW, BLACK, "[Warning]" ), mAutoSavePeriod );
  }

}

PChars::~PChars()
{
  for ( CharMap::iterator i = mChars.begin(); i != mChars.end(); i++ )
    delete i->second;
}

bool PChars::LoadChar( u32 CharID )
{
  if ( !CharID )
    return false;

  PChar *nChar = new PChar();
  if ( nChar->SQLLoad( CharID ) )
  {
    nChar->SetDirtyFlag( false );
    return AddChar( nChar );
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] Could not load char id %d from database", CharID );
    return false;
  }
}

bool PChars::AddChar( PChar* nChar )
{
  if ( !nChar )
    return false;

  mLastID = max( mLastID, nChar->GetID() );
  if ( mChars.insert( std::make_pair( nChar->GetID(), nChar ) ).second )
  {
    if ( gDevDebug )
      Console->Print( "%s Char: %s (id %d) added", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nChar->GetName().c_str() , nChar->GetID() );
    return true;
  }
  else
  {
    Console->Print( RED, BLACK, "[ERROR] Trying to load char twice : %s (id %d)", nChar->GetName().c_str(), nChar->GetID() );
    return false;
  }
}

PChar* PChars::RemoveChar( u32 CharID )
{
  PChar* Result = NULL;

  CharMap::iterator i = mChars.find( CharID );
  if ( i != mChars.end() )
  {
    Result = i->second;
    mChars.erase( i );
    if ( gDevDebug )
      Console->Print( "%s Char: %s (id %d) removed", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), Result->GetName().c_str() , Result->GetID() );
  }

  return Result;
}

void PChars::SQLSave()
{
  // saves all dirty-flagged chars
  int nDirtyChars = 0, nSavedChars = 0;
  int nDirtyInv = 0, nSavedInv = 0;

  for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
  {
    PChar* Char = i->second;
    if ( Char->IsDirty() )
    {
      ++nDirtyChars;
      if ( Char->SQLSave() )
        ++nSavedChars;
    }
    if ( Char->GetInventory()->IsDirty() )
    {
      ++nDirtyInv;
      if ( Char->GetInventory()->SQLSave() )
        ++nSavedInv;
    }
  }
  Console->Print( "%s %i chars saved on %i dirty, %i inventories saved on %i dirty", Console->ColorText( GREEN, BLACK, "[DEBUG]" ), nSavedChars, nDirtyChars, nSavedInv, nDirtyInv );
  return;
}

PChar* PChars::GetChar( u32 CharID ) const
{
  PChar *Result = 0;
  CharMap::const_iterator i = mChars.find( CharID );
  if ( i != mChars.end() )
    Result = i->second;

  return Result;
}

PChar* PChars::GetChar( const std::string &Name ) const
{
  PChar *Result = 0;
  for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
  {
    if ( !/*std::*/strcasecmp( i->second->GetName().c_str(), Name.c_str() ) )
    {
      Result = i->second;
      break;
    }
  }
  return Result;
}

bool PChars::CharExist( const std::string &Name ) const
{
  char query[256];
  int EntriesNb;
  MYSQL_RES *result = 0;

  char escUsername[256];
  MySQL->EscapeString( Name.c_str(), escUsername, 256 );
  snprintf( query, 256, "SELECT 1 FROM characters WHERE c_name = '%s' LIMIT 1;", escUsername );

  result = MySQL->GameResQuery( query );
  if ( result == NULL )
  {
    Console->Print( RED, BLACK, "[ERROR] Failed to get CharacterData from SQL" );
    MySQL->ShowGameSQLError();
    return true;
  }

  EntriesNb = mysql_num_rows( result );
  MySQL->FreeGameSQLResult( result );
  return( EntriesNb > 0 );
}

void PChars::Update()
{
  std::time_t t = std::time( NULL ); // changed to time() to have real time instead of cpu used time

  if ( mAutoSavePeriod && (( t - mLastSave ) >= mAutoSavePeriod ) )
  {
    bool NeedSave = false;
    for ( CharMap::const_iterator i = mChars.begin(); i != mChars.end(); i++ )
    {
      if ( i->second->IsAnyDirty() )
      {
        NeedSave = true;
        break;
      }
    }

    if ( NeedSave )
    {
      if ( gDevDebug ) Console->Print( "%s Some characters need autosaving...", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
      SQLSave();
      if ( gDevDebug ) Console->Print( "%s Autosave done.", Console->ColorText( CYAN, BLACK, "[DEBUG]" ) );
    }
    mLastSave = t;
  }
}

//MAX_CHARS_PER_ACCOUNT
int PChars::GetCharProfiles( const u32 AccountID, PCharProfile* CharSlotsArray, const u8 ArraySize )
{
  char query[256];
  int EntriesNb = 0;

  MYSQL_ROW row = 0;
  MYSQL_RES *result = 0;

  snprintf( query, 256, "SELECT * FROM characters WHERE a_id = %d ORDER BY c_slot ASC", AccountID );

  result = MySQL->GameResQuery( query );
  if ( result == NULL )
  {
    Console->Print( RED, BLACK, "[ERROR] Failed to load CharacterData from SQL" );
    MySQL->ShowGameSQLError();
    return 0;
  }

  //EntriesNb = mysql_num_rows(result);
  int SlotID;
  u32 CharID;
  PChar* tmpChar = new PChar();

  while (( row = mysql_fetch_row( result ) ) )
  {
    SlotID = atoi( row[c_slot] );
    CharID = atoi( row[c_id] );
    if (( SlotID >= 0 ) && ( SlotID < ArraySize ) )
    {
      if ( !CharSlotsArray[SlotID].in_use )
      {
        tmpChar->SetID( CharID );
        tmpChar->SetGender( atoi( row[c_sex] ) );
        tmpChar->SetProfession( atoi( row[c_profession] ) );

        CharSlotsArray[SlotID].CharID = CharID;
        CharSlotsArray[SlotID].Type = tmpChar->GetType();
        CharSlotsArray[SlotID].Location = static_cast<u32>( atoi( row[c_location] ) );
        CharSlotsArray[SlotID].Head = atoi( row[c_head] );
        CharSlotsArray[SlotID].Torso = atoi( row[c_torso] );
        CharSlotsArray[SlotID].Legs = atoi( row[c_legs] );
        CharSlotsArray[SlotID].Name = row[c_name];
        CharSlotsArray[SlotID].NameLen = CharSlotsArray[SlotID].Name.length() + 1;

        CharSlotsArray[SlotID].in_use = true;
        ++EntriesNb;
      }
      else
      {
        Console->Print( YELLOW, BLACK, "[Warning]Character %d using slot %d already used by char %d - Ignored", \
                        CharID, SlotID, CharSlotsArray[SlotID].CharID );
      }
    }
    else
    {
      Console->Print( YELLOW, BLACK, "[Warning]Character %d using invialid slot %d - Ignored", CharID, SlotID );
    }
  }

  delete tmpChar;
  MySQL->FreeGameSQLResult( result );
  return EntriesNb;
}
