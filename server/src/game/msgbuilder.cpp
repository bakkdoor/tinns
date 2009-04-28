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

 msgbuilder.h - a classes to build NC messages

 CREATION: 30 Aug 2006 Hammag

 MODIFIED:
 REASON: -

*/

#include "main.h"
#include "msgbuilder.h"

#include "worlds.h"
#include "appartements.h"
#include "vehicle.h"
#include "subway.h"
#include "item.h"
#include "container.h"


PMessage* PMsgBuilder::BuildCharHelloMsg( PClient* nClient )
{
  PChar *nChar = nClient->GetChar();
  u32 nSkin, nHead, nTorso, nLegs;
  u8 nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness;

  nChar->GetCurrentLook( nSkin, nHead, nTorso, nLegs );
  nChar->GetCurrentBodyColor( nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness );

  PMessage* tmpMsg = new PMessage( 80 );

  u8 currentActiveSlot = nChar->GetQuickBeltActiveSlot();
  u16 ItemVal1 = 0;
  switch ( currentActiveSlot )
  {
    case INV_WORN_QB_NONE:
      //ItemInHandID = 0;
      break;
    case INV_WORN_QB_HAND:
      //ItemInHandID = 0; // <= TODO
      break;
    default:
      PItem* currentItem = nChar->GetInventory()->GetContainer( INV_LOC_WORN )->GetItem( INV_WORN_QB_START + currentActiveSlot );
      //Todo : item addons & effects
      //ItemInHandID = currentItem->GetItemID():
      if ( currentItem )
        ItemVal1 = currentItem->GetValue1();
      //else
      //ItemInHandID = 0;
      break;
  }

  //nClient->IncreaseUDP_ID(); // This must be done outside

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x00; // size placeholder, set later in the function
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u32 )nChar->GetID();

  *tmpMsg << ( u8 )0x60; // 0x40 if current faction epic done (master), | 0x80 to display [afk] | 0x20 if LE in
  *tmpMsg << ( u8 )(( nChar->GetSpeedOverride() == 255 ) ? 10 : nChar->GetSpeedOverride() ); // move speed, reset by client (and for him only) when getting fall damage
  *tmpMsg << ( u8 )0x08; // ??? something to do with speed ?
  *tmpMsg << ( u16 )ItemVal1; // WeaponID of the weapon in hand
  *tmpMsg << ( u8 )0x01; // ???
  *tmpMsg << ( u8 )0x01; // ???
  *tmpMsg << ( u8 )( 128 + nChar->GetSoullight() );
  *tmpMsg << ( u8 )nChar->GetMainRank(); // in fact, Ranks are of type s8, but it doesn't matter much
  *tmpMsg << ( u8 )nChar->GetCombatRank();

  *tmpMsg << ( u8 )nChar->GetFaction();

  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u8 )0x0f; // size of the next bloc (skin + ?clan?)
  // Current skin
  *tmpMsg << ( u16 )nSkin;
  *tmpMsg << ( u8 )nHead;
  *tmpMsg << ( u8 )nTorso;
  *tmpMsg << ( u8 )nLegs;
  // Skin colors
  *tmpMsg << ( u8 )nHeadColor;
  *tmpMsg << ( u8 )nTorsoColor;
  *tmpMsg << ( u8 )nLegsColor;
  *tmpMsg << ( u8 )nHeadDarkness; // (0=bright 255=dark)
  *tmpMsg << ( u8 )nTorsoDarkness;
  *tmpMsg << ( u8 )nLegsDarkness;
  *tmpMsg << ( u8 )0x00; // ??? << not sure at all // eg: 0x3e
  *tmpMsg << ( u8 )0x00; // eg: 0x03
  *tmpMsg << ( u8 )0x00; // eg: 0xa3
  *tmpMsg << ( u8 )0x00; // eg: 0x03

  //Name
  *tmpMsg << ( u8 )(( nChar->GetName() ).length() + 1 );
  *tmpMsg << ( nChar->GetName() ).c_str();
  //Body effects
  u8 cBodyEffect, cEffectDensity;
  nChar->GetBodyEffect( cBodyEffect, cEffectDensity );
  if ( cBodyEffect )
  {
    *tmpMsg << ( u8 )0x06; // size of effect list : 6 bytes/effect. Only one supported atm
    *tmpMsg << ( u8 )cBodyEffect; // effect type (0=none, effecive values 1 - 17)
    *tmpMsg << ( u8 )cEffectDensity; // density: 0=max, 0xff=min (for some effects only)
    *tmpMsg << ( u8 )0x00; // ???
    *tmpMsg << ( u8 )0x00; // ???
    *tmpMsg << ( u8 )0x00; // ???
    *tmpMsg << ( u8 )0x00; // ???
  }
  else
  {
    *tmpMsg << ( u8 )0x00; // size of empty effect list
  }

  *tmpMsg << ( u8 )0x00; // ending null
  // alternate interpretation to this "ending null"/optional bloc:
  /* *tmpMsg << (u8)0x04; // size of unknown bloc ... 0x00 when empty (aka the "ending null")
   *tmpMsg << (u8)0x0b; // vary ... ??? 0b, eb, ee, ...
   *tmpMsg << (u8)0x44; // vary ... ???
   *tmpMsg << (u8)0x00; // these two seem always null
   *tmpMsg << (u8)0x00; */

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}


PMessage* PMsgBuilder::BuildReqInfoAnswerMsg( PClient* nClient, u16 nReqType, u32 nInfoId, void* nResponse, u16 nResponseLength )
{
  PMessage* tmpMsg;

  tmpMsg = new PMessage( 18 + nResponseLength );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << ( u16 )0x0006; // cmd
  *tmpMsg << ( u16 )nReqType; // wrong size here (u32) for buffer size u16 in NeoX
  *tmpMsg << ( u32 )nInfoId;
  tmpMsg->Write( nResponse, nResponseLength );

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharHealthUpdateMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 14 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x30;
  *tmpMsg << ( u8 )0x64; //Head Heath =Head HP/(3 *0.45)(with max Head HP = 45% of total)
  *tmpMsg << ( u8 )0x64; //Body Heath =Body HP/(3 *0.35)(for max 35% of total)
  *tmpMsg << ( u8 )0x64; //Feet Heath =Feet HP/(3 *0.20)(for max 20% of total)
  *tmpMsg << ( u8 )0x01; // Sta/Mana ?

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharPosUpdateMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 32 );
  PChar* nChar = nClient->GetChar();

  u32 cSeatObjectId;
  u8 cSeatId;
  PSeatType cSeatType = nChar->GetSeatInUse( &cSeatObjectId, &cSeatId );
  if ( cSeatType == seat_chair )   // temp ! Must migrate to RAW
  {
    cSeatObjectId = ( cSeatObjectId + 1 ) * 1024;
  }

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x1b;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u16 )0x0000; // pad to keep LocalID on u16
  *tmpMsg << ( u8 )0x03;
  if ( cSeatType )
  {
    *tmpMsg << ( u32 )cSeatObjectId;
    *tmpMsg << ( u16 )cSeatId; // testing... else 0x0000
  }
  else
  {
    *tmpMsg << ( u16 )(( nChar->Coords ).mY );
    *tmpMsg << ( u16 )(( nChar->Coords ).mZ );
    *tmpMsg << ( u16 )(( nChar->Coords ).mX );
  }
  *tmpMsg << ( u16 )( 31910 + ( nChar->Coords ).mUD - 50 );  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
  *tmpMsg << ( u16 )( 31820 + ( nChar->Coords ).mLR*2 - 179 ); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
  if ( cSeatType )
  {
    *tmpMsg << ( u8 )0x00;
    *tmpMsg << ( u8 )0x10;
  }
  else
  {
    *tmpMsg << ( u8 )(( nChar->Coords ).mAct );
    *tmpMsg << ( u8 )0x00;
  }

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharPosUpdate2Msg( PClient* nClient, u8 InfoBitfield )
{
  PMessage* tmpMsg = new PMessage( 32 );
  PChar* nChar = nClient->GetChar();

  if ( InfoBitfield == 0x80 )
  {
    Console->Print( RED, BLACK, "[ERROR] PMsgBuilder::BuildCharPosUpdate2Msg : using InfoBitfield=0x80 forbidden. Using 0x7f instead." );
    InfoBitfield = 0x7f;
  }

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x20;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )InfoBitfield;

  if ( InfoBitfield & 0x01 )
  {
    *tmpMsg << ( u16 )(( nChar->Coords ).mY );
  }
  if ( InfoBitfield & 0x02 )
  {
    *tmpMsg << ( u16 )(( nChar->Coords ).mZ );
  }
  if ( InfoBitfield & 0x04 )
  {
    *tmpMsg << ( u16 )(( nChar->Coords ).mX );
  }
  if ( InfoBitfield & 0x08 )
  {
    *tmpMsg << ( u8 )(( nChar->Coords ).mUD );
  }
  if ( InfoBitfield & 0x10 )
  {
    *tmpMsg << ( u8 )(( nChar->Coords ).mLR );
  }
  if ( InfoBitfield & 0x20 )
  {
    *tmpMsg << ( u8 )(( nChar->Coords ).mAct );
  }
  /*if(InfoBitfield & 0x40) // Not used (?)
  {
    *tmpMsg << (u8)((nChar->Coords).mUnknown);
  }*/
  /*if(InfoBitfield & 0x80) // mRoll ????
  {
    *tmpMsg << (u8)((nChar->Coords).mUnknown);
  }*/

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

/*
PMessage* PMsgBuilder::BuildCharSittingMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 24 );
  PChar* nChar = nClient->GetChar();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x32;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x00; // Type = chair/subway ???
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u8 )0x03; // Type = chair ???
  *tmpMsg << ( u16 )(( nChar->Coords ).mY );
  *tmpMsg << ( u16 )(( nChar->Coords ).mZ );
  *tmpMsg << ( u16 )(( nChar->Coords ).mX );
  *tmpMsg << ( u16 )( 31910 + ( nChar->Coords ).mUD - 50 );  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
  *tmpMsg << ( u16 )( 31820 + ( nChar->Coords ).mLR*2 - 179 ); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
  *tmpMsg << ( u8 )(( nChar->Coords ).mAct );
  *tmpMsg << ( u8 )0x00;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}
*/

