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
	REASON:   - PChar::CreateNewChar() and moved effective char creation from PChars to PChar
	          - added PChar::SQLDelete()
	            This method is put here because we want the char to be loaded when deleted from DB to avoid
	            any player to use it at the same time.
	          - added use of auto_save_period config option in PChars::update()
	          - removed old XML-storage related code

  TODO:     - implement PChar::SQLDelete()
*/

#include "main.h"
#include "worlds.h"
#include "appartements.h"

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

	mSpeedOverride = 255; // means no override. Value 0 can be used to forbid any move.

	mLocationLeased = false; // temp until char on-demand load/unload
	mLocation = 0;
	mStartApt=0;
	mPrimaryApt=0;
	mCash = 0;
	mChairInUse = 0;

  mIsOnline = false;
  mDirtyFlag = false;

    mShunned = false;
    mJailed = false;

	Skill = new PSkillHandler();
	mBuddyList = NULL;
	mGenrepList = NULL;

	// Required for initial OOC Broadcast welcome message.
	//Gets overwritten as soon as the first PingPacket arrives
	mActiveChatChannels = 262144;
}

PChar::~PChar()
{
  delete Skill;
  if (mBuddyList)
    delete mBuddyList;
  if (mGenrepList)
    delete mGenrepList;
}

void PChar::SetProfession(u32 Profession)
{
  const PDefCharKind *def = GameDefs->GetCharKindDef(Profession);
  if (def == NULL)
  {
    Console->Print(RED, BLACK, "Char %d: Invalid profession %d", mID, Profession);
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

u32 PChar::GetSkinFromCharType(u32 nType)
{
  const PDefCharacter* nDefCharacter = GameDefs->GetCharDef(nType);
  if (nDefCharacter)
  {
    return ((u32)(nDefCharacter->GetModel()));
  }
  else
    return 0;
}

inline u32 PChar::GetBaseModel()
{
  return GetSkinFromCharType(GetType());
}

void PChar::SetRealLook(u32 nHead, u32 nTorso, u32 nLegs)
{
  mRealHead = nHead;
  mRealTorso = nTorso;
  mRealLegs = nLegs;
  SetDirtyFlag();
  ResetCurrentLook();
}

void PChar::GetRealLook (u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs)
{
  nSkin = GetBaseModel();
  nHead = mRealHead;
  nTorso = mRealTorso;
  nLegs = mRealLegs;
}

void PChar::SetCurrentLookFromCharType(u32 nType)
{
  int iHead, iTorso, iLegs;
  u32 nSkin, nHead, nTorso, nLegs;

  const PDefCharacter* nDefCharacter = GameDefs->GetCharDef(nType);
  if (nDefCharacter)
  {
    nSkin = (u32) nDefCharacter->GetModel();
    iHead = nDefCharacter->GetHead();
    iTorso = nDefCharacter->GetTorso();
    iLegs = nDefCharacter->GetLegs();

    if ((iHead < 0) || (iTorso < 0) || (iLegs < 0))
    {
      // do something !
      nHead = nTorso = nLegs = 0xff;
    }
    else
    {
      nHead = iHead % 10;
      nTorso = iTorso % 10;
      nLegs = iLegs % 10;
    }

    SetCurrentLook(nSkin, nHead, nTorso, nLegs);
  }
}

void PChar::SetCurrentLook(u32 nSkin, u32 nHead, u32 nTorso, u32 nLegs)
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
  SetCurrentLook(GetSkinFromCharType(GetType()), mRealHead, mRealTorso, mRealLegs);
}

// GetCurrentLook will later have to take Power Armors and GM overrides into account
void PChar::GetCurrentLook (u32 &nSkin, u32 &nHead, u32 &nTorso, u32 &nLegs)
{
  nSkin = mSkin;
  nHead = mHead;
  nTorso = mTorso;
  nLegs = mLegs;
}

void PChar::SetCurrentBodyColor(u8 nHeadColor, u8 nTorsoColor, u8 nLegsColor, u8 nHeadDarkness, u8 nTorsoDarkness, u8 nLegsDarkness)
{
  mHeadColor = nHeadColor;
  mTorsoColor = nTorsoColor;
  mLegsColor = nLegsColor;
  mHeadDarkness = nHeadDarkness;
  mTorsoDarkness = nTorsoDarkness;
  mLegsDarkness = nLegsDarkness;
}

void PChar::GetCurrentBodyColor(u8 &nHeadColor, u8 &nTorsoColor, u8 &nLegsColor, u8 &nHeadDarkness, u8 &nTorsoDarkness, u8 &nLegsDarkness)
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
  const PDefCharKind *def = GameDefs->GetCharKindDef(mProfession);
  //Console->Print(YELLOW, BLACK, "PChar::SetBaseSkills() Profession: %d",def->GetIndex());
  if (def == NULL)
  {
    Console->Print(RED, BLACK, "PChar::SetBaseSkills: GetCharKindDef=NULL");
    return;
  }
  Skill->SetMainSkill(MS_INT, def->GetSkillInfo(MS_INT).mStart);
  Skill->SetMainSkill(MS_CON, def->GetSkillInfo(MS_CON).mStart);
  Skill->SetMainSkill(MS_DEX, def->GetSkillInfo(MS_DEX).mStart);
  Skill->SetMainSkill(MS_STR, def->GetSkillInfo(MS_STR).mStart);
  Skill->SetMainSkill(MS_PSI, def->GetSkillInfo(MS_PSI).mStart);
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
  Console->Print(YELLOW, BLACK, "PChar::SetBaseSkills() not fully functionnal - unused skill points will be lost");
}

