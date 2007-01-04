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

PMessage* PMsgBuilder::BuildCharHelloMsg(PClient* nClient)
{
  PChar *nChar = nClient->GetChar();
  u32 nSkin, nHead, nTorso, nLegs;
  u8 nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness;

  nChar->GetCurrentLook(nSkin, nHead, nTorso, nLegs);
  nChar->GetCurrentBodyColor(nHeadColor, nTorsoColor, nLegsColor, nHeadDarkness, nTorsoDarkness, nLegsDarkness);

  PMessage* tmpMsg = new PMessage(80);

    u16 ItemVal1 = 0;
    u16 item_inuse = nChar->GetItemInHand();
    if(item_inuse > 0)
    {
        const PDefItems *def = GameDefs->GetItemsDef(item_inuse);
        if(def != NULL)
            ItemVal1 = (u16)def->GetValue1();
        else
            Console->Print("%s Unable to get itemdata for itemID %d", Console->ColorText(RED, BLACK, "[ERROR]"), item_inuse);
    }


  //nClient->IncreaseUDP_ID(); // This must be done outside

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // size placeholder, set later in the function
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x25;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u32)nChar->GetID();

	*tmpMsg << (u8)0x60; // 0x40 if current faction epic done (master), | 0x80 to display [afk] | 0x20 if LE in
	*tmpMsg << (u8)((nChar->GetSpeedOverride() == 255) ? 10 : nChar->GetSpeedOverride()); // move speed, reset by client (and for him only) when getting fall damage
	*tmpMsg << (u8)0x08; // ??? something to do with speed ?
	//*tmpMsg << (u16)0x0000;// WeaponID of the weapon in hand
	*tmpMsg << (u16)ItemVal1;
	*tmpMsg << (u8)0x01; // ???
	*tmpMsg << (u8)0x01; // ???
	*tmpMsg << (u8)(128 + nChar->GetSoullight());
	*tmpMsg << (u8)nChar->GetMainRank(); // in fact, Ranks are of type s8, but it doesn't matter much
	*tmpMsg << (u8)nChar->GetCombatRank();

	*tmpMsg << (u8)nChar->GetFaction();

	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x0f; // size of the next bloc (skin + ?clan?)
  // Current skin
	*tmpMsg << (u16)nSkin;
	*tmpMsg << (u8)nHead;
	*tmpMsg << (u8)nTorso;
	*tmpMsg << (u8)nLegs;
  // Skin colors
	*tmpMsg << (u8)nHeadColor;
	*tmpMsg << (u8)nTorsoColor;
	*tmpMsg << (u8)nLegsColor;
	*tmpMsg << (u8)nHeadDarkness; // (0=bright 255=dark)
	*tmpMsg << (u8)nTorsoDarkness;
	*tmpMsg << (u8)nLegsDarkness;
	*tmpMsg << (u8)0x00; // ???
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x00;

	//Name
	*tmpMsg << (u8) ((nChar->GetName()).length()+1);
	*tmpMsg << (nChar->GetName()).c_str();
	//Body effects
	u8 cBodyEffect, cEffectDensity;
	nChar->GetBodyEffect(cBodyEffect, cEffectDensity);
	if (cBodyEffect)
	{
	  *tmpMsg << (u8)0x06; // size of effect list : 6 bytes/effect. Only one supported atm
  	*tmpMsg << (u8)cBodyEffect; // effect type (0=none, effecive values 1 - 17)
  	*tmpMsg << (u8)cEffectDensity; // density: 0=max, 0xff=min (for some effects only)
  	*tmpMsg << (u8)0x00; // ???
  	*tmpMsg << (u8)0x00; // ???
  	*tmpMsg << (u8)0x00; // ???
  	*tmpMsg << (u8)0x00; // ???
  }
  else
  {
	  *tmpMsg << (u8)0x00; // size of empty effect list
	}

	*tmpMsg << (u8)0x00; // ending null
// alternate interpretation to this "ending null"/optional bloc:
/*	*tmpMsg << (u8)0x04; // size of unknown bloc ... 0x00 when empty (aka the "ending null")
	*tmpMsg << (u8)0x0b; // vary ... ??? 0b, eb, ee, ...
	*tmpMsg << (u8)0x44; // vary ... ???
	*tmpMsg << (u8)0x00; // these two seem always null
	*tmpMsg << (u8)0x00; */

	(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

	return tmpMsg;
}


PMessage* PMsgBuilder::BuildReqInfoAnswerMsg (PClient* nClient, u16 nReqType, u32 nInfoId, void* nResponse, u16 nResponseLength)
{
  PMessage* tmpMsg;

  tmpMsg = new PMessage(18 + nResponseLength);
  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x23;
	*tmpMsg << (u8)0x06;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u16)nReqType; // wrong size here (u32) for buffer size u16 in NeoX
	*tmpMsg << (u32)nInfoId;
	tmpMsg->Write(nResponse, nResponseLength);

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharHealthUpdateMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x30;
	*tmpMsg << (u8)0x64; //Head Heath (% ?) // Not right here
	*tmpMsg << (u8)0x64; //Body Heath
	*tmpMsg << (u8)0x64; //Feet Heath
	*tmpMsg << (u8)0x01;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharPosUpdateMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(32);
  PChar* nChar = nClient->GetChar();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x1b;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u16)0x0000; // pad to keep LocalID on u16
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)((nChar->Coords).mY);
	*tmpMsg << (u16)((nChar->Coords).mZ);
	*tmpMsg << (u16)((nChar->Coords).mX);
	*tmpMsg << (u16)(31910+(nChar->Coords).mUD-50);  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
	*tmpMsg << (u16)(31820+(nChar->Coords).mLR*2-179); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
	*tmpMsg << (u8)((nChar->Coords).mAct);
	*tmpMsg << (u8)0x00;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharSittingMsg (PClient* nClient, u16 nData)
{ // This one was pure guess, but it works :p
  PMessage* tmpMsg = new PMessage(32);
  PChar* nChar = nClient->GetChar();
  nData = nData;
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x32;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u16)0x0000; // pad to keep LocalID on u16
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)((nChar->Coords).mY);
	*tmpMsg << (u16)((nChar->Coords).mZ);
	*tmpMsg << (u16)((nChar->Coords).mX);
	*tmpMsg << (u16)(31910+(nChar->Coords).mUD-50);  // Up - Mid - Down  mUD=(d6 - 80 - 2a) NeoX original offset: 31910
	*tmpMsg << (u16)(31820+(nChar->Coords).mLR*2-179); // Compass direction mLR=(S..E..N..W..S [0-45-90-135-179]) There still is a small buggy movement when slowly crossing the South axis from the right
	*tmpMsg << (u8)((nChar->Coords).mAct);
	*tmpMsg << (u8)0x00;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildCharSittingMsg (PClient* nClient, u16 nData)
{
  PMessage* tmpMsg = new PMessage(32);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x32;
	*tmpMsg << (u16)nData; // Chair object ID
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u8)0xad; // ????
	*tmpMsg << (u8)0x80;
	*tmpMsg << (u8)0xf9;
	*tmpMsg << (u8)0x85;
	*tmpMsg << (u8)0x6a;
	*tmpMsg << (u8)0x98;
	*tmpMsg << (u8)0x7c;
	*tmpMsg << (u8)0x3f;
	*tmpMsg << (u8)0x8b;
	*tmpMsg << (u8)0x14;
	*tmpMsg << (u8)0x7e;
  *tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u16)0x0000;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}*/