PMessage* PMsgBuilder::BuildCharUseSeatMsg( PClient* nClient, u32 nRawObjectId, u8 nSeatId )
{
  PMessage* tmpMsg = new PMessage( 18 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // nClient->GetUDP_ID() placeholder
  *tmpMsg << ( u16 )0x0000; // nClient->GetSessionID()placeholder

  *tmpMsg << ( u8 )0x0c; // Sub message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // ++ nClient->GetUDP_ID() placeholder
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x21;
  *tmpMsg << ( u32 )nRawObjectId;
  *tmpMsg << ( u8 )nSeatId; // 0x00 for real chair, 1+ for subway cab

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharExitSeatMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 22 );
  PChar* nChar = nClient->GetChar();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // nClient->GetUDP_ID() placeholder
  *tmpMsg << ( u16 )0x0000; // nClient->GetSessionID()placeholder
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // nClient->GetUDP_ID() placeholder
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x22;
  *tmpMsg << ( u16 )(( nChar->Coords ).mY + 768 ) ;
  *tmpMsg << ( u16 )(( nChar->Coords ).mZ + 768 ) ;
  *tmpMsg << ( u16 )(( nChar->Coords ).mX + 768 ) ;
  *tmpMsg << ( u8 )( nChar->Coords ).mUD;
  *tmpMsg << ( u8 )( nChar->Coords ).mLR;
  *tmpMsg << ( u8 )( nChar->Coords ).mAct;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildPacket0Msg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 70 );
  PChar* nChar = nClient->GetChar();
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x2c;
  *tmpMsg << ( u8 )0x01; // ??
  *tmpMsg << ( u8 )0x01; // ??
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( f32 )(( nChar->Coords ).mY - 32000 );
  *tmpMsg << ( f32 )(( nChar->Coords ).mZ - 32000 );
  *tmpMsg << ( f32 )(( nChar->Coords ).mX - 32000 );
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u8 )0x01; // ????
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u8 )0x07;
  *tmpMsg << ( u8 )0x02;
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;


  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
  return tmpMsg;
}

PMessage* PMsgBuilder::BuildPingMsg( PClient* nClient, u32 nClientTime )
{
  PMessage* tmpMsg = new PMessage( 15 );
  u32 LocalTime = GameServer->GetGameTime();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x0b;
  *tmpMsg << ( u32 )LocalTime;
  *tmpMsg << ( u32 )nClientTime;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildBaselineMsg( PClient* nClient )
{
  PMessage* BaselineMsg = new PMessage( 512 );
  PMessage SectionMsg( 256 );

  PChar *nChar = nClient->GetChar();
  const PDefCharKind *def = GameDefs->CharKinds()->GetDef( nChar->GetProfession() );
  PSkillHandler *Skill = nChar->Skill;

  nClient->IncreaseTransactionID( 3 );

  // Head
  *BaselineMsg << ( u8 )0x22;
  *BaselineMsg << ( u8 )0x02; // ?
  *BaselineMsg << ( u8 )0x01; // ?

  // ---- Section 1 ----
  *BaselineMsg << ( u8 )0x01; // section id

  SectionMsg << ( u8 )0xfa; // ?  // // section content at offset 3
  SectionMsg << ( u8 )nChar->GetProfession();
  SectionMsg << ( u16 )nClient->GetTransactionID(); // Transaction ID ? 0x8aa0
  SectionMsg << ( u32 )nChar->GetID();
  SectionMsg << ( u8 )0x0e; // ? 0x0e in NeoX, 0x10 in Tinns ... doesn't seem to matter
  SectionMsg << ( u8 )0x00; // ?

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 2 ----
  *BaselineMsg << ( u8 )0x02; // section id
  SectionMsg << ( u8 )0x04; // ?  // section content at offset 3
  SectionMsg << ( u8 )0x04; // ?
  SectionMsg << ( u16 )500; // nChar->GetHealth();
  SectionMsg << ( u16 )500; // nChar->GetMaxHealth();
  SectionMsg << ( u16 )500; // nChar->GetMana();
  SectionMsg << ( u16 )500; // nChar->GetMaxMana();
  SectionMsg << ( u16 )500; // nChar->GetStamina();
  SectionMsg << ( u16 )500; // nChar->GetMaxStamina();
  SectionMsg << ( u16 )0x00ff; // ?
  SectionMsg << ( u16 )0x00ff; // ?
  SectionMsg << ( u16 )0x00e1; // (nChar->GetHealth() + 1); // ? Probably Head Health (45% of total) 0x0065
  SectionMsg << ( u16 )0x0147; // (nChar->GetHealth() + 1); // ? Torso Health (35% of total)
  SectionMsg << ( u16 )0x0147; // (nChar->GetHealth() + 1); // ? Legs Health (20% of total)
  SectionMsg << ( u8 )100; // 100 - SI
  SectionMsg << ( u8 )0x80;
  SectionMsg << ( u16 )0x0000;

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 3 ----
  *BaselineMsg << ( u8 )0x03; // section id

  SectionMsg << ( u8 )0x06; // ?  // section content at offset 3
  SectionMsg << ( u8 )0x09; // ?
  SectionMsg << ( u32 )0x00000000; // ?
  SectionMsg << ( u32 )0x00000000; // ?
  SectionMsg << ( u8 )0x01; // ?

  SectionMsg << ( u8 )Skill->GetMainSkill( MS_STR );
  SectionMsg << ( u16 )Skill->GetSP( MS_STR );
  SectionMsg << ( u32 )Skill->GetXP( MS_STR );
  SectionMsg << ( u8 )def->GetSkillInfo( MS_STR ).mGrow;
  SectionMsg << ( u8 )def->GetSkillInfo( MS_STR ).mMax;

  SectionMsg << ( u8 )Skill->GetMainSkill( MS_DEX );
  SectionMsg << ( u16 )Skill->GetSP( MS_DEX );
  SectionMsg << ( u32 )Skill->GetXP( MS_DEX );
  SectionMsg << ( u8 )def->GetSkillInfo( MS_DEX ).mGrow;
  SectionMsg << ( u8 )def->GetSkillInfo( MS_DEX ).mMax;

  SectionMsg << ( u8 )Skill->GetMainSkill( MS_CON );
  SectionMsg << ( u16 )Skill->GetSP( MS_CON );
  SectionMsg << ( u32 )Skill->GetXP( MS_CON );
  SectionMsg << ( u8 )def->GetSkillInfo( MS_CON ).mGrow;
  SectionMsg << ( u8 )def->GetSkillInfo( MS_CON ).mMax;

  SectionMsg << ( u8 )Skill->GetMainSkill( MS_INT );
  SectionMsg << ( u16 )Skill->GetSP( MS_INT );
  SectionMsg << ( u32 )Skill->GetXP( MS_INT );
  SectionMsg << ( u8 )def->GetSkillInfo( MS_INT ).mGrow;
  SectionMsg << ( u8 )def->GetSkillInfo( MS_INT ).mMax;

  SectionMsg << ( u8 )Skill->GetMainSkill( MS_PSI );
  SectionMsg << ( u16 )Skill->GetSP( MS_PSI );
  SectionMsg << ( u32 )Skill->GetXP( MS_PSI );
  SectionMsg << ( u8 )def->GetSkillInfo( MS_PSI ).mGrow;
  SectionMsg << ( u8 )def->GetSkillInfo( MS_PSI ).mMax;

  SectionMsg << ( u16 )0x0000; // ?
  SectionMsg << ( u8 )0x00; // ?
  SectionMsg << ( u8 )0x08; // ?

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 4 ----
  *BaselineMsg << ( u8 )0x04; // section id

  SectionMsg << ( u8 )0x2e; // ?  // section content at offset 3
  SectionMsg << ( u8 )0x02; // ?
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_MC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_MC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_HC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_HC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_TRA );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_TRA );
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_PC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_PC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_RC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_RC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_TC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_TC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_VHC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_VHC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_AGL );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_AGL );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_REP );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_REP );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_REC );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_REC );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_RCL );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_RCL );
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_ATL );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_ATL );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_END );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_END );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_FOR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_FOR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_FIR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_FIR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_ENR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_ENR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_XRR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_XRR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_POR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_POR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_HLT );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_HLT );
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_HCK );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_HCK );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_BRT );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_BRT );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_PSU );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_PSU );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_WEP );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_WEP );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_CST );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_CST );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_RES );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_RES );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_IMP );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_IMP );
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )0x00; // spare
  SectionMsg << ( u8 )0x01; // cost
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_PPU );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_PPU );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_APU );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_APU );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_MST );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_MST );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_PPW );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_PPW );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_PSR );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_PSR );
  SectionMsg << ( u8 )Skill->GetSubSkill( SK_WPW );
  SectionMsg << ( u8 )Skill->GetSKPCost( SK_WPW );

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 5 ----
  *BaselineMsg << ( u8 )0x05; // section id

  PMessage* ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_BACKPACK ), INV_LOC_BACKPACK );
  SectionMsg << *ContentList;
  delete ContentList;

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 6 ----
  *BaselineMsg << ( u8 )0x06; // section id

  ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_WORN ), INV_LOC_WORN );
  SectionMsg << *ContentList;
  delete ContentList;

  /*    SectionMsg << (u8)0x04; // QB/Armor/Implants items nb  // section content at offset 3

      // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== BEGIN
      SectionMsg << (u16)0x06;     // Size of item
      SectionMsg << (u16)0x00;     // Location: Quickbelt slot 0
      SectionMsg << (u16)0x0051;   // ItemID: 81, Flashlight
      SectionMsg << (u8)0x01;      // Datatype
      SectionMsg << (u8)0x00;      // Data
  */

  /****
  SectionMsg << (u8)0x04; // QB/Armor/Implants items nb  // section content at offset 3

  // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== BEGIN
  SectionMsg << (u16)0x001b;     // Size of item
  SectionMsg << (u8)0x00;     // Location: Quickbelt slot 0
  SectionMsg << (u8)0x00; // nop (Y)
  SectionMsg << (u16)0x0003;   // ItemID: 3, assault riffle
  SectionMsg << (u8)(0x01|0x02|0x10|0x40);      // Datatype

  //SectionMsg << (u8)0x01; // for 0x80. Use ???

  SectionMsg << (u8)0x00; // Qty / remaining ammos

  SectionMsg << (u8)0x06; // Qual entries
  SectionMsg << (u8)0x40; // current qual
  SectionMsg << (u8)0x80; // dmg
  SectionMsg << (u8)0xc0; // freq
  SectionMsg << (u8)0xa0; // handl
  SectionMsg << (u8)0xb0; // range
  SectionMsg << (u8)0xff; // max qual <= always last

  SectionMsg << (u8)0x07; // addons bitflag: flashlight=1, scope, silencer, laserpointer

  SectionMsg << (u8)0x02; // used slots
  SectionMsg << (u8)0x05; // max slots
  SectionMsg << (u16)1526; // slots / explo ammo
  SectionMsg << (u16)21; // riffle-barrel
  SectionMsg << (u16)0x0000;
  SectionMsg << (u16)0x0000;
  SectionMsg << (u16)0x0000;
  *****/
  /*
  SectionMsg << (u16)0x06;     // Size of item
  SectionMsg << (u8)0x01;     // Location: Quickbelt slot 1
  SectionMsg << (u8)0x00; // nop (Y)
  SectionMsg << (u16)0x0023;   // ItemID: 35, Med Kit
  SectionMsg << (u8)0x01;      // Data=ammo count
  SectionMsg << (u8)0x03;      // Data
  */
  /*
      SectionMsg << (u16)0x06;     // Size of item
      SectionMsg << (u16)0x01;     // Location: Quickbelt slot 0
      SectionMsg << (u16)0x0055;   // ItemID: 81, Flashlight
      SectionMsg << (u8)0x01;      // Datatype
      SectionMsg << (u8)0x00;      // Data

      SectionMsg << (u16)0x06;     // Size of item
      SectionMsg << (u16)0x02;     // Location: Quickbelt slot 0
      SectionMsg << (u16)0x176F;   // ItemID: 81, Flashlight
      SectionMsg << (u8)0x01;      // Datatype
      SectionMsg << (u8)0x00;      // Data


      SectionMsg << (u16)0x08;      // Size of item
      SectionMsg << (u16)0x1a;      // Location: Brain #1
      SectionMsg << (u16)0x08fc;    // ItemID: Law enforcer
      SectionMsg << (u8)0x02;       // Datatype. 02: Item Duration information follows
      SectionMsg << (u8)0x02;       // SubDatatype02: Full itemdetails follow
      SectionMsg << (u8)0x2a;       // Current duration
      SectionMsg << (u8)0x2a;       // Max duration
  */