void PChar::SetBaseSubskills(u8 NSZNb, const char* NonZeroSubskills)
{
  int i;

  if (NSZNb == 0)
    return;

  for (i = 0; i < NSZNb; i++)
  {
     Skill->SetSubSkill((SUB_SKILLS) NonZeroSubskills[2 * i], (int) NonZeroSubskills[2 * i +1]);
  }
}

void PChar::SetBaseInventory()
{
  u8 i;
  u32 BaseItemID;
  const PDefCharKind *def = GameDefs->GetCharKindDef(mProfession);

  //mCash = 5000;
  mCash = def->GetStartMoney();

  for (i = 0; i < 8 ; i++)
  {
    BaseItemID = def->GetStartInventory(i);
    if (BaseItemID) {
if (gDevDebug) Console->Print(GREEN, BLACK, "Adding item %d to base inventory", BaseItemID);
      PItem* NewItem = new PItem(BaseItemID);
      if (NewItem->GetItemID())
        mInventory.PutItem(NewItem);
      else
        Console->Print(RED, BLACK, "Invalid item ID !");
    }
  }

Console->Print(YELLOW, BLACK, "Warning: Inventory are saved to DB at creation but not reloaded nor transfered ingame (for now)");
}

bool PChar::SQLLoad(int CharID) {
    MYSQL_RES *result;
    MYSQL_ROW row;
    char query[1024];

    sprintf(query, "SELECT * FROM characters WHERE c_id = %d LIMIT 1", CharID);
    result = MySQL->GameResQuery(query);
    if(result == NULL)
    {
        //Console->Print(RED, BLACK, "Unable to load data from MySQL DB!");
        return false;
    }
    while((row = mysql_fetch_row(result)))
    {
        SetID(CharID);
        SetName(row[1]);

        // Gender
        int genvalue = std::atoi(row[c_sex]);
        if((genvalue == 0) || (genvalue == 1))
            mGender = static_cast<u32>(genvalue);
        else
        {
          Console->Print(RED, BLACK, "Bad gender value: %d (Char ID %d)", genvalue, mID);
          mGender = 0;
        }

        // Profession
        int profvalue = std::atoi(row[c_profession]);
        SetProfession( static_cast<u32>(profvalue));

        // Class
        //int classvalue = std::atoi(row[c_class]);
        //if(classvalue < 4)
        //    mClass = static_cast<u32>(classvalue);
        //else
        //{
        //  Console->Print(RED, BLACK, "Bad class value: %d (Char ID %d)", classvalue, mID);
        //  classvalue = 0;
        //}

        // Faction
        int facvalue = std::atoi(row[c_faction]);
        if(GameDefs->GetFactionDef(facvalue))
            mFaction = static_cast<u32>(facvalue);
        else
            mFaction = 1;

        /* // Model
        int modvalue = std::atoi(row[c_model]);
        mModel = static_cast<u32>(modvalue);
        mModel = 10; */
        int headvalue = std::atoi(row[c_head]);
        int torsovalue = std::atoi(row[c_torso]);
        int legsvalue = std::atoi(row[c_legs]);
        SetRealLook(static_cast<u32>(headvalue), static_cast<u32>(torsovalue), static_cast<u32>(legsvalue));

        // Type
        /*
        int typevalue = std::atoi(row[c_type]);
        mType = static_cast<u32>(typevalue);
        //mType = 1; */

        // Location
        int locvalue = std::atoi(row[c_location]);
        mLocation = static_cast<u32>(locvalue);

        if(mLocation == 550 || mLocation == 551)
            mJailed = true;

        int posvalue = std::atoi(row[c_pos_x]);
        Coords.mX = static_cast<u16>(posvalue);
        posvalue = std::atoi(row[c_pos_y]);
        Coords.mY = static_cast<u16>(posvalue);
        posvalue = std::atoi(row[c_pos_z]);
        Coords.mZ = static_cast<u16>(posvalue);
        posvalue = std::atoi(row[c_angle_ud]);
        Coords.mUD = static_cast<u8>(posvalue);
        posvalue = std::atoi(row[c_angle_lr]);
        Coords.mLR = static_cast<u8>(posvalue);

        int primapt = std::atoi(row[c_apt]);
        mPrimaryApt = static_cast<u32>(primapt);
        mStartApt = mPrimaryApt;

        // Cash
        f32 cashvalue = std::atof(row[c_cash]);
        mCash = static_cast<u32>(cashvalue);

        // ---------------------------------------------
        // Loading skills --- MAIN Skills with SP and XP
        // ---------------------------------------------
        Skill->SetMainSkill(MS_INT, std::atoi(row[c_int_lvl]));
        Skill->SetMainSkill(MS_CON, std::atoi(row[c_con_lvl]));
        Skill->SetMainSkill(MS_DEX, std::atoi(row[c_dex_lvl]));
        Skill->SetMainSkill(MS_STR, std::atoi(row[c_str_lvl]));
        Skill->SetMainSkill(MS_PSI, std::atoi(row[c_psi_lvl]));
        // ---------------------------------------------
        Skill->SetSP(MS_INT, (short)std::atoi(row[c_int_pts]));
        Skill->SetSP(MS_CON, (short)std::atoi(row[c_con_pts]));
        Skill->SetSP(MS_DEX, (short)std::atoi(row[c_dex_pts]));
        Skill->SetSP(MS_STR, (short)std::atoi(row[c_str_pts]));
        Skill->SetSP(MS_PSI, (short)std::atoi(row[c_psi_pts]));
        // ---------------------------------------------
        Skill->SetXP(MS_INT, std::atof(row[c_int_xp]));
        Skill->SetXP(MS_CON, std::atof(row[c_con_xp]));
        Skill->SetXP(MS_DEX, std::atof(row[c_dex_xp]));
        Skill->SetXP(MS_STR, std::atof(row[c_str_xp]));
        Skill->SetXP(MS_PSI, std::atof(row[c_psi_xp]));
        // ---------------------------------------------
        // SubSkills
        // ---------------------------------------------
        Skill->SetSubSkill(SK_MC, std::atoi(row[c_mc]));
        Skill->SetSubSkill(SK_HC, std::atoi(row[c_hc]));
        Skill->SetSubSkill(SK_TRA, std::atoi(row[c_tra]));
        Skill->SetSubSkill(SK_FOR, std::atoi(row[c_for]));
        Skill->SetSubSkill(SK_PC, std::atoi(row[c_pc]));
        Skill->SetSubSkill(SK_RC, std::atoi(row[c_rc]));
        Skill->SetSubSkill(SK_TC, std::atoi(row[c_tc]));
        Skill->SetSubSkill(SK_VHC, std::atoi(row[c_vhc]));
        Skill->SetSubSkill(SK_AGL, std::atoi(row[c_agl]));
        Skill->SetSubSkill(SK_REP, std::atoi(row[c_rep]));
        Skill->SetSubSkill(SK_REC, std::atoi(row[c_rec]));
        Skill->SetSubSkill(SK_RCL, std::atoi(row[c_rcl]));
        Skill->SetSubSkill(SK_ATL, std::atoi(row[c_atl]));
        Skill->SetSubSkill(SK_END, std::atoi(row[c_end]));
        Skill->SetSubSkill(SK_FIR, std::atoi(row[c_fir]));
        Skill->SetSubSkill(SK_ENR, std::atoi(row[c_enr]));
        Skill->SetSubSkill(SK_XRR, std::atoi(row[c_xrr]));
        Skill->SetSubSkill(SK_POR, std::atoi(row[c_por]));
        Skill->SetSubSkill(SK_HLT, std::atoi(row[c_hlt]));
        Skill->SetSubSkill(SK_HCK, std::atoi(row[c_hck]));
        Skill->SetSubSkill(SK_BRT, std::atoi(row[c_brt]));
        Skill->SetSubSkill(SK_PSU, std::atoi(row[c_psu]));
        Skill->SetSubSkill(SK_WEP, std::atoi(row[c_wep]));
        Skill->SetSubSkill(SK_CST, std::atoi(row[c_cst]));
        Skill->SetSubSkill(SK_RES, std::atoi(row[c_res]));
        Skill->SetSubSkill(SK_IMP, std::atoi(row[c_imp]));
        Skill->SetSubSkill(SK_PPU, std::atoi(row[c_ppu]));
        Skill->SetSubSkill(SK_APU, std::atoi(row[c_apu]));
        Skill->SetSubSkill(SK_MST, std::atoi(row[c_mst]));
        Skill->SetSubSkill(SK_PPW, std::atoi(row[c_ppw]));
        Skill->SetSubSkill(SK_PSR, std::atoi(row[c_psr]));
        Skill->SetSubSkill(SK_WPW, std::atoi(row[c_wpw]));
        // ---------------------------------------------
        // Inventory
        // ---------------------------------------------
        mInventory.SQLLoad(mID);
        // + Belt, Implants(&Armor), Gogo, GR list, Chats settings & friendlist

        // temp value forcing, not get/saved from DB atm
        mSoullight = 10;
        mCombatRank = (u8)(random() % 127); // bad result there on randomness
        mSynaptic = 0;
        mIsDead = false;

        mDirectCharID = 0; // until saved/loaded with char
        mBuddyList = new PBuddyList(mID);
        if (!mBuddyList->SQLLoad())
        {
          Console->Print(RED, BLACK, "Char ID %d : Can't load buddy list", mID);
        }

        mGenrepList = new PGenrepList(mID);
        if (!mGenrepList->SQLLoad())
        {
          Console->Print(RED, BLACK, "Char ID %d : Can't load genrep list", mID);
        }

    }
    MySQL->FreeGameSQLResult(result);

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
    queryv += Ssprintf(",'%u'", mAccount);
    query += ",c_class";
    queryv += Ssprintf(",'%u'", mClass);
    query += ",c_sex";
    queryv += Ssprintf(",'%u'", mGender);
    query += ",c_profession";
    queryv += Ssprintf(",'%u'", mProfession);
    query += ",c_faction";
    queryv += Ssprintf(",'%u'", mFaction);
    query += ",c_head";
    queryv += Ssprintf(",'%u'", mRealHead);
    query += ",c_torso";
    queryv += Ssprintf(",'%u'", mRealTorso);
    query += ",c_legs";
    queryv += Ssprintf(",'%u'", mRealLegs);
    //query += ",c_model";
    //queryv += Ssprintf(",'%u'", mModel);
    //query += ",c_type";
    //queryv += Ssprintf(",'%u'", mType);
    query += ",c_location";
    queryv += Ssprintf(",'%u'", mLocation);
    query += ",c_cash";
    queryv += Ssprintf(",'%u'", mCash);

    query = query + queryv + ");";

    if ( MySQL->GameQuery(query.c_str()) )
    {
        Console->Print(RED, BLACK, "PChar::SQLCreate could not add char %s to database", mName.c_str());
        Console->Print("Query was:");
        Console->Print("%s", query.c_str());
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

bool PChar::CreateNewChar(u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
  u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot)
{
	SetName(Name);
	SetGender(Gender);
	SetProfession(Profession);
	SetFaction(Faction);
	SetRealLook(Head, Torso, Legs);
	SetBaseSkills();
	SetBaseSubskills(NZSNb, NonZeroSubskills);
	SetBaseInventory();
	SetAccount(Account);
	SetCharSlot(Slot);
	mLocation = Config->GetOptionInt("new_char_location");

	// This part will have to be rewritten with proper methods
  mSoullight = 10;
  mCombatRank = (u8)(random() % 127); // bad result there on randomness
  mSynaptic = 0;
  mIsDead = false;

  mDirectCharID = 0; // until saved/loaded with char

	SetDirtyFlag();

	if (SQLCreate()) // mID isn't defined before that
	{
    mBuddyList = new PBuddyList(mID);
    mGenrepList = new PGenrepList(mID);
    mStartApt = mPrimaryApt = Appartements->CreateBaseAppartement(mID, mName, mFaction);

	  if (mStartApt && SQLSave())
	  {
      return true;
    }
    else
    {
      if (mID)
      {
        SQLDelete();
        if (mStartApt)
        {
          Appartements->DeleteCharAppartements(mID);
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

    query += Ssprintf(" c_location='%u'", mLocation);
    query += Ssprintf(",c_pos_x='%u'", Coords.mX);
    query += Ssprintf(",c_pos_y='%u'", Coords.mY);
    query += Ssprintf(",c_pos_z='%u'", Coords.mZ);
    query += Ssprintf(",c_angle_ud='%u'", Coords.mUD);
    query += Ssprintf(",c_angle_lr='%u'", Coords.mLR);
    query += Ssprintf(",c_cash='%u'", mCash);
    query += Ssprintf(",c_apt='%u'", mPrimaryApt);

    query += Ssprintf(",c_head='%u'", mRealHead);
    query += Ssprintf(",c_torso='%u'", mRealTorso);
    query += Ssprintf(",c_legs='%u'", mRealLegs);

    query += Ssprintf(",c_faction='%u'", mFaction);

    /* This group of fiels shouldn't change in-game
    query = query + ",c_name='" + mName + "'";
    query += Ssprintf(",a_id='%u'", mAccount);
    query += Ssprintf(",c_class='%u'", mClass);
    query += Ssprintf(",c_sex='%u'", mGender);
    query += Ssprintf(",c_profession='%u'", mProfession);
    // query += Ssprintf(",c_model='%u'", mModel);
    // query += Ssprintf(",c_type='%u'", mType);
    */

        // ---------------------------------------------
        // Saving skills --- MAIN Skills with SP and XP
        // ---------------------------------------------
    query += Ssprintf(",c_int_lvl='%u'", Skill->GetMainSkill(MS_INT));
    query += Ssprintf(",c_con_lvl='%u'", Skill->GetMainSkill(MS_CON));
    query += Ssprintf(",c_dex_lvl='%u'", Skill->GetMainSkill(MS_DEX));
    query += Ssprintf(",c_str_lvl='%u'", Skill->GetMainSkill(MS_STR));
    query += Ssprintf(",c_psi_lvl='%u'", Skill->GetMainSkill(MS_PSI));
        // ---------------------------------------------
    query += Ssprintf(",c_int_pts='%u'", Skill->GetSP(MS_INT));
    query += Ssprintf(",c_con_pts='%u'", Skill->GetSP(MS_CON));
    query += Ssprintf(",c_dex_pts='%u'", Skill->GetSP(MS_DEX));
    query += Ssprintf(",c_str_pts='%u'", Skill->GetSP(MS_STR));
    query += Ssprintf(",c_psi_pts='%u'", Skill->GetSP(MS_PSI));
        // ---------------------------------------------
    query += Ssprintf(",c_int_xp='%u'", Skill->GetXP(MS_INT));
    query += Ssprintf(",c_con_xp='%u'", Skill->GetXP(MS_CON));
    query += Ssprintf(",c_dex_xp='%u'", Skill->GetXP(MS_DEX));
    query += Ssprintf(",c_str_xp='%u'", Skill->GetXP(MS_STR));
    query += Ssprintf(",c_psi_xp='%u'", Skill->GetXP(MS_PSI));
        // ---------------------------------------------
        // SubSkills
        // ---------------------------------------------
    query += Ssprintf(",c_mc='%u'", Skill->GetSubSkill(SK_MC));
    query += Ssprintf(",c_hc='%u'", Skill->GetSubSkill(SK_HC));
    query += Ssprintf(",c_tra='%u'", Skill->GetSubSkill(SK_TRA));
    query += Ssprintf(",c_for='%u'", Skill->GetSubSkill(SK_FOR));
    query += Ssprintf(",c_pc='%u'", Skill->GetSubSkill(SK_PC));
    query += Ssprintf(",c_rc='%u'", Skill->GetSubSkill(SK_RC));
    query += Ssprintf(",c_tc='%u'", Skill->GetSubSkill(SK_TC));
    query += Ssprintf(",c_vhc='%u'", Skill->GetSubSkill(SK_VHC));
    query += Ssprintf(",c_agl='%u'", Skill->GetSubSkill(SK_AGL));
    query += Ssprintf(",c_rep='%u'", Skill->GetSubSkill(SK_REP));
    query += Ssprintf(",c_rec='%u'", Skill->GetSubSkill(SK_REC));
    query += Ssprintf(",c_rcl='%u'", Skill->GetSubSkill(SK_RCL));
    query += Ssprintf(",c_atl='%u'", Skill->GetSubSkill(SK_ATL));
    query += Ssprintf(",c_end='%u'", Skill->GetSubSkill(SK_END));
    query += Ssprintf(",c_fir='%u'", Skill->GetSubSkill(SK_FIR));
    query += Ssprintf(",c_enr='%u'", Skill->GetSubSkill(SK_ENR));
    query += Ssprintf(",c_xrr='%u'", Skill->GetSubSkill(SK_XRR));
    query += Ssprintf(",c_por='%u'", Skill->GetSubSkill(SK_POR));
    query += Ssprintf(",c_htl='%u'", Skill->GetSubSkill(SK_HLT));
    query += Ssprintf(",c_hck='%u'", Skill->GetSubSkill(SK_HCK));
    query += Ssprintf(",c_brt='%u'", Skill->GetSubSkill(SK_BRT));
    query += Ssprintf(",c_psu='%u'", Skill->GetSubSkill(SK_PSU));
    query += Ssprintf(",c_wep='%u'", Skill->GetSubSkill(SK_WEP));
    query += Ssprintf(",c_cst='%u'", Skill->GetSubSkill(SK_CST));
    query += Ssprintf(",c_res='%u'", Skill->GetSubSkill(SK_RES));
    query += Ssprintf(",c_imp='%u'", Skill->GetSubSkill(SK_IMP));
    query += Ssprintf(",c_ppu='%u'", Skill->GetSubSkill(SK_PPU));
    query += Ssprintf(",c_apu='%u'", Skill->GetSubSkill(SK_APU));
    query += Ssprintf(",c_mst='%u'", Skill->GetSubSkill(SK_MST));
    query += Ssprintf(",c_ppw='%u'", Skill->GetSubSkill(SK_PPW));
    query += Ssprintf(",c_psr='%u'", Skill->GetSubSkill(SK_PSR));
    query += Ssprintf(",c_wpw='%u'", Skill->GetSubSkill(SK_WPW));
        // ---------------------------------------------

    query += Ssprintf(" WHERE c_id='%u' LIMIT 1;", mID);

    if ( MySQL->GameQuery( query.c_str()) )
    {
        Console->Print(RED, BLACK, "PChar::SQLSave could not save char %s (%u) to database", mName.c_str(), mID);
        Console->Print("Query was:");
        Console->Print("%s", query.c_str());
        MySQL->ShowGameSQLError();
        return false;
    }

    if (! mInventory.SQLSave(mID))
      return false;

    // + Belt, Implants(&Armor), Gogo, GRs,
    // Chats settings (?), directs & buddies, GR list

    mDirtyFlag = false;
    return true;
}

bool PChar::SQLDelete()
{
  return true;
}

void PChar::SetOnlineStatus(bool IsOnline)
{
    // Deactivated, until Maxx added c_isonline row to `characters`
    //char query[255];
    int onlinestatus = 0;

    if(IsOnline)
    {
        onlinestatus = 0; // Strange ????
        mIsOnline = true;
    }
    else
    {
        onlinestatus = 1; // Strange ????
        mIsOnline = false;
    }

//    sprintf(query, "UPDATE charlist SET c_isonline = %d WHERE a_id = %d AND c_id = %d", onlinestatus, mAccount, mID);
//    if(MySQL->Query(query))
//    {
//        Console->Print("Error: Cant set onlinestatus to '%d' for Account: %d, Char: %d", onlinestatus, mAccount, mID);
//        MySQL->ShowSQLError();
//        return;
//    }
    return;
}

void PChar::FillinCharDetails(u8 *Packet)
{
    //const PDefCharKind *def = GameDefs->GetCharKindDef(GetType());
    const PDefCharKind *def = GameDefs->GetCharKindDef(GetProfession());

    if(!Packet)
        return;

    Packet[36] = mHealth;
    Packet[38] = GetMaxHealth();
    Packet[40] = mMana;
    Packet[42] = GetMaxMana();
    Packet[44] = mStamina;
    Packet[46] = GetMaxStamina();

    Packet[52] = (u16)mHealth + 1;
    Packet[54] = (u16)mHealth + 1;
    Packet[56] = (u16)mHealth + 1;

// ---------------------------------------------------

    Packet[76] = (u8)Skill->GetMainSkill(MS_STR);
    Packet[77] = (u16)Skill->GetSP(MS_STR);
    Packet[79] = (u32)Skill->GetXP(MS_STR);
    Packet[83] = (u8)def->GetSkillInfo(MS_STR).mGrow;
    Packet[84] = (u8)def->GetSkillInfo(MS_STR).mMax;

    Packet[85] = (u8)Skill->GetMainSkill(MS_DEX);
    Packet[86] = (u16)Skill->GetSP(MS_DEX);
    Packet[88] = (u32)Skill->GetXP(MS_DEX);
    Packet[92] = (u8)def->GetSkillInfo(MS_DEX).mGrow;
    Packet[93] = (u8)def->GetSkillInfo(MS_DEX).mMax;

    Packet[94] = (u8)Skill->GetMainSkill(MS_CON);
    Packet[95] = (u16)Skill->GetSP(MS_CON);
    Packet[97] = (u32)Skill->GetXP(MS_CON);
    Packet[101] = (u8)def->GetSkillInfo(MS_CON).mGrow;
    Packet[102] = (u8)def->GetSkillInfo(MS_CON).mMax;

    Packet[103] = (u8)Skill->GetMainSkill(MS_INT);
    Packet[104] = (u16)Skill->GetSP(MS_INT);
    Packet[106] = (u32)Skill->GetXP(MS_INT);
    Packet[110] = (u8)def->GetSkillInfo(MS_INT).mGrow;
    Packet[111] = (u8)def->GetSkillInfo(MS_INT).mMax;

    Packet[112] = (u8)Skill->GetMainSkill(MS_PSI);
    Packet[113] = (u16)Skill->GetSP(MS_PSI);
    Packet[115] = (u32)Skill->GetXP(MS_PSI);
    Packet[119] = (u8)def->GetSkillInfo(MS_PSI).mGrow;
    Packet[120] = (u8)def->GetSkillInfo(MS_PSI).mMax;

// ---------------------------------------------------

    Packet[132] = (u8)Skill->GetSubSkill(SK_MC);
    Packet[133] = (u8)Skill->GetSKPCost(SK_MC);
    Packet[134] = (u8)Skill->GetSubSkill(SK_HC);
    Packet[135] = (u8)Skill->GetSKPCost(SK_HC);
    Packet[136] = (u8)Skill->GetSubSkill(SK_TRA);
    Packet[137] = (u8)Skill->GetSKPCost(SK_TRA);
    //Packet[138] =
    //Packet[139] =
    //Packet[140] =
    //Packet[141] =
    //Packet[142] =
    //Packet[143] =
    //Packet[144] =
    //Packet[145] =
    //Packet[146] =
    //Packet[147] =
    //Packet[148] =
    //Packet[149] =
    Packet[150] = (u8)Skill->GetSubSkill(SK_PC);
    Packet[151] = (u8)Skill->GetSKPCost(SK_PC);
    Packet[152] = (u8)Skill->GetSubSkill(SK_RC);
    Packet[153] = (u8)Skill->GetSKPCost(SK_RC);
    Packet[154] = (u8)Skill->GetSubSkill(SK_TC);
    Packet[155] = (u8)Skill->GetSKPCost(SK_TC);
    Packet[156] = (u8)Skill->GetSubSkill(SK_VHC);
    Packet[157] = (u8)Skill->GetSKPCost(SK_VHC);
    Packet[158] = (u8)Skill->GetSubSkill(SK_AGL);
    Packet[159] = (u8)Skill->GetSKPCost(SK_AGL);
    Packet[160] = (u8)Skill->GetSubSkill(SK_REP);
    Packet[161] = (u8)Skill->GetSKPCost(SK_REP);
    Packet[162] = (u8)Skill->GetSubSkill(SK_REC);
    Packet[163] = (u8)Skill->GetSKPCost(SK_REC);
    Packet[164] = (u8)Skill->GetSubSkill(SK_RCL);
    Packet[165] = (u8)Skill->GetSKPCost(SK_RCL);
    //Packet[166] =
    //Packet[167] =
    //Packet[168] =
    //Packet[169] =
    Packet[170] = (u8)Skill->GetSubSkill(SK_ATL);
    Packet[171] = (u8)Skill->GetSKPCost(SK_ATL);
    Packet[172] = (u8)Skill->GetSubSkill(SK_END);
    Packet[173] = (u8)Skill->GetSKPCost(SK_END);
    Packet[174] = (u8)Skill->GetSubSkill(SK_FOR);
    Packet[175] = (u8)Skill->GetSKPCost(SK_FOR);
    Packet[176] = (u8)Skill->GetSubSkill(SK_FIR);
    Packet[177] = (u8)Skill->GetSKPCost(SK_FIR);
    Packet[178] = (u8)Skill->GetSubSkill(SK_ENR);
    Packet[179] = (u8)Skill->GetSKPCost(SK_ENR);
    Packet[180] = (u8)Skill->GetSubSkill(SK_XRR);
    Packet[181] = (u8)Skill->GetSKPCost(SK_XRR);
    Packet[182] = (u8)Skill->GetSubSkill(SK_POR);
    Packet[183] = (u8)Skill->GetSKPCost(SK_POR);
    Packet[184] = (u8)Skill->GetSubSkill(SK_HLT);
    Packet[185] = (u8)Skill->GetSKPCost(SK_HLT);
    //Packet[186] =
    //Packet[187] =
    //Packet[188] =
    //Packet[189] =
    Packet[190] = (u8)Skill->GetSubSkill(SK_HCK);
    Packet[191] = (u8)Skill->GetSKPCost(SK_HCK);
    Packet[192] = (u8)Skill->GetSubSkill(SK_BRT);
    Packet[193] = (u8)Skill->GetSKPCost(SK_BRT);
    Packet[194] = (u8)Skill->GetSubSkill(SK_PSU);
    Packet[195] = (u8)Skill->GetSKPCost(SK_PSU);
    Packet[196] = (u8)Skill->GetSubSkill(SK_WEP);
    Packet[197] = (u8)Skill->GetSKPCost(SK_WEP);
    Packet[198] = (u8)Skill->GetSubSkill(SK_CST);
    Packet[199] = (u8)Skill->GetSKPCost(SK_CST);
    Packet[200] = (u8)Skill->GetSubSkill(SK_RES);
    Packet[201] = (u8)Skill->GetSKPCost(SK_RES);
    Packet[202] = (u8)Skill->GetSubSkill(SK_IMP);
    Packet[203] = (u8)Skill->GetSKPCost(SK_IMP);
    //Packet[204] =
    //Packet[205] =
    //Packet[206] =
    //Packet[207] =
    //Packet[208] =
    //Packet[209] =
    Packet[210] = (u8)Skill->GetSubSkill(SK_PPU);
    Packet[211] = (u8)Skill->GetSKPCost(SK_PPU);
    Packet[212] = (u8)Skill->GetSubSkill(SK_APU);
    Packet[213] = (u8)Skill->GetSKPCost(SK_APU);
    Packet[214] = (u8)Skill->GetSubSkill(SK_MST);
    Packet[215] = (u8)Skill->GetSKPCost(SK_MST);
    Packet[216] = (u8)Skill->GetSubSkill(SK_PPW);
    Packet[217] = (u8)Skill->GetSKPCost(SK_PPW);
    Packet[218] = (u8)Skill->GetSubSkill(SK_PSR);
    Packet[219] = (u8)Skill->GetSKPCost(SK_PSR);
    Packet[220] = (u8)Skill->GetSubSkill(SK_WPW);
    Packet[221] = (u8)Skill->GetSKPCost(SK_WPW);
}

u8 PChar::GetMainRank() {
   u16 total;
   total  = Skill->GetMainSkill(MS_STR) + Skill->GetMainSkill(MS_DEX);
   total += Skill->GetMainSkill(MS_CON) + Skill->GetMainSkill(MS_INT);
   total += Skill->GetMainSkill(MS_PSI);
   return ((u8)(total/5));
}

u32 PChar::SetCash(u32 nCash)
{
    //Console->Print("Trying to set cash to nCash: %d", nCash);
    if((int)nCash > Config->GetOptionInt("max_cash"))
    {
        //Console->Print("Newcash would be more than dynamic maxcash, setting to maxcash");
        mCash = (u32)Config->GetOptionInt("max_cash");
    }
    else if(nCash > MAXCASH)
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

// ===================================

PChars::PChars()
{
	mLastID = 0;
	mLastSave = std::time(NULL);

	mAutoSavePeriod = Config->GetOptionInt("auto_save_period");
	if (mAutoSavePeriod < 0)
	{
	  Console->Print("%s auto_save_period (%d) must be strict positive.", Console->ColorText(RED, BLACK, "[Error]"), mAutoSavePeriod);
	  mAutoSavePeriod = 0;
	}
	else if (mAutoSavePeriod > 3600)
	{
	  Console->Print("%s auto_save_period (%d) too high. Forced to 3600 sec.", Console->ColorText(YELLOW, BLACK, "[Warning]"), mAutoSavePeriod);
	  mAutoSavePeriod = 0;
	}

	if (mAutoSavePeriod == 0)
	{
	  Console->Print("%s Auto-save disabled.", Console->ColorText(YELLOW, BLACK, "[Info]"));
	}
	else if (mAutoSavePeriod < 60)
	{
	  Console->Print("%s auto_save_period (%d) is low and might lead to high server load.", Console->ColorText(YELLOW, BLACK, "[Warning]"), mAutoSavePeriod);
	}

}

PChars::~PChars()
{
	for(CharMap::iterator i=mChars.begin(); i!=mChars.end(); i++)
		delete i->second;
}

void PChars::SQLLoad()
{
    Console->Print("Loading player chars...");
    int nChars = 0;
    //char query[1024];
    MYSQL_RES *result;
    MYSQL_ROW row;

    result = MySQL->GameResQuery("SELECT c_id, c_name, a_id FROM characters");
    if(result == NULL)
    {
        Console->LPrint(RED, BLACK, "FAILED");
        Console->Print(" Unable to load data from MySQL DB");
        Console->LClose();
        MySQL->ShowGameSQLError();
        exit(0);
    }
    while((row = mysql_fetch_row(result)))
    {
        int AccId = 0;
        AccId = std::atoi(row[2]);
        PAccount *Account = Database->GetAccount(AccId);
        if(Account)
        {
            PChar *info = new PChar();
            int CharID = std::atoi(row[0]);
            if(info->SQLLoad(CharID))
            {
                info->SetDirtyFlag(false);
                info->SetAccount(Account->GetID());
                mLastID = max(mLastID, info->GetID());
                if(!mChars.insert(std::make_pair(info->GetID(), info)).second)
                {
                    Console->Print(YELLOW, BLACK, "Duplicate char id found: %s (%d)", info->GetName().c_str(), info->GetID());
                    delete info;
                }
                else
                {
                    if(!Account->AddChar(info->GetID()))
						Console->Print(RED, BLACK, "Could not add char '%s' to account '%s'", info->GetName().c_str(), Account->GetName().c_str());
                    ++nChars;
                }
            }
            else
            {
                Console->Print(RED, BLACK, "Invalid char entry found in database. Ignored.");
                delete info;
            }
        }
        else
        {
            Console->Print(MAGENTA, BLACK, "Ignoring chars of inexistant account %i", AccId);
        }
    }
    MySQL->FreeGameSQLResult(result);
    Console->Print("%s Loaded %i player chars", Console->ColorText(GREEN, BLACK, "[Success]"), nChars);
	  mLastSave = std::time(NULL);
}

void PChars::SQLSave()
{
  // saves all dirty-flagged chars
  	int nChars = 0;
  	for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
	  {
		    PChar *Char = i->second;
		    if(Char->IsDirty())
    		{
    			if (Char->SQLSave())
    			  ++nChars;
    		}
		}
Console->Print("%i characters saved", nChars);
    return;
}

PChar *PChars::GetChar(u32 CharID) const
{
	PChar *Result = 0;
	CharMap::const_iterator i = mChars.find(CharID);
	if(i != mChars.end())
		Result = i->second;

	return Result;
}

PChar *PChars::GetChar(const std::string &Name) const
{
	PChar *Result = 0;
	for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
    {
		if(!/*std::*/strcasecmp(i->second->GetName().c_str(), Name.c_str()))
		{
			Result = i->second;
			break;
		}
	}
	return Result;
}

void PChars::Update()
{
	std::time_t t = std::time(NULL); // changed to time() to have real time instead of cpu used time

	if(mAutoSavePeriod && ((t-mLastSave) >= mAutoSavePeriod))
	{
		bool NeedSave = false;
		for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
		{
			if(i->second->IsDirty())
			{
				NeedSave=true;
				break;
			}
		}

		if(NeedSave)
		{
if (gDevDebug) Console->Print("Some characters need autosaving...");
			SQLSave();
if (gDevDebug) Console->Print("Autosave done.");
		}
		mLastSave = t;
	}
}

PChar *PChars::CreateChar(u32 Account, const std::string &Name, u32 Gender, u32 Profession, u32 Faction,
  u32 Head, u32 Torso, u32 Legs, u8 NZSNb, const char *NonZeroSubskills, u32 Slot)
{
	PChar* nChar = new PChar();
	if (nChar->CreateNewChar(Account, Name, Gender, Profession, Faction, Head, Torso, Legs, NZSNb, NonZeroSubskills, Slot))
  {
    mChars.insert(std::make_pair(nChar->GetID(), nChar));
    if (mLastID < nChar->GetID())
      mLastID = nChar->GetID(); // just in case it is needed somewhere. To be removed later with all LastID.
    return nChar;
  }
  else
  {
    delete nChar;
    return NULL;
  }
}