PMessage* PMsgBuilder::BuildCharUseChairMsg (PClient* nClient, u32 nRawChairID)
{
  //PMessage* tmpMsg = new PMessage(31);
  PMessage* tmpMsg = new PMessage(18);
  //PChar* nChar = nClient->GetChar();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u16)0x0000; // nClient->GetSessionID()placeholder

	*tmpMsg << (u8)0x0c; // Sub message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // ++ nClient->GetUDP_ID() placeholder
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x21;
	*tmpMsg << (u32)nRawChairID;
	*tmpMsg << (u8)0x00;
/*
	*tmpMsg << (u8)0x0c; // Sub message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // ++ nClient->GetUDP_ID() placeholder
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x21;
	*tmpMsg << (u32)nRawChairID;
	*tmpMsg << (u8)0x00;
*/
  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharExitChairMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(22);
  PChar* nChar = nClient->GetChar();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u16)0x0000; // nClient->GetSessionID()placeholder
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x22;
	*tmpMsg << (u16)((nChar->Coords).mY + 768) ;
	*tmpMsg << (u16)((nChar->Coords).mZ + 768) ;
	*tmpMsg << (u16)((nChar->Coords).mX + 768) ;
	*tmpMsg << (u8)(nChar->Coords).mUD;
	*tmpMsg << (u8)(nChar->Coords).mLR;
	*tmpMsg << (u8)(nChar->Coords).mAct;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharJumpingMsg (PClient* nClient)
{
  /*PMessage* tmpMsg = new PMessage(22);
  PChar* nChar = nClient->GetChar();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u16)0x0000; // nClient->GetSessionID()placeholder
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x22;
	*tmpMsg << (u16)(nChar->Coords).mY; // TODO: set the wakeup position in front of
	*tmpMsg << (u16)(nChar->Coords).mZ; // the chair instead of last char pos.
	*tmpMsg << (u16)(nChar->Coords).mX;
	*tmpMsg << (u8)(nChar->Coords).mUD;
	*tmpMsg << (u8)(nChar->Coords).mLR;
	*tmpMsg << (u8)(nChar->Coords).mAct;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;*/
  nClient = NULL;
  return NULL;
}

PMessage* PMsgBuilder::BuildPacket0Msg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(70);
  PChar* nChar = nClient->GetChar();
  nClient->IncreaseUDP_ID();

    nClient->CharIsAwaitingWarpto(); // Update location
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x2c;
	*tmpMsg << (u8)0x01; // ??
	*tmpMsg << (u8)0x01; // ??
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (f32)((nChar->Coords).mY-32000);
	*tmpMsg << (f32)((nChar->Coords).mZ-32000);
	*tmpMsg << (f32)((nChar->Coords).mX-32000);
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u8)0x01; // ????
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u8)0x07;
	*tmpMsg << (u8)0x02;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;
	*tmpMsg << (u32)0x00000000;


  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);
  return tmpMsg;
}