//    nChar->GetInventory()->QB_SetSlot(0, 81); // Add Flashlight to QB slot 1
//    nChar->GetInventory()->QB_SetSlot(1, 85); // Add Flashlight to QB slot 1
//    nChar->GetInventory()->QB_SetSlot(2, 5999); // Add Flashlight to QB slot 1
  // THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== END
  /*
   StatsBuffer[len+3] = 0; //Number of items
   plen = 4;
   for (i=0;i<MAX_INVENTORY;i++)
   {
    for (t=0;t<128;t++)
    {
     if (CurrentChar.QuickBelt[t]-1 != i)
      continue;
     *(unsigned short*)&StatsBuffer[len+plen] = 6;    //Data size of item
     *(unsigned short*)&StatsBuffer[len+plen+2] = t;  //X position in Inventory
     *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].ItemID; //Item ID
     *(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].Qty;   //Quantity
     plen += 8;
     *(unsigned short*)&StatsBuffer[len+3] += 1;      //Add to item
     break;
    }
  */
  /*if (CurrentChar.Inventory[i].ItemID == 0)     //Last Item
   break;
  if (CurrentChar.Inventory[i].Location != 2)     //Not Quickbelt, skip it
   continue;
  if (Def_GetItemType(CurrentChar.Inventory[i].ItemID) == 1)
  {
   //18 00 01 00 08 00 63 00 06 ff c8 c8 c8 c8 ff 00 01 00 28 04 00 01 04 ff 01
   //12 00 11    5f 07 23 00 06 eb a4 99 a3 a5 ff 04 00 01 04 ff 01
   //13 00 05 00 03 02 23 00 06 6d c4 c4 c4 c4 ff 04 00 01 04 ff 00
   //17 00 05    9d 01 73 28 06 13 c1 c1 c1 c1 ff 00 01 01 f1 05 04
   //|Size|Slot|ItemID|???  |? |Du|    Stats  |MD|?????????????????|
   *(unsigned short*)&StatsBuffer[len+plen] = 19; //Size
   *(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX; //Slot
   *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID;//Item ID
   StatsBuffer[len+plen+6] = 0x23;            //0x73 = Ranged, 0x63 = close/spell
   StatsBuffer[len+plen+7] = CurrentChar.Inventory[i].Qty;      //Quantity
   StatsBuffer[len+plen+8] = 0x06;            //0x06
   StatsBuffer[len+plen+9] = CurrentChar.Inventory[i].CurDur;     //Current Durability
   StatsBuffer[len+plen+10] = CurrentChar.Inventory[i].Damage;     //Stats
   StatsBuffer[len+plen+11] = CurrentChar.Inventory[i].Freq;     //Stats
   StatsBuffer[len+plen+12] = CurrentChar.Inventory[i].Hand;     //Stats
   StatsBuffer[len+plen+13] = CurrentChar.Inventory[i].Rng;     //Stats
   StatsBuffer[len+plen+14] = CurrentChar.Inventory[i].MaxDur;     //Max Durability
   StatsBuffer[len+plen+15] = 0x04;           //Size or part
   StatsBuffer[len+plen+16] = 0x00;           //follow or ?
   StatsBuffer[len+plen+17] = 0x01;           //Ammo loaded?
   StatsBuffer[len+plen+18] = 0x04;           //0x04 if not named, 0x0a if named
   StatsBuffer[len+plen+19] = 0x05;           //0x06 if named, 0xff for no ammo needed, no idea otherwise
   StatsBuffer[len+plen+20] = 0x0f;           //Binary representation of ammo allowed 0x01 = normal
   plen+=21;
  }
  else
  {
   *(unsigned short*)&StatsBuffer[len+plen] = 6;         //Size of Item Data
   *(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX;  //Slot
   *(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID; //Item ID
   *(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.Inventory[i].Qty;  //Quantity
   plen+= 8;
  }
  StatsBuffer[len+3] += 1;*/

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 7 ----
  *BaselineMsg << ( u8 )0x07; // section id

  SectionMsg << ( u8 )0x00; // ?? // section content at offset 3

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0c ----
  *BaselineMsg << ( u8 )0x0c; // section id

  ContentList = BuildContainerContentList( nChar->GetInventory()->GetContainer( INV_LOC_GOGO ), INV_LOC_GOGO );
  SectionMsg << *ContentList;

  delete ContentList;

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 8 ----
  *BaselineMsg << ( u8 )0x08; // section id

  u32 nSkin, nHead, nTorso, nLegs;
  nChar->GetRealLook( nSkin, nHead, nTorso, nLegs );

  SectionMsg << ( u8 )0x0a; // ? // section content at offset 3
  SectionMsg << ( u32 )nChar->GetCash();

  // Genrep list
  SectionMsg << ( u16 )nChar->GetGenrepCount(); //Number of GR tagged (Tinns value on u8 only)
  SectionMsg << ( u8 )0x04;
  if ( nChar->GetGenrepCount() ) // For each entry : (u16)genrep.g_worldid, (u16)genrep.g_stationid
  {
    SectionMsg.Write( nChar->GetGenrepListData(), nChar->GetGenrepListDataSize() );
  }

  SectionMsg << ( u8 )0x04;
  SectionMsg << ( u8 )0x04;
  SectionMsg << ( u8 )0x00;

  // Direct chat
  if ( nChar->GetDirectChat() )
  {
    SectionMsg << ( u8 )1;
    SectionMsg << ( u32 )nChar->GetDirectChat();
  }
  else
  {
    SectionMsg << ( u8 )0;
  }

  // Buddy Chat
  SectionMsg << ( u8 )nChar->GetBuddyCount(); //Number of Buddies
  if ( nChar->GetBuddyCount() ) // For each buddy (u32)buddy CharID
  {
    SectionMsg.Write( nChar->GetBuddyListData(), nChar->GetBuddyListDataSize() );
  }
  SectionMsg << ( u8 )0x00;

  SectionMsg << ( u16 )nClient->GetTransactionID(); // ??
  //SectionMsg << (u32)0x00000000;
  SectionMsg << ( u32 )0x00000000;
  SectionMsg << ( u32 )0x00000000; // Epic status ?
  SectionMsg << ( u16 )nSkin;
  SectionMsg << ( u8 )nHead;
  SectionMsg << ( u8 )nTorso;
  SectionMsg << ( u8 )nLegs;
  SectionMsg << ( u8 )0x00; // Rank
  SectionMsg << ( u32 )( nChar->GetBaseApartment() + PWorlds::mAptBaseWorldId ); // 0x22, 0x00, 0x00, 0x00, //Primary Apartment (GR activated) ???
  SectionMsg << ( u8 )0x01; // ?
  SectionMsg << ( u8 )0x00; // ?
  SectionMsg << ( u8 )0x00; // ?
  SectionMsg << ( u8 )0x00; // ?
  SectionMsg << ( u8 )0x00; // ?

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 9 ----
  *BaselineMsg << ( u8 )0x09; // section id

  SectionMsg << ( u16 )0x15; // Nb of factions // section content at offset 3
  SectionMsg << ( u16 )nChar->GetFaction();
  SectionMsg << ( u8 )0x04; // ?
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // 96.0f*9000.0f; // SL
  // Faction Sympathies Points effective sympathie is Square root of these points, abs value rounded down (beware of the sign ! :p)
  SectionMsg << ( f32 ) 1000.0f*1250.0f; // City Admin => 111
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Diamond => 100
  SectionMsg << ( f32 ) 1000.0f* -750.0f; // Next => -86
  SectionMsg << ( f32 ) 1000.0f*500.0f; // Tangent => 70
  SectionMsg << ( f32 ) 1000.0f* -250.0f; // Biotech => -50
  SectionMsg << ( f32 ) 1000.0f*0.0f; // ProtoPharm => 0
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Trader's Union
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Tsunami
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Black Dragons
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // City Mercs
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Crahn Sect
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Dome Of York
  SectionMsg << ( f32 ) 1000.0f* -1000.0f; // Anarchy Breed
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Fallen Angels
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Twilight Guardian
  SectionMsg << ( f32 ) 1000.0f* -1000.0f; // Regeant's Legacy
  SectionMsg << ( f32 ) 1000.0f* -1000.0f; // Regeant's Mutants
  SectionMsg << ( f32 ) 1000.0f* -1000.0f; // Insects
  SectionMsg << ( f32 ) 1000.0f* -1000.0f; // Monsters
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Unknown
  SectionMsg << ( f32 ) 1000.0f*1000.0f; // Highest SL?

  SectionMsg << ( u32 )0x00000000; // Epic done Data : bit 2^FactionID set <=> Epic done
  SectionMsg << ( u8 )nChar->GetFaction(); // Faction ??? wrong size ...

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0a ----
  *BaselineMsg << ( u8 )0x0a; // section id

  // Clan data ?
  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0b ----
  *BaselineMsg << ( u8 )0x0b; // section id

  SectionMsg << ( u8 )0x00; // ?? // section content at offset 3

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0d ----
  *BaselineMsg << ( u8 )0x0d; // section id

  SectionMsg << ( u8 )0xfa; // ?? // section content at offset 3
  SectionMsg << ( u8 )nChar->GetProfession();
  SectionMsg << ( u16 )nClient->GetTransactionID(); // ?? TransactionID ? 0x8aa0
  SectionMsg << ( u32 )nChar->GetID();

  *BaselineMsg << ( u16 )SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  return BaselineMsg;
}