PMessage* PMsgBuilder::BuildPingMsg (PClient* nClient, u32 nClientTime)
{
  PMessage* tmpMsg = new PMessage(15);
  u32 LocalTime = GameServer->GetGameTime();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x0b;
	*tmpMsg << (u32)LocalTime;
	*tmpMsg << (u32)nClientTime;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildBaselineMsg (PClient* nClient)
{
  PMessage* BaselineMsg = new PMessage(512);
  PMessage SectionMsg(256);

  PChar *nChar = nClient->GetChar();
  const PDefCharKind *def = GameDefs->GetCharKindDef(nChar->GetProfession());
  PSkillHandler *Skill = nChar->Skill;

  nClient->IncreaseTransactionID(3);

  // Head
  *BaselineMsg << (u8)0x22;
  *BaselineMsg << (u8)0x02; // ?
  *BaselineMsg << (u8)0x01; // ?

  // ---- Section 1 ----
  *BaselineMsg << (u8)0x01; // section id

  SectionMsg << (u8)0xfa; // ?  // // section content at offset 3
  SectionMsg << (u8)nChar->GetProfession();
  SectionMsg << (u16)nClient->GetTransactionID(); // Transaction ID ? 0x8aa0
  SectionMsg << (u32)nChar->GetID();
  SectionMsg << (u8)0x0e; // ? 0x0e in NeoX, 0x10 in Tinns ... doesn't seem to matter
  SectionMsg << (u8)0x00; // ?

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 2 ----
  *BaselineMsg << (u8)0x02; // section id
  SectionMsg << (u8)0x04; // ?  // section content at offset 3
  SectionMsg << (u8)0x04; // ?
  SectionMsg << (u16)500; // nChar->GetHealth();
  SectionMsg << (u16)500; // nChar->GetMaxHealth();
  SectionMsg << (u16)500; // nChar->GetMana();
  SectionMsg << (u16)500; // nChar->GetMaxMana();
  SectionMsg << (u16)500; // nChar->GetStamina();
  SectionMsg << (u16)500; // nChar->GetMaxStamina();
  SectionMsg << (u16)0x00ff; // ?
  SectionMsg << (u16)0x00ff; // ?
  SectionMsg << (u16)0x00e1; // (nChar->GetHealth() + 1); // ? Probably Head Health (45% of total) 0x0065
  SectionMsg << (u16)0x0147; // (nChar->GetHealth() + 1); // ? Torso Health (35% of total)
  SectionMsg << (u16)0x0147; // (nChar->GetHealth() + 1); // ? Legs Health (20% of total)
  SectionMsg << (u8)100; // 100 - SI
  SectionMsg << (u8)0x80;
  SectionMsg << (u16)0x0000;

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 3 ----
  *BaselineMsg << (u8)0x03; // section id

  SectionMsg << (u8)0x06; // ?  // section content at offset 3
  SectionMsg << (u8)0x09; // ?
  SectionMsg << (u32)0x00000000; // ?
  SectionMsg << (u32)0x00000000; // ?
  SectionMsg << (u8)0x01; // ?

  SectionMsg << (u8)Skill->GetMainSkill(MS_STR);
  SectionMsg << (u16)Skill->GetSP(MS_STR);
  SectionMsg << (u32)Skill->GetXP(MS_STR);
  SectionMsg << (u8)def->GetSkillInfo(MS_STR).mGrow;
  SectionMsg << (u8)def->GetSkillInfo(MS_STR).mMax;

  SectionMsg << (u8)Skill->GetMainSkill(MS_DEX);
  SectionMsg << (u16)Skill->GetSP(MS_DEX);
  SectionMsg << (u32)Skill->GetXP(MS_DEX);
  SectionMsg << (u8)def->GetSkillInfo(MS_DEX).mGrow;
  SectionMsg << (u8)def->GetSkillInfo(MS_DEX).mMax;

  SectionMsg << (u8)Skill->GetMainSkill(MS_CON);
  SectionMsg << (u16)Skill->GetSP(MS_CON);
  SectionMsg << (u32)Skill->GetXP(MS_CON);
  SectionMsg << (u8)def->GetSkillInfo(MS_CON).mGrow;
  SectionMsg << (u8)def->GetSkillInfo(MS_CON).mMax;

  SectionMsg << (u8)Skill->GetMainSkill(MS_INT);
  SectionMsg << (u16)Skill->GetSP(MS_INT);
  SectionMsg << (u32)Skill->GetXP(MS_INT);
  SectionMsg << (u8)def->GetSkillInfo(MS_INT).mGrow;
  SectionMsg << (u8)def->GetSkillInfo(MS_INT).mMax;

  SectionMsg << (u8)Skill->GetMainSkill(MS_PSI);
  SectionMsg << (u16)Skill->GetSP(MS_PSI);
  SectionMsg << (u32)Skill->GetXP(MS_PSI);
  SectionMsg << (u8)def->GetSkillInfo(MS_PSI).mGrow;
  SectionMsg << (u8)def->GetSkillInfo(MS_PSI).mMax;

  SectionMsg << (u16)0x0000; // ?
  SectionMsg << (u8)0x00; // ?
  SectionMsg << (u8)0x08; // ?

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 4 ----
  *BaselineMsg << (u8)0x04; // section id

  SectionMsg << (u8)0x2e; // ?  // section content at offset 3
  SectionMsg << (u8)0x02; // ?
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)Skill->GetSubSkill(SK_MC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_MC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_HC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_HC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_TRA);
  SectionMsg << (u8)Skill->GetSKPCost(SK_TRA);
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)Skill->GetSubSkill(SK_PC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_PC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_RC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_RC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_TC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_TC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_VHC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_VHC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_AGL);
  SectionMsg << (u8)Skill->GetSKPCost(SK_AGL);
  SectionMsg << (u8)Skill->GetSubSkill(SK_REP);
  SectionMsg << (u8)Skill->GetSKPCost(SK_REP);
  SectionMsg << (u8)Skill->GetSubSkill(SK_REC);
  SectionMsg << (u8)Skill->GetSKPCost(SK_REC);
  SectionMsg << (u8)Skill->GetSubSkill(SK_RCL);
  SectionMsg << (u8)Skill->GetSKPCost(SK_RCL);
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)Skill->GetSubSkill(SK_ATL);
  SectionMsg << (u8)Skill->GetSKPCost(SK_ATL);
  SectionMsg << (u8)Skill->GetSubSkill(SK_END);
  SectionMsg << (u8)Skill->GetSKPCost(SK_END);
  SectionMsg << (u8)Skill->GetSubSkill(SK_FOR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_FOR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_FIR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_FIR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_ENR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_ENR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_XRR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_XRR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_POR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_POR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_HLT);
  SectionMsg << (u8)Skill->GetSKPCost(SK_HLT);
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)Skill->GetSubSkill(SK_HCK);
  SectionMsg << (u8)Skill->GetSKPCost(SK_HCK);
  SectionMsg << (u8)Skill->GetSubSkill(SK_BRT);
  SectionMsg << (u8)Skill->GetSKPCost(SK_BRT);
  SectionMsg << (u8)Skill->GetSubSkill(SK_PSU);
  SectionMsg << (u8)Skill->GetSKPCost(SK_PSU);
  SectionMsg << (u8)Skill->GetSubSkill(SK_WEP);
  SectionMsg << (u8)Skill->GetSKPCost(SK_WEP);
  SectionMsg << (u8)Skill->GetSubSkill(SK_CST);
  SectionMsg << (u8)Skill->GetSKPCost(SK_CST);
  SectionMsg << (u8)Skill->GetSubSkill(SK_RES);
  SectionMsg << (u8)Skill->GetSKPCost(SK_RES);
  SectionMsg << (u8)Skill->GetSubSkill(SK_IMP);
  SectionMsg << (u8)Skill->GetSKPCost(SK_IMP);
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)0x00; // spare
  SectionMsg << (u8)0x01; // cost
  SectionMsg << (u8)Skill->GetSubSkill(SK_PPU);
  SectionMsg << (u8)Skill->GetSKPCost(SK_PPU);
  SectionMsg << (u8)Skill->GetSubSkill(SK_APU);
  SectionMsg << (u8)Skill->GetSKPCost(SK_APU);
  SectionMsg << (u8)Skill->GetSubSkill(SK_MST);
  SectionMsg << (u8)Skill->GetSKPCost(SK_MST);
  SectionMsg << (u8)Skill->GetSubSkill(SK_PPW);
  SectionMsg << (u8)Skill->GetSKPCost(SK_PPW);
  SectionMsg << (u8)Skill->GetSubSkill(SK_PSR);
  SectionMsg << (u8)Skill->GetSKPCost(SK_PSR);
  SectionMsg << (u8)Skill->GetSubSkill(SK_WPW);
  SectionMsg << (u8)Skill->GetSKPCost(SK_WPW);

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 5 ----
  *BaselineMsg << (u8)0x05; // section id

  //SectionMsg << (u16)0x00;

  SectionMsg << (u16)0x01; // Backpack items nb  // section content at offset 3

  SectionMsg << (u16)0x07; // data size of item
  SectionMsg << (u8)0x00; // Spare on inventory
  SectionMsg << (u8)0x00; // pos X
  SectionMsg << (u8)0x00; // pos Y
  SectionMsg << (u16)0x0051; // item id (torch)
  SectionMsg << (u8)0x00;  // type
  SectionMsg << (u8)0x01; // Qty

  /*
  	//
  	//Section 5
  	//
  	StatsBuffer[len] = 0x05;
  	*(unsigned short*)&StatsBuffer[len+3] = 0; //Reset number of items
  	plen = 5;
  	for (i=0;i<MAX_INVENTORY;i++)
  	{
  		for (t=0;t<MAX_BACKPACK;t++)
  		{
  			if (CurrentChar.Inventory[t]-1 != i)
  				continue;

  			y=0;
  			while (y*10 < t)
  				y++;
  			y--;
  			x = t-(y*10);
  			*(unsigned short*)&StatsBuffer[len+plen] = 7;						//Data size of item
  			StatsBuffer[len+plen+2] = 0x00;
  			StatsBuffer[len+plen+3] = x;										//X position in Inventory
  			StatsBuffer[len+plen+4] = y;										//Y position in Inventory
  			*(unsigned short*)&StatsBuffer[len+plen+5] = CurrentChar.ItemList[CurrentChar.Inventory[t]-1].ItemID;	//Item ID
  			StatsBuffer[len+plen+7] = 0;										//Type
  			StatsBuffer[len+plen+8] = CurrentChar.ItemList[CurrentChar.Inventory[t]-1].Qty;			//Quantity
  			plen += 9;
  			*(unsigned short*)&StatsBuffer[len+3] += 1;							//Add to item
  			break;
  		}
  	}
  	*(unsigned short*)&StatsBuffer[len+1] = plen-3;			//Size
  	*/

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 6 ----
  *BaselineMsg << (u8)0x06; // section id

  SectionMsg << (u8)0x04; // QB/Armor/Implants items nb  // section content at offset 3

// THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== BEGIN
  SectionMsg << (u16)0x06;     // Size of item
  SectionMsg << (u16)0x00;     // Location: Quickbelt slot 0
  SectionMsg << (u16)0x0051;   // ItemID: 81, Flashlight
  SectionMsg << (u8)0x01;      // Datatype
  SectionMsg << (u8)0x00;      // Data

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

  nChar->GetInventory()->QB_SetSlot(0, 81); // Add Flashlight to QB slot 1
  nChar->GetInventory()->QB_SetSlot(1, 85); // Add Flashlight to QB slot 1
  nChar->GetInventory()->QB_SetSlot(2, 5999); // Add Flashlight to QB slot 1
// THIS IS A TEMP SOLUTION UNTIL WE HAVE ITEM STUFF WORKING ===== END
  /*
  	StatsBuffer[len+3] = 0;	//Number of items
  	plen = 4;
  	for (i=0;i<MAX_INVENTORY;i++)
  	{
  		for (t=0;t<128;t++)
  		{
  			if (CurrentChar.QuickBelt[t]-1 != i)
  				continue;
  			*(unsigned short*)&StatsBuffer[len+plen] = 6;				//Data size of item
  			*(unsigned short*)&StatsBuffer[len+plen+2] = t;		//X position in Inventory
  			*(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].ItemID;	//Item ID
  			*(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.ItemList[CurrentChar.QuickBelt[t]-1].Qty;			//Quantity
  			plen += 8;
  			*(unsigned short*)&StatsBuffer[len+3] += 1;						//Add to item
  			break;
  		}
  */
  		/*if (CurrentChar.Inventory[i].ItemID == 0)					//Last Item
  			break;
  		if (CurrentChar.Inventory[i].Location != 2)					//Not Quickbelt, skip it
  			continue;
  		if (Def_GetItemType(CurrentChar.Inventory[i].ItemID) == 1)
  		{
  			//18 00 01 00 08 00 63 00 06 ff c8 c8 c8 c8 ff 00 01 00 28 04 00 01 04 ff 01
  			//12 00 11    5f 07 23 00 06 eb a4 99 a3 a5 ff 04 00 01 04 ff 01
  			//13 00 05 00 03 02 23 00 06 6d c4 c4 c4 c4 ff 04 00 01 04 ff 00
  			//17 00 05    9d 01 73 28 06 13 c1 c1 c1 c1 ff 00 01 01 f1 05 04
  			//|Size|Slot|ItemID|???  |? |Du|    Stats  |MD|?????????????????|
  			*(unsigned short*)&StatsBuffer[len+plen] = 19;	//Size
  			*(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX;	//Slot
  			*(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID;//Item ID
  			StatsBuffer[len+plen+6] = 0x23;												//0x73 = Ranged, 0x63 = close/spell
  			StatsBuffer[len+plen+7] = CurrentChar.Inventory[i].Qty;						//Quantity
  			StatsBuffer[len+plen+8] = 0x06;												//0x06
  			StatsBuffer[len+plen+9] = CurrentChar.Inventory[i].CurDur;					//Current Durability
  			StatsBuffer[len+plen+10] = CurrentChar.Inventory[i].Damage;					//Stats
  			StatsBuffer[len+plen+11] = CurrentChar.Inventory[i].Freq;					//Stats
  			StatsBuffer[len+plen+12] = CurrentChar.Inventory[i].Hand;					//Stats
  			StatsBuffer[len+plen+13] = CurrentChar.Inventory[i].Rng;					//Stats
  			StatsBuffer[len+plen+14] = CurrentChar.Inventory[i].MaxDur;					//Max Durability
  			StatsBuffer[len+plen+15] = 0x04;											//Size or part
  			StatsBuffer[len+plen+16] = 0x00;											//follow or ?
  			StatsBuffer[len+plen+17] = 0x01;											//Ammo loaded?
  			StatsBuffer[len+plen+18] = 0x04;											//0x04 if not named, 0x0a if named
  			StatsBuffer[len+plen+19] = 0x05;											//0x06 if named, 0xff for no ammo needed, no idea otherwise
  			StatsBuffer[len+plen+20] = 0x0f;											//Binary representation of ammo allowed 0x01 = normal
  			plen+=21;
  		}
  		else
  		{
  			*(unsigned short*)&StatsBuffer[len+plen] = 6;									//Size of Item Data
  			*(unsigned short*)&StatsBuffer[len+plen+2] = CurrentChar.Inventory[i].LocX;		//Slot
  			*(unsigned short*)&StatsBuffer[len+plen+4] = CurrentChar.Inventory[i].ItemID;	//Item ID
  			*(unsigned short*)&StatsBuffer[len+plen+6] = CurrentChar.Inventory[i].Qty;		//Quantity
  			plen+= 8;
  		}
  		StatsBuffer[len+3] += 1;*/

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 7 ----
  *BaselineMsg << (u8)0x07; // section id

  SectionMsg << (u8)0x00; // ?? // section content at offset 3

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0c ----
  *BaselineMsg << (u8)0x0c; // section id

  //SectionMsg << (u8)0x00;

  SectionMsg << (u8)0x01; // Gogo items nb  // section content at offset 3

  SectionMsg << (u16)0x05; // data size of item
  SectionMsg << (u8)0x00; // pos (X) in gogo
  SectionMsg << (u16)0x0c6d; // item id (bullets 8mm explosive)
  SectionMsg << (u8)0x00;  // type
  SectionMsg << (u8)0x04; // Qty

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 8 ----
  *BaselineMsg << (u8)0x08; // section id

  u32 nSkin, nHead, nTorso, nLegs;
  nChar->GetRealLook(nSkin, nHead, nTorso, nLegs);

  SectionMsg << (u8)0x0a; // ? // section content at offset 3
  SectionMsg << (u32)nChar->GetCash();

  // Genrep list
  SectionMsg << (u16)nChar->GetGenrepCount(); //Number of GR tagged (Tinns value on u8 only)
  SectionMsg << (u8)0x04;
  if (nChar->GetGenrepCount())   // For each entry : (u16)genrep.g_worldid, (u16)genrep.g_stationid
  {
    SectionMsg.Write(nChar->GetGenrepListData(), nChar->GetGenrepListDataSize());
  }

  SectionMsg << (u8)0x04;
  SectionMsg << (u8)0x04;
  SectionMsg << (u8)0x00;

  // Direct chat
  if (nChar->GetDirectChat())
  {
    SectionMsg << (u8)1;
    SectionMsg << (u32)nChar->GetDirectChat();
  }
  else
  {
    SectionMsg << (u8)0;
  }

  // Buddy Chat
  SectionMsg << (u8)nChar->GetBuddyCount(); //Number of Buddies
  if (nChar->GetBuddyCount())   // For each buddy (u32)buddy CharID
  {
    SectionMsg.Write(nChar->GetBuddyListData(), nChar->GetBuddyListDataSize());
  }
  SectionMsg << (u8)0x00;

  SectionMsg << (u16)nClient->GetTransactionID(); // ??
  //SectionMsg << (u32)0x00000000;
  SectionMsg << (u32)0x00000000;
  SectionMsg << (u32)0x00000000; // Epic status ?
  SectionMsg << (u16)nSkin;
  SectionMsg << (u8)nHead;
  SectionMsg << (u8)nTorso;
  SectionMsg << (u8)nLegs;
  SectionMsg << (u8)0x00; // Rank
  SectionMsg << (u32)(nChar->GetBaseApartment() + 100000); // 0x22, 0x00, 0x00, 0x00, //Primary Apartment (GR activated) ???
  SectionMsg << (u8)0x01; // ?
  SectionMsg << (u8)0x00; // ?
  SectionMsg << (u8)0x00; // ?
  SectionMsg << (u8)0x00; // ?
  SectionMsg << (u8)0x00; // ?

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 9 ----
  *BaselineMsg << (u8)0x09; // section id

  SectionMsg << (u16)0x15; // Nb of factions // section content at offset 3
  SectionMsg << (u16)nChar->GetFaction();
  SectionMsg << (u8)0x04; // ?
  SectionMsg << (f32) 1000.0f*1000.0f; // 96.0f*9000.0f; // SL
  // Faction Sympathies Points effective sympathie is Square root of these points, abs value rounded down (beware of the sign ! :p)
  SectionMsg << (f32) 1000.0f*1250.0f; // City Admin => 111
  SectionMsg << (f32) 1000.0f*1000.0f; // Diamond => 100
  SectionMsg << (f32) 1000.0f*-750.0f; // Next => -86
  SectionMsg << (f32) 1000.0f*500.0f; // Tangent => 70
  SectionMsg << (f32) 1000.0f*-250.0f; // Biotech => -50
  SectionMsg << (f32) 1000.0f*0.0f; // ProtoPharm => 0
  SectionMsg << (f32) 1000.0f*1024.0f; // Trader's Union
  SectionMsg << (f32) 1000.0f*1024.0f; // Tsunami
  SectionMsg << (f32) 1000.0f*1024.0f; // Black Dragons
  SectionMsg << (f32) 1000.0f*1024.0f; // City Mercs
  SectionMsg << (f32) 1000.0f*1024.0f; // Crahn Sect
  SectionMsg << (f32) 1000.0f*1024.0f; // Dome Of York
  SectionMsg << (f32) 1000.0f*1024.0f; // Anarchy Breed
  SectionMsg << (f32) 1000.0f*1024.0f; // Fallen Angels
  SectionMsg << (f32) 1000.0f*1024.0f; // Twilight Guardian
  SectionMsg << (f32) 1000.0f*1024.0f; // Regeant's Legacy
  SectionMsg << (f32) 1000.0f*1024.0f; // Regeant's Mutants
  SectionMsg << (f32) 1000.0f*1024.0f; // Insects
  SectionMsg << (f32) 1000.0f*1024.0f; // Monsters
  SectionMsg << (f32) 1000.0f*1024.0f; // Unknown
  SectionMsg << (f32) 1000.0f*1024.0f; // Highest SL?

  SectionMsg << (u32)0x00000000; // Epic done Data : bit 2^FactionID set <=> Epic done
  SectionMsg << (u8)nChar->GetFaction(); // Faction ??? wrong size ...

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0a ----
  *BaselineMsg << (u8)0x0a; // section id

  // Clan data ?
  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0b ----
  *BaselineMsg << (u8)0x0b; // section id

  SectionMsg << (u8)0x00; // ?? // section content at offset 3

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  // ---- Section 0d ----
  *BaselineMsg << (u8)0x0d; // section id

  SectionMsg << (u8)0xfa; // ?? // section content at offset 3
  SectionMsg << (u8)nChar->GetProfession();
  SectionMsg << (u16)nClient->GetTransactionID(); // ?? TransactionID ? 0x8aa0
  SectionMsg << (u32)nChar->GetID();

  *BaselineMsg << (u16)SectionMsg.GetSize();
  *BaselineMsg << SectionMsg;
  SectionMsg.Clear();

  return BaselineMsg;
}