PMessage* PMsgBuilder::BuildAliveRepMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 7 );

  // u8 up[] = {0x04, 0x01, 0x00, 0xe3, 0x6b, 0xe6, 0xee};
  *tmpMsg << ( u8 )0x04;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0xe3; // ? not always...
  *tmpMsg << ( u8 )0x6b; // ? not always...
  *tmpMsg << ( u16 )( nClient->getUDPConn()->getPort() ); // really ?

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning1Msg( PClient* nClient, u16 nEntity, u8 nUnknown )
{
  PMessage* tmpMsg = new PMessage( 42 );

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID(); // from NeoX

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0c; // Message length place;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID(); // from NeoX
  *tmpMsg << ( u8 )0x25; // ??
  *tmpMsg << ( u8 )0x13; // ??
  *tmpMsg << ( u16 )nClient->GetTransactionID(); // from NeoX / ?? right ???
  // In case of apt GR or NC2.2 ?
  // *tmpMsg << (u8)0x04; // len ?
  // *tmpMsg << (u32)AptWorldID; // len ?
  // nClient->IncreaseTransactionID();
  // *tmpMsg << (u16)nClient->GetTransactionID();
  *tmpMsg << ( u8 )0x0e; // cmd => but not for zoning, because used in non-zoning situation
  *tmpMsg << ( u8 )0x02; // ?? from NeoX
  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x17; // Message length place;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << ( u16 )0x0004; // cmd
  *tmpMsg << ( u32 )0x00000000; // from NeoX
  *tmpMsg << ( u32 )0x00000000; // from NeoX
  *tmpMsg << ( u8 )nUnknown;
  *tmpMsg << ( u16 )nEntity;
  *tmpMsg << ( u16 )0x0000; // from NeoX
  *tmpMsg << ( u16 )nClient->GetTransactionID(); // from NeoX
  *tmpMsg << ( u16 )0x0000; // from NeoX

  // We DO need this! This is a multiframe packet, and the first UDP/SessionID set *HAS* to be
  // as high as the last UDP/SessionID set in the frame!
  tmpMsg->U16Data( 1 ) = nClient->GetUDP_ID();
  tmpMsg->U16Data( 3 ) = nClient->GetSessionID();

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoningTCPReadyMsg()
{
  PMessage* tmpMsg = new PMessage( 7 );

  //static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
  *tmpMsg << ( u8 )0xfe;
  *tmpMsg << ( u16 )0x0004; //length
  *tmpMsg << ( u8 )0x83; //cmd
  *tmpMsg << ( u8 )0x0d; // sub-cmd
  *tmpMsg << ( u16 )0x0000;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildSendZoneTCPMsg( u32 nLocation, std::string* nWorldName )
{
  PMessage* tmpMsg = new PMessage( 14 + nWorldName->size() );

  *tmpMsg << ( u8 )0xfe;
  *tmpMsg << ( u16 )0x0000; // size placeholder
  *tmpMsg << ( u8 )0x83;
  *tmpMsg << ( u8 )0x0c;
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u32 )0x00000000;
  tmpMsg->Write( nWorldName->c_str(), nWorldName->size() + 1 );

  tmpMsg->U16Data( 1 ) = ( u16 )( tmpMsg->GetSize() - 3 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning2Msg( PClient* nClient, u32 nClientTime )
{
  PMessage* tmpMsg = new PMessage( 22 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x0d;
  *tmpMsg << ( u32 )GameServer->GetGameTime();

  *tmpMsg << ( u32 )nClientTime;

  *tmpMsg << ( u8 )0xe5; // ??? varies
  *tmpMsg << ( u8 )0x0a; // ??? varies
  *tmpMsg << ( u8 )0xbb; // ??? varies
  *tmpMsg << ( u8 )0x00; // ??? usually 0

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepZoningMsg( PClient* nClient, u32 nLocation, u16 nEntity )
{
  PMessage* tmpMsg = new PMessage( 50 );

  nClient->IncreaseUDP_ID();

  tmpMsg->Fill( 0 );
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << ( u16 )0x000c; // cmd
  tmpMsg->SetNextByteOffset( 38 );
  *tmpMsg << ( u32 )0xffffffff;
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u16 )nEntity;
  *tmpMsg << ( u16 )0x0000;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepAddToListMsg( PClient* nClient, u32 nLocation, u16 nEntity )
{
  PMessage* tmpMsg = new PMessage( 23 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x00000002;
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u16 )nEntity;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftUseMsg( PClient* nClient, u32 nLocation, u16 nEntity, u8 nEntityType )
{
  PMessage* tmpMsg = new PMessage( 43 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x0f; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x38;
  *tmpMsg << ( u8 )0x04; // Accepted (?)
  *tmpMsg << ( u8 )nEntityType; // "Sewer Level"
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u16 )nEntity;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftFailedMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 14 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x08; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x38;
  *tmpMsg << ( u8 )0x03; // Refused

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildChangeLocationMsg( PClient* nClient, u32 nLocation, u16 nEntity, u8 nEntityType, u32 nRawItemID )
{
  PMessage* tmpMsg = new PMessage( 28 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  if ( nRawItemID )
  {
    *tmpMsg << ( u8 )0x06; // Sub message length;
    *tmpMsg << ( u8 )0x2d; // Item use response;
    *tmpMsg << ( u32 )nRawItemID;
    *tmpMsg << ( u8 )0x0a; // Use allowed
  }

  *tmpMsg << ( u8 )0x0f; // Sub message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x38;
  *tmpMsg << ( u8 )0x04; // Accepted (?)
  *tmpMsg << ( u8 )nEntityType;
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u16 )nEntity;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildEntityPositionMsg( PClient* nClient, u16 pX, u16 pY, u16 pZ )
{
  PMessage* tmpMsg = new PMessage( 18 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << ( u16 )0x000a;
  *tmpMsg << ( u16 )( pY + 768 );
  *tmpMsg << ( u16 )( pZ + 768 );
  *tmpMsg << ( u16 )( pX + 768 );

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );
  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharAptLocInfoMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 21 );
  nClient->IncreaseUDP_ID();

  int BaseAppId = nClient->GetChar()->GetBaseApartment();
  u32 AptLocation = ( u32 )Appartements->GetAptLocation( BaseAppId );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0f;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x0000000b;
  *tmpMsg << ( u32 )AptLocation;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildSubskillIncMsg( PClient* nClient, u8 nSubskill, u16 nSkillPoints )
{
  PMessage* tmpMsg = new PMessage( 33 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  nClient->IncreaseUDP_ID();
  *tmpMsg << ( u8 )0x09; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << ( u8 )0x41; // 0x28 ??

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID(); // testing ...
  *tmpMsg << ( u8 )0x11; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetTransactionID(); // testing / 0x0000 ????
  *tmpMsg << ( u8 )0x09; // ?
  *tmpMsg << ( u16 )nSubskill;
  *tmpMsg << ( u16 )nClient->GetChar()->Skill->GetSubSkill( nSubskill ); // nSubskill ?
  *tmpMsg << ( u16 )nSkillPoints;

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildChatAddMsg( PClient* nClient, u32 nAddedCharID, u8 nMode )
{
  PMessage* tmpMsg = new PMessage( 18 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x32;
  *tmpMsg << ( u8 )nMode;
  *tmpMsg << ( u32 )nAddedCharID;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildDoorOpenMsg( u32 nRawItemID, bool nDoubleDoor )
{
  //PMessage* tmpMsg = new PMessage(37);
  PMessage* tmpMsg = new PMessage( 21 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside

  *tmpMsg << ( u8 )0x0f; // Sub-message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; //++Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u8 )0x1b;
  *tmpMsg << ( u32 )nRawItemID;
  *tmpMsg << ( u8 )0x20; //?
  if ( nDoubleDoor )
  {
    *tmpMsg << ( u16 )0x0005; //?
    *tmpMsg << ( u16 )0x0000; //?
    *tmpMsg << ( u16 )0x1500; //?
  }
  else
  {
    *tmpMsg << ( u16 )0x0000; //?
    *tmpMsg << ( u16 )0x00c8; //? or 0x64 ?
    *tmpMsg << ( u16 )0x10ff; //?
  }

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

// Message from text.ini, section [MISC], id = 100+nTxtMsgId
PMessage* PMsgBuilder::BuildText100Msg( PClient* nClient, u8 nTxtMsgId, u32 nRawObjectID )
{
  PMessage* tmpMsg = new PMessage( 17 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x0c; // Message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x31;
  *tmpMsg << ( u8 )nTxtMsgId;
  *tmpMsg << ( u32 )nRawObjectID;

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

// Same as BuildText100Msg, but here we can specify *ANY* text from text.ini, not limited to section [MISC]
// 1: [DIALOG]
// 2: [STATEMENT]
// 3: [GUI] + [playertextures]
// 4: [TERMINAL]
// 5: [MENU]
// 6: [MISC]
// 7: [ITEMDESC]
// 8: [HELPTEXT]
PMessage* PMsgBuilder::BuildTextIniMsg( PClient* nClient, u8 nTxtGroupID, u16 nTxtID )
{
  PMessage* tmpMsg = new PMessage( 20 );

  nClient->IncreaseUDP_ID();
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0e; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25; // ??
  *tmpMsg << ( u8 )0x15; // ??
  *tmpMsg << nTxtGroupID;
  *tmpMsg << nTxtID;
  *tmpMsg << ( u8 )0x00; // ??
  *tmpMsg << ( u8 )0x00; // ??
  *tmpMsg << ( u8 )0x00; // ??

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharInteractionMenuMsg( PClient* nClient, u32 nRawTargetID )
{
  PMessage* tmpMsg = new PMessage( 17 );

  nClient->IncreaseUDP_ID();
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0b; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x4d;
  *tmpMsg << ( u32 )nRawTargetID;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildFurnitureActivateMsg( PClient* nClient, u32 nRawObjectID, u8 nActionValue )
{
  PMessage* tmpMsg = new PMessage( 12 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x06; // SubMessage length;
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u8 )nActionValue; // known valid are 5 (ring), 9 (remove) and 10 (clic)

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseFurnitureMsg( PClient* nClient, u32 nRawObjectID )
{
  PMessage* tmpMsg = new PMessage( 24 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x06; // SubMessage length;
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u8 )0x0a;

  *tmpMsg << ( u8 )0x0b; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x17;
  *tmpMsg << ( u32 )nRawObjectID;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVhcTerminalMsg( PClient* nClient, u32 nRawObjectID )
{
  PMessage* tmpMsg = new PMessage( 24 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x06; // SubMessage length;
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u8 )0x0a;

  *tmpMsg << ( u8 )0x07; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x4a;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseGogoMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 17 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x0b; // Message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x0000000d; // cmd

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVentureWarpMsg( PClient* nClient, u32 nRawObjectID )
{
  PMessage* tmpMsg = new PMessage( 17 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x0b; // Message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x00000008; // cmd
  *tmpMsg << ( u32 )0x00000007; // cmd ?
  *tmpMsg << ( u32 )0x00000002; // ?
  *tmpMsg << ( u16 )0x0004; // ?
  *tmpMsg << nRawObjectID;
  // *tmpMsg << (u8)0x13; // ? Seems we can do without...

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcAccessRequestMsg( PClient* nClient, u32 nRequestId, u32 nRequesterCharId, u32 nRequesterLocalId, u32 nVhcRawObjectID )
{
  PMessage* tmpMsg = new PMessage( 40 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x22; // Message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x00000008; // cmd
  *tmpMsg << ( u32 )0x00000005; // cmd ?
  *tmpMsg << ( u32 )nRequestId;
  *tmpMsg << ( u16 )0x000c; // ? length ?
  *tmpMsg << nRequesterCharId; //u32
  *tmpMsg << nRequesterLocalId; // ? u32
  *tmpMsg << nVhcRawObjectID;
  *tmpMsg << ( u8 )0x08; // ?

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}
/* S->C
13:ff:00:72:d6:22:
03:ff:00:1f:03:00:
3d:
08:00:00:00:
05:00:00:00:
01:00:00:00:
0c:00:
2f:d8:01:00:
02:00:00:00:
5d:03:00:00:
08:
*/

/* C->S RESP: OK
13:79:00:ec:d5:17:
03:79:00:1f:03:00:
3d:
09:00:00:00:
06:00:00:00:
02:00:00:00:
01:00:
01:00
*/
/* C->S RESP: NOK
13:74:00:e7:d5:17:
03:74:00:1f:03:00:
3d:
09:00:00:00:
06:00:00:00:
01:00:00:00:
01:00:
00:00

*/

PMessage* PMsgBuilder::BuildCharUseGenrepMsg( PClient* nClient, u32 nRawObjectID, u32 nLocation, u16 nEntity )
{
  PMessage* tmpMsg = new PMessage( 24 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x06; // SubMessage length;
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u8 )0x0a;

  // this submessage is only needed to set to location/entity of the GR for a potential record in the char's GR list
  *tmpMsg << ( u8 )0x0d; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nLocation;
  *tmpMsg << ( u16 )nEntity;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseLiftMsg( PClient* nClient, u32 nRawObjectID, u16 nAptPlace )
{
  PMessage* tmpMsg = new PMessage( 29 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x06; // SubMessage length;
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u8 )0x0a;

  *tmpMsg << ( u8 )0x11; // SubMessage length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x38;
  *tmpMsg << ( u8 )0x01;
  *tmpMsg << ( u32 )nRawObjectID;
  *tmpMsg << ( u16 )nAptPlace;
  *tmpMsg << ( u16 )0x0000;
  *tmpMsg << ( u8 )0x00;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharShowGlowCircleMsg( PClient* nClient )
{
  PMessage* tmpMsg = new PMessage( 14 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // UDP ID placeholder
  *tmpMsg << ( u16 )0x0000; // SessionID placeholder
  *tmpMsg << ( u8 )0x08;  // Len (static, always 0x08
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // Sub UDP ID placeholder
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3c; // Command SHOW GLOWING CIRCLE (kinda ^^)
  *tmpMsg << ( u8 )0x01; // "on" ?

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharMoneyUpdateMsg( PClient* nClient, u32 nCredits )
{
  PMessage* tmpMsg = new PMessage( 21 );
  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0f; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25; // cmd
  *tmpMsg << ( u8 )0x13; // cmd
  *tmpMsg << ( u16 )nClient->GetTransactionID();
  *tmpMsg << ( u8 )0x04; // cmd
  *tmpMsg << nCredits;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildUndefineduseMsg( PClient* nClient, u8 nValue )
{
  PMessage* tmpMsg = new PMessage( 15 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x09; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u8 )0x23;
  *tmpMsg << nValue;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg2( PClient* nClient, u16 nV1, u16 nV2, u16 nV3, u16 nV4, u16 nV5, u16 nV6, u16 nV7 )
{
  // lol? Whats this?
  PMessage* tmpMsg = new PMessage( 28 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x16; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25; // cmd
  *tmpMsg << ( u8 )0x22; // cmd
  *tmpMsg << nV1;
  *tmpMsg << nV2;
  *tmpMsg << nV3;
  *tmpMsg << nV4;
  *tmpMsg << nV5;
  *tmpMsg << nV6;
  *tmpMsg << nV7;
  /* *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x64; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x00; // ??
   *tmpMsg << (u8)0x00; // ??
  */

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg3( PClient* nClient, u8 nSlot )
{
  PMessage* tmpMsg = new PMessage( 19 );
  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0d; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25; // cmd
  *tmpMsg << ( u8 )0x13; // cmd
  *tmpMsg << ( u16 )nClient->GetTransactionID();
  *tmpMsg << ( u8 )0x0b; // cmd
  *tmpMsg << nSlot; // ??
  *tmpMsg << ( u8 )0x00; // ??

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg4( PClient* nClient, u16 nValue1 )
{
  PMessage* tmpMsg = new PMessage( 16 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x0a; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x2f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x02; // ??
  *tmpMsg << ( u8 )0x02; // ??
  *tmpMsg << nValue1;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildContainerContentList( PContainer* nContainer, u8 nLocType )
{
  PMessage* tmpMsg = new PMessage( 256 );
  std::vector< PContainerEntry* >* Entries = nContainer->GetEntries();
  PContainerEntry* tEntry;
  PMessage* entryMsg;

//Console->Print(YELLOW, BLACK, "BuildContainerContentList for loc %d", nLocType);
  if ( nLocType != INV_LOC_BOX )
  {
    if ( nLocType == INV_LOC_BACKPACK )
      *tmpMsg << ( u16 )Entries->size(); // items nb
    else
      *tmpMsg << ( u8 )Entries->size(); // items nb
  }

  for ( u16 i = 0; i < Entries->size(); ++i )
  {
    tEntry = Entries->at( i );
    entryMsg = BuildContainerContentEntry( tEntry, nLocType );
//if(tEntry->mItem->mItemID == 390)
//{
//Console->Print(YELLOW, BLACK, "BuildContainerContentList entry %d - size %d", i, entryMsg->GetSize());
//entryMsg->Dump();
//}
    *tmpMsg << *entryMsg;
    delete entryMsg;
  }

  delete Entries;
  return tmpMsg;
}

PMessage* PMsgBuilder::BuildContainerContentEntry( PContainerEntry* nEntry, u8 nLocType )
{
  PMessage* tmpMsg = new PMessage( 16 );
  PItem* tItem;
  u8 dataFlags, Qualifier;

  tItem = nEntry->mItem;
  dataFlags = Qualifier = 0x00 ;

  if (( tItem->mItemID == 390 ) /* testing */ || tItem->mLoadedAmmoNb )
  {
    dataFlags |= 0x01;
  }
  if (( tItem->GetType() == ITEM_TYPE_WEAPON ) || ( tItem->GetType() == ITEM_TYPE_BLUEPRINT ) || ( tItem->GetType() == ITEM_TYPE_WRECKEDPART )/*|| (tItem->GetType() == ITEM_TYPE_APARTMENTKEY) || (tItem->GetType() == ITEM_TYPE_CLANKEY) || (tItem->GetType() == ITEM_TYPE_VHCKEY) */ ) // testing loaded ammo type & BP attributes
  {
    dataFlags |= 0x20;
  }

  switch ( tItem->GetType() )
  {
    case ITEM_TYPE_WEAPON:
    case ITEM_TYPE_AUTOWEAPON:
      Qualifier = 6;
      dataFlags |= 0x02;
      break;
    case ITEM_TYPE_IMPLANT:
    case ITEM_TYPE_ARMOR:
      Qualifier = 2;
      dataFlags |= 0x02;
      break;
    default:
      Qualifier = 0;
      break;
  }

  if ( tItem->IsStackable() && tItem->mStackSize )
  {
    dataFlags |= 0x04;
  }

  if (( tItem->mModificators ) || ( tItem->mItemID == 390 ) ) // TEST
    dataFlags |= 0x10;

  if ( tItem->mMaxSlots || ( tItem->mItemID == 390 ) ) // TEST
    dataFlags |= 0x40;



  if ( nLocType == INV_LOC_BOX )
    *tmpMsg << ( u8 )0x00;   // Size of item placeholder
  else
    *tmpMsg << ( u16 )0x0000;   // Size of item placeholder

  switch ( nLocType )
  {
    case INV_LOC_WORN:
      *tmpMsg << ( u8 )nEntry->mPosX; // X Location
      *tmpMsg << ( u8 )0x00; // just nothing
      break;
    case INV_LOC_BACKPACK:
      *tmpMsg << ( u8 )0x00; // just nothing again
      *tmpMsg << ( u8 )nEntry->mPosX; // X Location
      *tmpMsg << ( u8 )nEntry->mPosY; // Y Location
      break;
    case INV_LOC_GOGO:
      *tmpMsg << ( u8 )nEntry->mPosX;
      break;
    case INV_LOC_BOX:
    case INV_LOC_BOX2:
      break;
    default:
      break;
  }

  *tmpMsg << ( u16 )tItem->mItemID; // ItemID
  *tmpMsg << ( u8 )dataFlags; // (0x01|0x02|0x04|0x10|0x20|0x40|0x80); // Datatypes

  if ( dataFlags & 0x01 )
  {
    if ( tItem->GetType() == ITEM_TYPE_WEAPON ) // TESTING
      *tmpMsg << ( u8 )6; // Remaining ammos
    else
      *tmpMsg << ( u8 )tItem->mLoadedAmmoNb; // Remaining ammos
  }

  if ( dataFlags & 0x02 )
  {
    *tmpMsg << ( u8 )Qualifier; // Qual entries
    if ( Qualifier >= 2 )
    {
      *tmpMsg << ( u8 )tItem->mCurDuration; // current qual
      if ( Qualifier == 6 )
      {
        *tmpMsg << ( u8 )tItem->mDamages; // dmg
        *tmpMsg << ( u8 )tItem->mFrequency; // freq
        *tmpMsg << ( u8 )tItem->mHandling; // handl
        *tmpMsg << ( u8 )tItem->mRange; // range
      }
      *tmpMsg << ( u8 )tItem->mMaxDuration; // max qual
    }
  }

  if ( dataFlags & 0x10 )
  {
    if ( tItem->mItemID == 390 ) // test
      *tmpMsg << ( u8 )4;
    else
      *tmpMsg << ( u8 )tItem->mModificators; // addons bitflag: flashlight=1, scope, silencer, laserpointer
  }

  if ( dataFlags & 0x40 )
  {
    if ( tItem->mItemID == 390 ) // test
    {
      *tmpMsg << ( u8 )3;
      *tmpMsg << ( u8 )3;
      *tmpMsg << ( u16 )0x000b; // enlarged
      *tmpMsg << ( u16 )0x05de; // phosophore
      *tmpMsg << ( u16 )( -3 ); // silencer
    }
    else
    {

      *tmpMsg << ( u8 )tItem->mUsedSlots; // used slots
      *tmpMsg << ( u8 )tItem->mMaxSlots; // max slots
      for ( u8 j = 0; j < tItem->mMaxSlots; ++j )
        *tmpMsg << ( u16 )(( j < tItem->mUsedSlots ) ? tItem->mSlot[j] : 0 ); // mod in slot
    }
  }

  if ( dataFlags & 0x20 ) // loaded ammo type ????
  {
    u16 lengthFieldOffset = tmpMsg->GetNextByteOffset();
    *tmpMsg << ( u16 )0x0000; // length placeholder

    if ( tItem->GetType() == ITEM_TYPE_WEAPON )
    {
      *tmpMsg << ( u8 )0x01; // ammo info
      *tmpMsg << ( u8 )0x04; // total length ?
      *tmpMsg << ( u8 )0x00; // + baseammo => current ammoId. 0xff => undefined
      *tmpMsg << ( u8 )0xff; // supported ammos bitmap (all here)
    }

    if ( false && ( tItem->GetType() == ITEM_TYPE_APARTMENTKEY ) ) // activated Apartment key
    {
      *tmpMsg << ( u8 )0x02; // ammo info
      *tmpMsg << ( u8 )0x06; // total length
      *tmpMsg << ( u32 )123456; // apartmentObjectID ?
    }

    if ( false && ( tItem->GetType() == ITEM_TYPE_CLANKEY ) ) // activated ClanKey
    {
      *tmpMsg << ( u8 )0x04; // BP of... info
      *tmpMsg << ( u8 )0x0a; // total length
      *tmpMsg << ( u32 )1234; // ClanID ?
      *tmpMsg << ( u32 )123456; // apartmentObjectID ?
    }

    if ( tItem->GetType() == ITEM_TYPE_BLUEPRINT ) // BP
    {
      *tmpMsg << ( u8 )0x05; // BP of... info
      *tmpMsg << ( u8 )0x06; // total length
      *tmpMsg << ( u32 )486; // ItemID ("Tangent Sniper Rifle")
    }

    if ( false && ( tItem->GetType() == ITEM_TYPE_VHCKEY ) ) // activated VHC Key
    {
      *tmpMsg << ( u8 )0x08; // VHC Key
      *tmpMsg << ( u8 )0x0a; // total length
      *tmpMsg << ( u32 )654321; // vhcObjectID ?
      *tmpMsg << ( u32 )123456; // Owner CharID ?
    }

    if ( false && ( tItem->GetType() == ITEM_TYPE_WRECKEDPART ) ) // Identified rare part
    {
      *tmpMsg << ( u8 )0x09; // Rare part
      *tmpMsg << ( u8 )0x05; // total length
      *tmpMsg << ( u16 )453; // Rare Item ID ? REDEEMER
      *tmpMsg << ( u8 )0; // ??
    }

    if (( tItem->mConstructorId ) || ( tItem->mItemID == 390 ) ) // Named item /itemId 390: test
    {
      *tmpMsg << ( u8 )0x0a; // constructor info
      *tmpMsg << ( u8 )0x06; // total length
      //*tmpMsg << (u32)tItem->mConstructorId; // charID
      *tmpMsg << ( u32 )2;
    }

    tmpMsg->U16Data( lengthFieldOffset ) = tmpMsg->GetNextByteOffset() - lengthFieldOffset - 2;
  }

  if ( dataFlags & 0x04 )
  {
    *tmpMsg << ( u32 )tItem->mStackSize;
  }


  if ( nLocType == INV_LOC_BOX )
    tmpMsg->U8Data( 0 ) = tmpMsg->GetSize() - 1;
  else
    tmpMsg->U16Data( 0 ) = tmpMsg->GetSize() - 2;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharOpenContainerMsg( PClient* nClient, u32 nContainerID, PContainer* nContainer )
{
  PMessage* ContentList = BuildContainerContentList( nContainer, INV_LOC_BOX );

  PMessage* tmpMsg = new PMessage();
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x26;
  *tmpMsg << nContainerID;
  *tmpMsg << ( u8 )0x00; // Always the same on item containers? // 0x01 for Trader (NeoX gameclient 3608)
  *tmpMsg << ( u8 )0x64; // Always the same on item containers?
  *tmpMsg << ( u8 )0x00; // Always the same on item containers?
  *tmpMsg << ( u8 )0x08; // 0x08 when container is filled, 0x00 when not? At least it works..

  *tmpMsg << ( u16 )( ContentList->GetSize() );
  *tmpMsg << *ContentList;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  delete ContentList;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildItemMoveMsg( PClient* nClient, u8 nSource, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt )
{
  PMessage* tmpMsg = new PMessage( 26 );
  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x14; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetTransactionID();
  *tmpMsg << ( u8 )0x14; // ItemMove Answer
  *tmpMsg << nSource;
  *tmpMsg << nSrcX;
  *tmpMsg << nSrcY;
  *tmpMsg << nDestination;
  *tmpMsg << nDestX;
  *tmpMsg << nDestY;
  *tmpMsg << nItemCnt;
  *tmpMsg << ( u8 )0x00; // ??
  *tmpMsg << ( u8 )0x00; // ??

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildBoxItemMoveMsg( PClient* nClient, PContainerEntry* nEntry, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt )
{
  PMessage* tmpMsg = new PMessage( 64 );
  PMessage* entryMsg = BuildContainerContentEntry( nEntry, INV_LOC_BOX2 );

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x09; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x25;
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetTransactionID();
  *tmpMsg << ( u8 )0x17; // BoxItemMove Answer Src
  *tmpMsg << ( u8 )INV_LOC_BOX; // Src = Box
  *tmpMsg << nSrcX;
  *tmpMsg << nSrcY;
  *tmpMsg << nItemCnt;
  *tmpMsg << ( u8 )0x00; // Qty high
  *tmpMsg << ( u8 )0x18; // BoxItemMove Answer Dst
  *tmpMsg << nDestination;
  *tmpMsg << nDestX;
  *tmpMsg << nDestY;
  *tmpMsg << *entryMsg;
  *tmpMsg << ( u8 )0x12; // ? vary ...
  *tmpMsg << ( u8 )0x00;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  delete entryMsg;
  return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildItemAmmoUpdateMsg (PClient* nClient, PContainerEntry* nEntry, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt)
{
  PMessage* tmpMsg = new PMessage(64);
  PMessage* entryMsg = BuildContainerContentEntry(nEntry, INV_LOC_BOX2);

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID();

  *tmpMsg << (u8)0x13;
  *tmpMsg << (u16)nClient->GetUDP_ID();
  *tmpMsg << (u16)nClient->GetSessionID();

  *tmpMsg << (u8)0x09; // Message length
  *tmpMsg << (u8)0x03;
  *tmpMsg << (u16)nClient->GetUDP_ID();
  *tmpMsg << (u8)0x1f;
  *tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x25;
  *tmpMsg << (u8)0x13;
  *tmpMsg << (u16)nClient->GetTransactionID();
  *tmpMsg << (u8)0x17; // BoxItemMove Answer Src
  *tmpMsg << (u8)INV_LOC_BOX; // Src = Box
  *tmpMsg << nSrcX;
  *tmpMsg << nSrcY;
  *tmpMsg << nItemCnt;
  *tmpMsg << (u8)0x00; // Qty high
  *tmpMsg << (u8)0x18; // BoxItemMove Answer Dst
  *tmpMsg << nDestination;
  *tmpMsg << nDestX;
  *tmpMsg << nDestY;
  *tmpMsg << *entryMsg;
  *tmpMsg << (u8)0x12; // ? vary ...
  *tmpMsg << (u8)0x00;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  delete entryMsg;
  return tmpMsg;
}*/
/* Resp:
03:33:00:1f:01:00:25:13
c2:01:0a:00:02:00:00:00 ??
c3:01:05:03:00:00:12 Update ammo left
c4:01:05:02:00:00:0c Update ammo left
*/

PMessage* PMsgBuilder::BuildStartHackGameMsg( PClient* nClient, u32 nWorldObjID, u8 nHackDifficult )
{
  PMessage* tmpMsg = new PMessage( 22 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x10;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x28;
  *tmpMsg << nWorldObjID;
  *tmpMsg << nHackDifficult;
  *tmpMsg << ( u8 )0x28;
  *tmpMsg << ( u8 )0x5c;
  *tmpMsg << ( u8 )0xcf;
  *tmpMsg << ( u8 )0x3e;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildSubwaySpawnMsg( PClient* nClient, bool IsSecondMessage )
{
  PMessage* tmpMsg = new PMessage( 197 );
  u16 First = IsSecondMessage ? 6 : 0;
  u16 Last = IsSecondMessage ? 10 : 5;

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( u16 )0x0000; // placeholder for SessionID();

  for ( u16 i = First; ( i <= Last ); i++ )
  {
    nClient->IncreaseUDP_ID();
    *tmpMsg << ( u8 )0x11; //msg size
    *tmpMsg << ( u8 )0x03;
    *tmpMsg << ( u16 )nClient->GetUDP_ID();
    *tmpMsg << ( u8 )0x28;
    *tmpMsg << ( u16 )0x0027;
    *tmpMsg << ( u32 )( PSubway::mCabsBaseId + i );
    *tmpMsg << ( u16 )0x0000;
    *tmpMsg << ( u8 )0x00;
    *tmpMsg << ( u16 )Subway->mSubways[i].mPosition;
    *tmpMsg << ( u8 )0x00;
    *tmpMsg << ( u8 )Subway->mSubways[i].mDoorOpened;;

    nClient->IncreaseUDP_ID(); // vhc health update
    *tmpMsg << ( u8 )0x0d; //msg size
    *tmpMsg << ( u8 )0x03;
    *tmpMsg << ( u16 )nClient->GetUDP_ID();
    *tmpMsg << ( u8 )0x2d;
    *tmpMsg << ( u32 )( PSubway::mCabsBaseId + i );
    *tmpMsg << ( u8 )0x0a;
    *tmpMsg << ( u32 )PSubway::mCabsBaseHealth;
  }

  tmpMsg->U16Data( 1 ) = nClient->GetUDP_ID();
  tmpMsg->U16Data( 3 ) = nClient->GetSessionID();

  return tmpMsg;
}

/*
PMessage* PMsgBuilder::BuildSubwayFullUpdateMsg(PClient* nClient)
{
    PMessage* tmpMsg = new PMessage(148);
    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)0x0000; // placeholder for UDP_ID;
    *tmpMsg << (u16)0x0000; // placeholder for SessionID();

    for(u8 i=0; i<PSubway::mCabsNumber; i++)
    {
      nClient->IncreaseUDP_ID();
      *tmpMsg << (u8)0x0c; //msg size
      *tmpMsg << (u8)0x03;
      *tmpMsg << (u16)nClient->GetUDP_ID();
      *tmpMsg << (u8)0x32;
      *tmpMsg << (u32)(PSubway::mCabsBaseId + i);
      *tmpMsg << (u8)0x00;
      *tmpMsg << (u16)Subway->mSubways[i].mPosition;
      *tmpMsg << (u8)Subway->mSubways[i].mDoorOpened;;
  }

    tmpMsg->U16Data(1) = nClient->GetUDP_ID();
    tmpMsg->U16Data(3) = nClient->GetSessionID();

    return tmpMsg;
}
*/

PMessage* PMsgBuilder::BuildSubwaySingleUpdateMsg( u32 nVehicleID, u16 nPosition, u8 nDoorOpened )
{
  PMessage* tmpMsg = new PMessage( 18 );
  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( u16 )0x0000; // placeholder for SessionID();

  *tmpMsg << ( u8 )0x0c; //msg size
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // ++UDP_ID placeholder
  *tmpMsg << ( u8 )0x32;
  *tmpMsg << ( u32 )nVehicleID;
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u16 )nPosition;
  *tmpMsg << ( u8 )nDoorOpened;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildSpawnWorldObjectMsg( u16 nModelID, u16 nFunctionID, u32 nWOID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotZ )
{
  PMessage* tmpMsg = new PMessage( 31 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // UDP placeholder
  *tmpMsg << ( u16 )0x0000; // Session placeholder
  *tmpMsg << ( u8 )0x16;  // Message length
  *tmpMsg << ( u8 )0x03;  // 0x03 commandset
  *tmpMsg << ( u16 )0x0000; // UDP placeholder
  *tmpMsg << ( u8 )0x1b;  // Subcommandset
  *tmpMsg << ( u32 )nWOID;  // WorldobjectID

  *tmpMsg << ( u8 )0x19;  // Positiondata follows
  *tmpMsg << ( u16 )nPosY;
  *tmpMsg << ( u16 )nPosZ;
  *tmpMsg << ( u16 )nPosX;
  *tmpMsg << ( u8 )nRotY;  // Rotation X
  *tmpMsg << ( u8 )nRotZ;  // Rotation Y
  *tmpMsg << ( u8 )nRotX;  // Rotation Z
  *tmpMsg << ( u16 )nModelID;
  *tmpMsg << ( u16 )nFunctionID;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildRemoveWorldObjectMsg( u32 nWOID )
{
  PMessage* tmpMsg = new PMessage( 14 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // UDP ID placeholder
  *tmpMsg << ( u16 )0x0000; // SessionID placeholder
  *tmpMsg << ( u8 )0x08;  // Len (static, always 0x08
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // Sub UDP ID placeholder
  *tmpMsg << ( u8 )0x26;  // Command FADE AWAY CHAR (kinda ^^)
  *tmpMsg << ( u32 )nWOID;  // WorldobjectID

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildDBRequestStatusMsg( PClient* nClient, std::string* nCommandName, u8 nStatus, u16 nErrCode )
{
  PMessage* tmpMsg = new PMessage( 32 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x14; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x2b;
  *tmpMsg << ( u8 )0x1a;
  *tmpMsg << ( u16 )( nCommandName->size() + 1 );
  *tmpMsg << ( u8 )nStatus;
  *tmpMsg << ( u16 )nErrCode;
  *tmpMsg << ( *nCommandName );

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildDBAnswerMsg( PClient* nClient, std::string* nCommandName, std::string* nAnswerData, u16 nRows, u16 nCols )
{
  u8 i, j, k;
  PMessage* tmpMsg = new PMessage( 32 );
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x14; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x2b;
  *tmpMsg << ( u8 )0x17;
  *tmpMsg << ( u16 )( nCommandName->size() + 1 );
  *tmpMsg << ( u16 )nRows;
  *tmpMsg << ( u16 )nCols;
  *tmpMsg << ( *nCommandName );

  for ( i = 0, k = 0; i < nRows; ++i )
  {
    for ( j = 0; j < nCols; ++j, ++k )
    {
      *tmpMsg << ( u16 )( nAnswerData[k].size() + 1 );
      *tmpMsg << nAnswerData[k];
    }
  }

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseVhcMsg( PClient* nClient, u32 nRawObjectID, u16 nVhcType, u16 nAvailableSeats )
{
  PMessage* tmpMsg = new PMessage( 24 );

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x13; // Message length;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x3d;
  *tmpMsg << ( u32 )0x0000000e; // cmd
  *tmpMsg << nRawObjectID;
  *tmpMsg << nVhcType;
  *tmpMsg << nAvailableSeats; // Bit flags

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcInfoMsg( PClient* nClient, PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 32 );
  PVhcCoordinates VhcPos = nVehicle->GetPosition();
  PVehicleInformation VhcInfo = nVehicle->GetInformation();

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x28;
  *tmpMsg << ( u16 )0x0031;
  *tmpMsg << ( u32 )nVehicle->GetLocalId();
  *tmpMsg << ( u8 )0x02;
  *tmpMsg << ( u16 )( VhcPos.GetY() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetZ() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetX() + 768 );
  *tmpMsg << ( u8 )VhcPos.GetUD();
  *tmpMsg << ( u16 )VhcPos.GetLR();
  *tmpMsg << ( u16 )VhcPos.GetRoll();
  *tmpMsg << ( u8 )VhcInfo.GetVehicleType();
  *tmpMsg << ( u8 )0xff;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u16 )0x0000;
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u8 )0x01; // ? changes

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildVhcInfoMsg( PClient* nClient, PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 32 );
  PVhcCoordinates VhcPos = nVehicle->GetPosition();
  PVehicleInformation VhcInfo = nVehicle->GetInformation();

  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();

  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x28;
  *tmpMsg << ( u16 )0x0031;
  *tmpMsg << ( u32 )nVehicle->GetLocalId();
  *tmpMsg << ( u8 )0x02;
  *tmpMsg << ( u16 )( VhcPos.GetY() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetZ() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetX() + 768 );
  *tmpMsg << ( u8 )VhcPos.GetUD();
  *tmpMsg << ( u16 )VhcPos.GetLR();
  *tmpMsg << ( u16 )VhcPos.GetRoll();
  *tmpMsg << ( u8 )VhcInfo.GetVehicleType();
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u32 )0x00000000;
  *tmpMsg << ( u16 )0x0000;
  u32 tCharId;
  for(u8 i = 0; i < 8; ++i)
  {
    if( (tCharId = nVehicle->GetSeatUser(i)) )
    {
      *tmpMsg << tCharId;
    }
    else
    {
      *tmpMsg << ( u32 )0xffffffff;
    }
    *tmpMsg << i;
    *tmpMsg << ( u16 )0x0000;
  }

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}*/
/* ????
13 f7 00 49 bf
5d
03 f7 00
28 31 00
c9 03 00 00 = Object ID
02
47 ff
00 cd
c3 c3
d7 
d7 ec
00 00
29
2b 65 35 8b 8c 6c 7f 80 96
5f 26 00 80 00
ff ff ff ff 00 00 00
ff ff ff ff 01 00 00
ff ff ff ff 02 00 00
ff ff ff ff 03 00 00
ff ff ff ff 04 00 00
ff ff ff ff 05 00 00
ff ff ff ff 06 00 00
ff ff ff ff 07 00 00

*/

PMessage* PMsgBuilder::BuildVhcHealthUpdateMsg( PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 19 );

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( u16 )0x0000; // placeholder for SessionID();

  *tmpMsg << ( u8 )0x0d; //msg size
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // placeholder for ++UDP_ID
  *tmpMsg << ( u8 )0x2d;
  *tmpMsg << ( u32 )nVehicle->GetLocalId();
  *tmpMsg << ( u8 )0x0a; // Health update
  *tmpMsg << ( f32 )( nVehicle->GetInformation().GetHealth() );

  return tmpMsg;
}

// NB: same as BuildCharSittingMsg. To be merged later when classes are adapted
PMessage* PMsgBuilder::BuildVhcPosUpdateMsg( PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 33 );
  PVhcCoordinates VhcPos = nVehicle->GetPosition();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
  *tmpMsg << ( u16 )0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside

  *tmpMsg << ( u8 )0x00; // Message length placeholder;
  *tmpMsg << ( u8 )0x32;
  *tmpMsg << ( u16 )( nVehicle->GetLocalId() & 0xffff );
  *tmpMsg << ( u8 )0x03; // We suppose we use move type 3, as in client message
  *tmpMsg << ( u16 )( VhcPos.GetY() + 768 ); // +768 or +0 ??? All Char/Vhc/NPC/Objet offset to clean up...
  *tmpMsg << ( u16 )( VhcPos.GetZ() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetX() + 768 );
  *tmpMsg << ( u8 )VhcPos.GetUD();
  *tmpMsg << ( u16 )VhcPos.GetLR();
  *tmpMsg << ( u16 )VhcPos.GetRoll();
  *tmpMsg << ( u16 )VhcPos.GetUnknown();
  *tmpMsg << ( u8 )0x00;
  *tmpMsg << ( u8 )VhcPos.GetAct();
  /* What is that for ????
  *tmpMsg << ( u8 )0x02; // <= these two u8 corresond to mUnknown... = additionnal data ?
  *tmpMsg << ( u8 )0x00; // maybe weapon related ?
  *tmpMsg << ( u16 )0x0000;
  *tmpMsg << ( u16 )0x0001;
  *tmpMsg << ( u16 )0x8000;
  *tmpMsg << ( u16 )0x8000;
  */

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildVhcPosUpdate2Msg( PSpawnedVehicle* nVehicle )
{
  PMessage* tmpMsg = new PMessage( 28 );
  PVhcCoordinates VhcPos = nVehicle->GetPosition();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( u16 )0x0000; // placeholder for SessionID();

  *tmpMsg << ( u8 )0x00; //msg size
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )0x0000; // ++UDP_ID placeholder
  *tmpMsg << ( u8 )0x32;
  *tmpMsg << ( u16 )( nVehicle->GetLocalId() & 0xffff );
  *tmpMsg << ( u8 )0x03; // 0x07 in nc2.2, and one more field (same a received msg)
  *tmpMsg << ( u16 )( VhcPos.GetY() + 768 ); // +768 or +0 ??? All Char/Vhc/NPC/Objet offset to clean up...
  *tmpMsg << ( u16 )( VhcPos.GetZ() + 768 );
  *tmpMsg << ( u16 )( VhcPos.GetX() + 768 );
  *tmpMsg << ( u8 )VhcPos.GetUD();
  *tmpMsg << ( u16 )VhcPos.GetLR();
  *tmpMsg << ( u16 )VhcPos.GetRoll();
  *tmpMsg << ( u16 )VhcPos.GetUnknown();
  *tmpMsg << ( u8 )VhcPos.GetFF();
  *tmpMsg << ( u8 )VhcPos.GetAct();

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  return tmpMsg;
}

/*
ground vhc in NC1
C->S
[...] 32:fb:03:03:d4:8c:2f:84:a0:7b:7b:bf:c6:53:28:01:00:ff:05
S->C
[...] 32: fb:03: 03: d4:8c: 2f:84: a0:7b: 7b: bf:c6: 53:28: 01:00:ff:05

  *nMsg >> mVhcLocalId;
  *nMsg >> mMoveType; // 0 for subway, 3 for nc1 vhc, 7 for nc2 vhc // u8
  *nMsg >> mNewY;
  *nMsg >> mNewZ;
  *nMsg >> mNewX;
  *nMsg >> mNewUD; // u8
  *nMsg >> mNewLR;
  *nMsg >> mNewRoll;
  *nMsg >> mUnk1;
  *nMsg >> mFF; // u8
  *nMsg >> mAction; // u8

quad in NC2.2
C->S
[...] 32:c9:03:07:28:65:35:8b:8c:6c:7f:09:99:45:26:10:80:01:00:ff:05
S->C
[...] 32:c9:03:07:28:65:35:8b:8c:6c:7f:09:99:45:26:10:80:01:00:ff:05

glider in NC2.2
C->S
[...] 32:4b:03:07:13:57:a9:88:89:af:86:c1:89:ec:81:64:82:01:00:ff:00
S->C
[...] 32:4b:03:07:98:57:c8:88:59:b0:83:5c:88:13:81:64:82:01:00:ff:00:
[...] 32:4b:03:07:13:57:a9:88:89:af:86:c1:89:ec:81:64:82:01:00:ff:00:


*/
PMessage* PMsgBuilder::BuildTraderItemListMsg( PClient* nClient, u32 nTraderNpcID ) //, PContainer* nContainer)
{
//    PMessage* ContentList = BuildContainerContentList(nContainer, INV_LOC_BOX);
// Tmp manual content list:
  PMessage* ContentList = new PMessage();
  f32 PriceCoef = 1 / 1.379942;
  u8 Quality = 255; // Range 0 - 255

  //Item 1:
  *ContentList << ( u16 )0x05e6; // Item Id "Clan key for CityAdmin"
  *ContentList << ( u32 )304567; // Base (?) Item price = item.def price / 1.97
  // Displayed price is this price * 1.38 . Is this faction/barter factor ?
  //Item 2:
  *ContentList << ( u16 )0x05e7; // Item Id "Clan key for Diamond Real Estate"
  *ContentList << ( u32 )( 420285 * PriceCoef );
  //Item 3:
  *ContentList << ( u16 )0x05e8; // Item Id "Clan key for N.E.X.T."
  *ContentList << ( u32 )( 420285 * PriceCoef );
  //Item 4:
  *ContentList << ( u16 )0x060f; // Item Id "PLAZA - 2nd Lev. Apartment"
  *ContentList << ( u32 )( 245166 * PriceCoef );
  //Item 5:
  *ContentList << ( u16 )0x065c; // Item Id "Normal Viarosso Apartment Alamo Living"
  *ContentList << ( u32 )( 840571 * PriceCoef );
  //Item 6:
  *ContentList << ( u16 )0x065d; // Item Id "Luxus Viarosso Apartment Alamo Living"
  *ContentList << ( u32 )( 1260856 * PriceCoef );

  //Item 7:
  *ContentList << ( u16 )355; // Item Id "HEW �Liquid Fire� Rifle"
  *ContentList << ( u32 )( 1260856 * PriceCoef );

  PMessage* tmpMsg = new PMessage();
  nClient->IncreaseUDP_ID();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u16 )nClient->GetSessionID();
  *tmpMsg << ( u8 )0x00; // Message length
  *tmpMsg << ( u8 )0x03;
  *tmpMsg << ( u16 )nClient->GetUDP_ID();
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )nClient->GetLocalID();
  *tmpMsg << ( u8 )0x26;
  *tmpMsg << nTraderNpcID;
  *tmpMsg << ( u8 )0x01; // Traders inventory
  *tmpMsg << ( u16 )( ContentList->GetSize() / 6 ); // List entries
  *tmpMsg << ( u8 )Quality; // Items quality
  *tmpMsg << *ContentList;

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  delete ContentList;

  return tmpMsg;
}

// For testing - packet to be broadcasted to zone
PMessage* PMsgBuilder::BuildNpcDeathMsg( PClient* nClient, u32 nNpcId, u8 unknown1, u8 unknown2 )
{
  PMessage* tmpMsg = new PMessage( 19 );
  PChar *nChar = nClient->GetChar();

  *tmpMsg << ( u8 )0x13;
  *tmpMsg << ( u16 )0x0000; // placeholder for UDP_ID;
  *tmpMsg << ( u16 )0x0000; // placeholder for SessionID();

  *tmpMsg << ( u8 )0x00;  // Message length placeholder;
  *tmpMsg << ( u8 )0x1b;
  *tmpMsg << ( u32 )nNpcId;
  *tmpMsg << ( u8 )0x1f;
  *tmpMsg << ( u16 )( nChar->Coords.mY + 768 + 30 ); //move Npc near to char :p
  *tmpMsg << ( u16 )( nChar->Coords.mZ + 768 + 0 );
  *tmpMsg << ( u16 )( nChar->Coords.mX + 768 + 30 );
  *tmpMsg << ( u8 )2; // ??? 0x01=look at target, 0x02=?, 0x10 = kneel, 0x80 = die
  *tmpMsg << ( u8 )1; //0=> dead on health 0 / else alive on health 0. Changes in caps
  *tmpMsg << ( u8 )96; // health => 0 alive if prec >0 1-127 alive, <0 dead (ie u8 128-255 = neg signed values)
  *tmpMsg << ( u16 )259; // targetId (N)PC - Here: left copbot at NC entrance (zone 2008)
  *tmpMsg << ( u8 )0x00; // ? doesn't seem to change in caps
  *tmpMsg << ( u8 )0x00; // ? doesn't seem to change in caps
  *tmpMsg << ( u8 )0; // ? changes in caps
  *tmpMsg << ( u8 )0; // ? changes in caps // moving speed somewhere ?

  ( *tmpMsg )[5] = ( u8 )( tmpMsg->GetSize() - 6 );

  unknown2 = unknown1; // so that gcc doesn't complain if these vars are not used
  return tmpMsg;
}

/*
void Cmd_GiveItem (int ItemId, int Amount, int ClientNum)
{
 unsigned char SendBuffer[256];

// Inventory_AddNewItem (&Client_Sockets[ClientNum].CharInfo, ItemId, Amount);
 SendBuffer[0] = 0x13;
 SendBuffer[5] = 0x1b;
 SendBuffer[6] = 0x03;
 Network_IncrementUDP (ClientNum);
 *(unsigned short*)&SendBuffer[7] = Client_Sockets[ClientNum].UDP_ID;
 SendBuffer[9] = 0x1f;
 *(unsigned short*)&SendBuffer[10] = Client_Sockets[ClientNum].CharInfo.MapID;
 SendBuffer[12] = 0x25;
 SendBuffer[13] = 0x13;
 Client_Sockets[ClientNum].TransactionID++;
 *(unsigned short*)&SendBuffer[14] = Client_Sockets[ClientNum].TransactionID;//Transaction ID
 SendBuffer[16] = 0x18;
 SendBuffer[17] = 0x03; //Location
 SendBuffer[18] = 0xff; // \/
 SendBuffer[19] = 0xff; //Anywhere
 SendBuffer[20] = 0x08;
 SendBuffer[21] = 0x00;
 *(unsigned short*)&SendBuffer[22] = ItemId; //Item Id
 SendBuffer[24] = 0x05;
 SendBuffer[25] = 0x01;
 *(unsigned short*)&SendBuffer[26] = Amount; //Quantity
 SendBuffer[28] = 0x00;
 SendBuffer[29] = 0x00;
 *(unsigned short*)&SendBuffer[30] = Client_Sockets[ClientNum].CharInfo.ItemTransactionID; //Id of Purchased Item (Client sends another packet for placement of new item)

 *(unsigned short*)&SendBuffer[1] = Client_Sockets[ClientNum].UDP_ID;
 *(unsigned short*)&SendBuffer[3] = Client_Sockets[ClientNum].UDP_ID_HIGH;

 Network_SendUDP (SendBuffer, 32, ClientNum);
}
*/

/* Unkown use packets (from nc2.2)
13:81:00:81:e2: 0c: 03:81:00:23: 12:00: 07:00:00:00:00:00  // Weather realted ? / thunderstorm trigger ?

13:56:00:56:e2: 40: 03:56:00:1f:01:00:25:13: f1:18:13:01:77:05:48:c7: f2:18:13:02:16:74:61:c7: f3:18:13:03:17:74:61:c7: f4:18:13:04:18:74:61:c7: f5:18:13:05:1f:2a:60:c7: f6:18:13:06:1f:2a:60:c7: f7:18:13:0b:3e:8f:6d:c7

13:5b:00:5b:e2: 07: 03:5b:00:2e: 01:06:00 // Weather related ?

13:5c:00:5c:e2: 0c: 03:5c:00:1f:01:00:25:13: f8:18:0e:02

13:xx:xx:xx:xx: 09: 03:68:00:2d: 6d:03:00:00: 06 // Action update ? other NPC update ? some vanish ?
*/