PMessage* PMsgBuilder::BuildCharInfo3Msg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(22);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x0d;
	*tmpMsg << (u32)GameServer->GetGameTime(); // offset 10 (NeoX) or 11 ????

	*tmpMsg << (u8)0x47; // ???
	*tmpMsg << (u8)0xc0;
	*tmpMsg << (u8)0x22;
	*tmpMsg << (u8)0x00;

	*tmpMsg << (u8)0xe5; // ???
	*tmpMsg << (u8)0x0a;
	*tmpMsg << (u8)0xbb;
	*tmpMsg << (u8)0x00;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning1Msg (PClient* nClient, u16 nData)
{
  PMessage* tmpMsg = new PMessage(42);

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID(); // from NeoX

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0c; // Message length place;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID(); // from NeoX
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x13; // ??
	*tmpMsg << (u16)nClient->GetTransactionID(); // from NeoX / ?? right ???
	*tmpMsg << (u8)0x0e; // ?? from NeoX
	*tmpMsg << (u8)0x02; // ?? from NeoX

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x17; // Message length place;
	*tmpMsg << (u8)0x03; // from NeoX
  *tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x23; // from NeoX
	*tmpMsg << (u8)0x04; // from NeoX
	*tmpMsg << (u32)0x00000000; // from NeoX
	*tmpMsg << (u32)0x00000000; // from NeoX
	*tmpMsg << (u16)0x0000; // from NeoX
	*tmpMsg << (u16)nData; // from NeoX
	*tmpMsg << (u16)0x0000; // from NeoX
	*tmpMsg << (u16)nClient->GetTransactionID(); // from NeoX
	*tmpMsg << (u16)0x0000; // from NeoX

	//tmpMsg->U16Data(1)=nClient->GetUDP_ID(); // from NeoX... seems to work without
	//tmpMsg->U16Data(3)=nClient->GetSessionID(); // from NeoX

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoningTCPReadyMsg ()
{
  PMessage* tmpMsg = new PMessage(7);

//static const u8 READY[7] = {0xfe, 0x04, 0x00, 0x83, 0x0d, 0x00, 0x00};
	*tmpMsg << (u8)0xfe;
	*tmpMsg << (u16)0x0004;
	*tmpMsg << (u8)0x83;
	*tmpMsg << (u8)0x0d;
	*tmpMsg << (u16)0x0000;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildZoning2Msg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(22);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x0d;
	*tmpMsg << (u32)GameServer->GetGameTime(); // offset 10 (NeoX) or 11 ????

	*tmpMsg << (u8)0x47; // ???
	*tmpMsg << (u8)0xc0;
	*tmpMsg << (u8)0x22;
	*tmpMsg << (u8)0x00;

	*tmpMsg << (u8)0xe5; // ???
	*tmpMsg << (u8)0x0a;
	*tmpMsg << (u8)0xbb;
	*tmpMsg << (u8)0x00;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepZoningMsg (PClient* nClient, u32 nLocation, u16 nEntity)
{
  PMessage* tmpMsg = new PMessage(50);

  nClient->IncreaseUDP_ID();

  tmpMsg->Fill(0);
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x23;
	*tmpMsg << (u8)0x0c;
	tmpMsg->SetNextByteOffset(38);
	*tmpMsg << (u32)0xffffffff;
	*tmpMsg << (u32)nLocation;
  *tmpMsg << (u16)nEntity;
  *tmpMsg << (u16)0x0000;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepAddToListMsg (PClient* nClient, u32 nLocation, u16 nEntity)
{
  PMessage* tmpMsg = new PMessage(23);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x3d;
  *tmpMsg << (u8)0x02;
	*tmpMsg << (u16)0x0000;
	*tmpMsg << (u8)0x00;
	*tmpMsg << (u32)nLocation;
	*tmpMsg << (u16)nEntity;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildGenrepDenyBrokenMsg (PClient* nClient)
{
    PMessage* tmpMsg = new PMessage(18);

    nClient->IncreaseUDP_ID();
	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0c; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x31; // ??
	*tmpMsg << (u8)0x3c; // ??
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharEnteringVhcMsg (PClient* nClient, u16 nVehicleID, u8 nVehicleSeat)
{
  PMessage* tmpMsg = new PMessage(18);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // nClient->GetUDP_ID() placeholder
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x21;
	*tmpMsg << (u16)nVehicleID;
	*tmpMsg << (u16)0x0000;
	*tmpMsg << (u8)nVehicleSeat;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftUseMsg (PClient* nClient, u32 nLocation, u16 nEntity)
{
  PMessage* tmpMsg = new PMessage(43);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x0f; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x38;
  *tmpMsg << (u8)0x04; // Accepted (?)
  *tmpMsg << (u8)0x00; // Sewer Level
	*tmpMsg << (u32)nLocation;
	*tmpMsg << (u16)nEntity;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

	u8 msgPart2[] = { // What is really this part for ???
	  0x1b, 0x22, 0x01, 0x00, 0x00, 0x1f, 0x49, 0x82, 0x81, 0x81,
		0xe5, 0x6b, 0x04, 0xd5, 0x76, 0x01, 0x00, 0x00, 0x00, 0x11,
		0x11};
	*tmpMsg << (u8)sizeof(msgPart2); // (0x15) Message length placeholder;
	tmpMsg->Write(msgPart2, sizeof(msgPart2));

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildAptLiftFailedMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(14);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x08; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x38;
  *tmpMsg << (u8)0x03; // Refused

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildChangeLocationMsg (PClient* nClient, u32 nLocation, u16 nEntity, u8 nLevel, u32 nRawItemID)
{
  PMessage* tmpMsg = new PMessage(28);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	if (nRawItemID)
	{
	  *tmpMsg << (u8)0x06; // Sub message length;
    *tmpMsg << (u8)0x2d; // Item use response;
    *tmpMsg << (u32)nRawItemID;
    *tmpMsg << (u8)0x0a; // Use allowed
	}

	*tmpMsg << (u8)0x0f; // Sub message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x38;
  *tmpMsg << (u8)0x04; // Accepted (?)
  *tmpMsg << (u8)nLevel;
	*tmpMsg << (u32)nLocation;
	*tmpMsg << (u16)nEntity;

  //nClient->IncreaseUDP_ID(); // Why PostIncrement here??

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharAptLocInfoMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(21);
  nClient->IncreaseUDP_ID();

  int BaseAppId = nClient->GetChar()->GetBaseApartment();
  u32 AptLocation = (u32)MySQL->GetAptLocation(BaseAppId);

  *tmpMsg << (u8)0x13;
  *tmpMsg << (u16)nClient->GetUDP_ID();
  *tmpMsg << (u16)nClient->GetSessionID();
  *tmpMsg << (u8)0x0f;
  *tmpMsg << (u8)0x03;
  *tmpMsg << (u16)nClient->GetUDP_ID();
  *tmpMsg << (u8)0x1f;
  *tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x3d;
  *tmpMsg << (u8)0x0b;
  *tmpMsg << (u8)0x00;
  *tmpMsg << (u8)0x00;
  *tmpMsg << (u8)0x00;
  *tmpMsg << (u32)AptLocation;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildSubskillIncMsg (PClient* nClient, u8 nSubskill, u16 nSkillPoints)
{
  /*** Doesn't work for NC1 ??? ***/

  PMessage* tmpMsg = new PMessage(33);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

  nClient->IncreaseUDP_ID();
	*tmpMsg << (u8)0x09; // SubMessage length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x25;
  *tmpMsg << (u8)0x23;
  *tmpMsg << (u8)0x41; // 0x28 ??

  nClient->IncreaseUDP_ID();
  nClient->IncreaseTransactionID(); // testing ...
	*tmpMsg << (u8)0x11; // SubMessage length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x25;
  *tmpMsg << (u8)0x13;
  *tmpMsg << (u16)nClient->GetTransactionID(); // testing / 0x0000 ????
  *tmpMsg << (u8)0x09; // ?
  *tmpMsg << (u16)nSubskill;
  *tmpMsg << (u16)nClient->GetChar()->Skill->GetSubSkill(nSubskill); // nSubskill ?
	*tmpMsg << (u16)nSkillPoints;

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildChatAddMsg (PClient* nClient, u32 nAddedCharID, u8 nMode)
{
  PMessage* tmpMsg = new PMessage(18);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x00; // Message length placeholder;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x32;
  *tmpMsg << (u8)nMode;
	*tmpMsg << (u32)nAddedCharID;

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildDoorOpenMsg (u32 nRawItemID, bool nDoubleDoor)
{
  //PMessage* tmpMsg = new PMessage(37);
  PMessage* tmpMsg = new PMessage(21);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; //Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u16)0x0000;  // Client->GetSessionID(); // just placeholder, must be set outside

	*tmpMsg << (u8)0x0f; // Sub-message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; //++Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x1b;
	*tmpMsg << (u32)nRawItemID;
	*tmpMsg << (u8)0x20; //?
	if (nDoubleDoor)
	{
  	*tmpMsg << (u16)0x0005; //?
  	*tmpMsg << (u16)0x0000; //?
  	*tmpMsg << (u16)0x1500; //?
	}
	else
	{
  	*tmpMsg << (u16)0x0000; //?
  	*tmpMsg << (u16)0x00c8; //?
  	*tmpMsg << (u16)0x10ff; //?
  }

  (*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

/*	*tmpMsg << (u8)0x0f; // Sub-message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; //++Client->GetUDP_ID(); // just placeholder, must be set outside
	*tmpMsg << (u8)0x1b;
	*tmpMsg << (u32)nRawItemID;
	*tmpMsg << (u8)0x20; //?
	if (nDoubleDoor)
	{
  	*tmpMsg << (u16)0x0005; //?
  	*tmpMsg << (u16)0x0000; //?
  	*tmpMsg << (u16)0x1500; //?
	}
	else
	{
  	*tmpMsg << (u16)0x0000; //?
  	*tmpMsg << (u16)0x00c8; //?
  	*tmpMsg << (u16)0x10ff; //?
  }*/

/*
  case 3: // Single Locked
  AptDoorUse[22] = 0x00;
  AptDoorUse[24] = 0x64;
  AptDoorUse[26] = 0xFF;
  AptDoorUse[27] = 0x10;
  break;
  case 4: // Double Locked
  AptDoorUse[22] = 0x05;
  AptDoorUse[24] = 0x00;
  AptDoorUse[26] = 0x00;
  AptDoorUse[27] = 0x15;
  break;
  default:
  AptDoorUse[22] = 0x00;
  AptDoorUse[24] = 0x64;
  AptDoorUse[26] = 0xFF;
  AptDoorUse[27] = 0x10;
  break;
*/
  return tmpMsg;
}

// Message from text.ini, section [MISC], id = 100+nTxtMsgId
PMessage* PMsgBuilder::BuildText100Msg (PClient* nClient, u8 nTxtMsgId, u32 nRawObjectID)
{
    PMessage* tmpMsg = new PMessage(17);

    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u16)nClient->GetSessionID();

    *tmpMsg << (u8)0x0c; // Message length;
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u16)nClient->GetUDP_ID();
    *tmpMsg << (u8)0x1f;
    *tmpMsg << (u16)nClient->GetLocalID();
    *tmpMsg << (u8)0x31;
    *tmpMsg << (u8)nTxtMsgId;
    *tmpMsg << (u32)nRawObjectID;

    //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

    return tmpMsg;
}

// Same as BuildText100Msg, but here we can specify *ANY* text from text.ini, not limited to section [MISC]
PMessage* PMsgBuilder::BuildTextIniMsg (PClient* nClient, u8 nTxtGroupID, u16 nTxtID)
{
    PMessage* tmpMsg = new PMessage(20);

    nClient->IncreaseUDP_ID();
    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0e; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x15; // ??
	*tmpMsg << nTxtGroupID;
	*tmpMsg << nTxtID;
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildFurnitureActivateMsg (PClient* nClient, u32 nRawObjectID, u8 nActionValue)
{
  PMessage* tmpMsg = new PMessage(12);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x06; // SubMessage length;
	*tmpMsg << (u8)0x2d;
	*tmpMsg << (u32)nRawObjectID;
	*tmpMsg << (u8)nActionValue; // known valid are 5 (ring), 9 (remove) and 10 (clic)

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseFurnitureMsg (PClient* nClient, u32 nRawObjectID)
{
  PMessage* tmpMsg = new PMessage(24);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x06; // SubMessage length;
	*tmpMsg << (u8)0x2d;
	*tmpMsg << (u32)nRawObjectID;
	*tmpMsg << (u8)0x0a;

	*tmpMsg << (u8)0x0b; // SubMessage length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x17;
	*tmpMsg << (u32)nRawObjectID;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseGogoMsg (PClient* nClient)
{
  PMessage* tmpMsg = new PMessage(17);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x0b; // Message length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x3d;
	*tmpMsg << (u32)0x0000000d; // ?

  //(*tmpMsg)[5] = (u8)(tmpMsg->GetSize() - 6);

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseGenrepMsg (PClient* nClient, u32 nRawObjectID, u32 nLocation, u16 nEntity)
{
  PMessage* tmpMsg = new PMessage(24);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x06; // SubMessage length;
	*tmpMsg << (u8)0x2d;
	*tmpMsg << (u32)nRawObjectID;
	*tmpMsg << (u8)0x0a;

	// this submessage is only needed to set to location/entity ofthe GR for a potential record in the char's GR list
	*tmpMsg << (u8)0x0d; // SubMessage length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x2d;
	*tmpMsg << (u32)nLocation;
	*tmpMsg << (u16)nEntity;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseLiftMsg (PClient* nClient, u32 nRawObjectID, u16 nAptPlace)
{
  PMessage* tmpMsg = new PMessage(29);

  nClient->IncreaseUDP_ID();

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();

	*tmpMsg << (u8)0x06; // SubMessage length;
	*tmpMsg << (u8)0x2d;
	*tmpMsg << (u32)nRawObjectID;
	*tmpMsg << (u8)0x0a;

	*tmpMsg << (u8)0x11; // SubMessage length;
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
  *tmpMsg << (u8)0x38;
  *tmpMsg << (u8)0x01;
	*tmpMsg << (u32)nRawObjectID;
	*tmpMsg << (u16)nAptPlace; // Entity ???
	*tmpMsg << (u16)0x0000;
	*tmpMsg << (u8)0x00;

  return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharVanishMsg (PClient* nClient)
{
    PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // UDP ID placeholder
	*tmpMsg << (u16)0x0000; // SessionID placeholder
	*tmpMsg << (u8)0x08;    // Len (static, always 0x08
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // Sub UDP ID placeholder
	*tmpMsg << (u8)0x26;    // Command FADE AWAY CHAR (kinda ^^)
	*tmpMsg << (u16)nClient->GetLocalID();
    *tmpMsg << (u16)0x0000;   // No idea yet...

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharShowGlowCircleMsg (PClient* nClient)
{
    PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // UDP ID placeholder
	*tmpMsg << (u16)0x0000; // SessionID placeholder
	*tmpMsg << (u8)0x08;    // Len (static, always 0x08
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // Sub UDP ID placeholder
	*tmpMsg << (u8)0x1F;    // Command SHOW GLOWING CIRCLE (kinda ^^)
	*tmpMsg << (u16)nClient->GetLocalID();
    *tmpMsg << (u16)0x013C;   // No idea yet...

    return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharMoneyUpdateMsg (PClient* nClient, u32 nCredits)
{
    PMessage* tmpMsg = new PMessage(21);
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0f; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x13; // ??
    *tmpMsg << (u16)nClient->GetTransactionID();
	*tmpMsg << (u8)0x04; // ??
	*tmpMsg << nCredits;

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg1 (PClient* nClient, u8 nValue)
{
    PMessage* tmpMsg = new PMessage(15);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x09; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x23; // ??
	*tmpMsg << nValue; // ??

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg2 (PClient* nClient, u16 nV1, u16 nV2, u16 nV3, u16 nV4, u16 nV5, u16 nV6, u16 nV7)
{
    PMessage* tmpMsg = new PMessage(28);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x16; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x22; // ??
	*tmpMsg << nV1;
	*tmpMsg << nV2;
	*tmpMsg << nV3;
	*tmpMsg << nV4;
	*tmpMsg << nV5;
	*tmpMsg << nV6;
	*tmpMsg << nV7;
/*	*tmpMsg << (u8)0x64; // ??
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

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg3 (PClient* nClient, u8 nSlot)
{
    PMessage* tmpMsg = new PMessage(19);
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0D; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25; // ??
	*tmpMsg << (u8)0x13; // ??
    *tmpMsg << (u16)nClient->GetTransactionID();
	*tmpMsg << (u8)0x0B; // ??
	*tmpMsg << nSlot; // ??
	*tmpMsg << (u8)0x00; // ??

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharUseQBSlotMsg4 (PClient* nClient, u16 nValue1)
{
    PMessage* tmpMsg = new PMessage(16);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x0a; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x2f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x02; // ??
	*tmpMsg << (u8)0x02; // ??
    *tmpMsg << nValue1;

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildCharOpenContainerMsg (PClient* nClient, u32 nContainerID)
{
// "Header"
// 13 7f 00 d7 b5 0f 03 7f 00 1f 01 00 26 00 70 01 00 00 64 00 00
// Nc  UDP  SESS  Ln Cs  UDP  Cs ChrID Cs |.Cont.ID.| |.Cont.| 0x00: Empty 0x08: Filled

// Item SubHeader
// 03 00
// Len

// Item Data
// u8   - bytes to follow for THIS item
// u16  - ItemID (items.def)
// u8   - DataType

    PMessage* tmpMsg = new PMessage(24);
    nClient->IncreaseUDP_ID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x14; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x26; // ??
	*tmpMsg << nContainerID;
    *tmpMsg << (u8)0x00; // Always the same on item containers?
    *tmpMsg << (u8)0x64; // Always the same on item containers?
    *tmpMsg << (u8)0x00; // Always the same on item containers?
    *tmpMsg << (u8)0x08; // 0x08 when container is filled, 0x00 when not? At least it works..
    *tmpMsg << (u8)0x03;
    *tmpMsg << (u8)0x00;
    *tmpMsg << (u8)0x02;
    *tmpMsg << (u8)0x06;
    *tmpMsg << (u8)0x29;

	return tmpMsg;
}

PMessage* PMsgBuilder::BuildItemMoveMsg (PClient* nClient, u8 nSource, u8 nSrcX, u8 nSrcY, u8 nDestination, u8 nDestX, u8 nDestY, u8 nItemCnt)
{
// 13 01 01 C6 E3 14 03 01 01 1F 01 00 25 13 BE F8 14 02 06 00 03 03 00 01 00 00
    PMessage* tmpMsg = new PMessage(24);
    nClient->IncreaseUDP_ID();
    nClient->IncreaseTransactionID();

    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u16)nClient->GetSessionID();
	*tmpMsg << (u8)0x14; // Message length
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)nClient->GetUDP_ID();
	*tmpMsg << (u8)0x1f;
	*tmpMsg << (u16)nClient->GetLocalID();
	*tmpMsg << (u8)0x25;
	*tmpMsg << (u8)0x13;
    *tmpMsg << (u16)nClient->GetTransactionID();
	*tmpMsg << (u8)0x14; // ItemMove Answer
	*tmpMsg << nSource;
	*tmpMsg << nSrcX;
	*tmpMsg << nSrcY;
	*tmpMsg << nDestination;
	*tmpMsg << nDestX;
	*tmpMsg << nDestY;
	*tmpMsg << nItemCnt;
	*tmpMsg << (u8)0x00; // ??
	*tmpMsg << (u8)0x00; // ??

	return tmpMsg;
}

/*PMessage* PMsgBuilder::BuildRemoveWorldObjectMsg (u32 nWOID)
{
    PMessage* tmpMsg = new PMessage(14);

	*tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // UDP ID placeholder
	*tmpMsg << (u16)0x0000; // SessionID placeholder
	*tmpMsg << (u8)0x08;    // Len (static, always 0x08
	*tmpMsg << (u8)0x03;
	*tmpMsg << (u16)0x0000; // Sub UDP ID placeholder
	*tmpMsg << (u8)0x26;    // Command FADE AWAY CHAR (kinda ^^)
    *tmpMsg << (u32)nWOID;  // WorldobjectID

    return tmpMsg;
}*/

/*PMessage* PMsgBuilder::BuiltSpawnObjectMsg (u16 nActorID, u16 nFunctionID, u32 nWOID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotX)
{
    PMessage* tmpMsg = new PMessage(29);
    *tmpMsg << (u8)0x13;
	*tmpMsg << (u16)0x0000; // UDP placeholder
	*tmpMsg << (u16)0x0000; // Session placeholder
	*tmpMsg << (u8)0x16;    // Message length
	*tmpMsg << (u8)0x03;    // 0x03 commandset
	*tmpMsg << (u16)0x0000; // UDP placeholder
	*tmpMsg << (u8)0x1b;    // Subcommandset
    *tmpMsg << nWOID;  // WorldobjectID

	*tmpMsg << (u8)0x19;    // Positiondata follows
	*tmpMsg << nPosY;
	*tmpMsg << nPosZ;
	*tmpMsg << nPosX;

	*tmpMsg << nRotY;    // Rotation X
	*tmpMsg << nRotZ;    // Rotation Y
	*tmpMsg << nRotX;    // Rotation Z
	*tmpMsg << nActorID;
	*tmpMsg << nFunctionID;

	return tmpMsg;
}*/
