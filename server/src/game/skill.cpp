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
	skill.cpp

	Authors:
	- Namikon

	MODIFIED: 18 Dec 2005 Namikon
	REASON: - Initial Release
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Initial Release
            - Added SetMainSkill()
            - Added SetSubSkill()
            - Added SetXP()
            - Added SetSP()
            - Removed IncreaseSubSkill and IncreaseMainSkill

*/
#include "main.h"

void PSkillHandler::SetMainSkill(MAIN_SKILLS Skill, int value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTR = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEX = value;
        break;

    case MS_CON:
        m_Skills.Main->mCON = value;
        break;

    case MS_INT:
        m_Skills.Main->mINT = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSI = value;
        break;
    };
}

void PSkillHandler::SetSubSkill(SUB_SKILLS Skill, int value)
{
    switch(Skill)
    {
// STR Skills ------------------------
    case SK_MC:
        m_Skills.STR->mMC = value;
        break;

    case SK_HC:
        m_Skills.STR->mHC = value;
        break;

    case SK_TRA:
        m_Skills.STR->mTRA = value;
        break;

    case SK_FOR:
        m_Skills.STR->mFOR = value;
        break;
// DEX Skills ------------------------
    case SK_PC:
        m_Skills.DEX->mPC = value;
        break;

    case SK_RC:
        m_Skills.DEX->mRC = value;
        break;

    case SK_TC:
        m_Skills.DEX->mTC = value;
        break;

    case SK_VHC:
        m_Skills.DEX->mVHC = value;
        break;

    case SK_AGL:
        m_Skills.DEX->mAGL = value;
        break;

    case SK_REP:
        m_Skills.DEX->mREP = value;
        break;

    case SK_REC:
        m_Skills.DEX->mREC = value;
        break;

    case SK_RCL:
        m_Skills.DEX->mRCL = value;
        break;
// CON Skills ------------------------
    case SK_ATL:
        m_Skills.CON->mATL = value;
        break;

    case SK_END:
        m_Skills.CON->mEND = value;
        break;

    case SK_FIR:
        m_Skills.CON->mFIR = value;
        break;

    case SK_ENR:
        m_Skills.CON->mENR = value;
        break;

    case SK_XRR:
        m_Skills.CON->mXRR = value;
        break;

    case SK_POR:
        m_Skills.CON->mPOR = value;
        break;

    case SK_HLT:
        m_Skills.CON->mHLT = value;
        break;
// INT Skills ------------------------
    case SK_HCK:
        m_Skills.INT->mHCK = value;
        break;

    case SK_BRT:
        m_Skills.INT->mBRT = value;
        break;

    case SK_PSU:
        m_Skills.INT->mPSU = value;
        break;

    case SK_WEP:
        m_Skills.INT->mWEP = value;
        break;

    case SK_CST:
        m_Skills.INT->mCST = value;
        break;

    case SK_RES:
        m_Skills.INT->mRES = value;
        break;

    case SK_IMP:
        m_Skills.INT->mIMP = value;
        break;

    case SK_WPW:
        m_Skills.INT->mWPW = value;
        break;
// PSI Skills ------------------------
    case SK_PPU:
        m_Skills.PSI->mPPU = value;
        break;

    case SK_APU:
        m_Skills.PSI->mAPU = value;
        break;

    case SK_MST:
        m_Skills.PSI->mMST = value;
        break;

    case SK_PPW:
        m_Skills.PSI->mPPW = value;
        break;

    case SK_PSR:
        m_Skills.PSI->mPSR = value;
        break;
    };
}

void PSkillHandler::SetXP(MAIN_SKILLS Skill, float value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTRxp = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEXxp = value;
        break;

    case MS_CON:
        m_Skills.Main->mCONxp = value;
        break;

    case MS_INT:
        m_Skills.Main->mINTxp = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSIxp = value;
        break;
    };
}

void PSkillHandler::SetSP(MAIN_SKILLS Skill, short value)
{
    switch (Skill)
    {
    case MS_STR:
        m_Skills.Main->mSTRsp = value;
        break;

    case MS_DEX:
        m_Skills.Main->mDEXsp = value;
        break;

    case MS_CON:
        m_Skills.Main->mCONsp = value;
        break;

    case MS_INT:
        m_Skills.Main->mINTsp = value;
        break;

    case MS_PSI:
        m_Skills.Main->mPSIsp = value;
        break;
    };
}

int PSkillHandler::GetMainSkill(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTR;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEX;
        break;

    case MS_CON:
        return m_Skills.Main->mCON;
        break;

    case MS_INT:
        return m_Skills.Main->mINT;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSI;
        break;
    }
    return 0;
}

int PSkillHandler::GetSubSkill(SUB_SKILLS Skill)
{
    switch(Skill)
    {
// STR Skills ------------------------
    case SK_MC:
        return m_Skills.STR->mMC;
        break;

    case SK_HC:
        return m_Skills.STR->mHC;
        break;

    case SK_TRA:
        return m_Skills.STR->mTRA;
        break;

    case SK_FOR:
        return m_Skills.STR->mFOR;
        break;
// DEX Skills ------------------------
    case SK_PC:
        return m_Skills.DEX->mPC;
        break;

    case SK_RC:
        return m_Skills.DEX->mRC;
        break;

    case SK_TC:
        return m_Skills.DEX->mTC;
        break;

    case SK_VHC:
        return m_Skills.DEX->mVHC;
        break;

    case SK_AGL:
        return m_Skills.DEX->mAGL;
        break;

    case SK_REP:
        return m_Skills.DEX->mREP;
        break;

    case SK_REC:
        return m_Skills.DEX->mREC;
        break;

    case SK_RCL:
        return m_Skills.DEX->mRCL;
        break;
// CON Skills ------------------------
    case SK_ATL:
        return m_Skills.CON->mATL;
        break;

    case SK_END:
        return m_Skills.CON->mEND;
        break;

    case SK_FIR:
        return m_Skills.CON->mFIR;
        break;

    case SK_ENR:
        return m_Skills.CON->mENR;
        break;

    case SK_XRR:
        return m_Skills.CON->mXRR;
        break;

    case SK_POR:
        return m_Skills.CON->mPOR;
        break;

    case SK_HLT:
        return m_Skills.CON->mHLT;
        break;
// INT Skills ------------------------
    case SK_HCK:
        return m_Skills.INT->mHCK;
        break;

    case SK_BRT:
        return m_Skills.INT->mBRT;
        break;

    case SK_PSU:
        return m_Skills.INT->mPSU;
        break;

    case SK_WEP:
        return m_Skills.INT->mWEP;
        break;

    case SK_CST:
        return m_Skills.INT->mCST;
        break;

    case SK_RES:
        return m_Skills.INT->mRES;
        break;

    case SK_IMP:
        return m_Skills.INT->mIMP;
        break;

    case SK_WPW:
        return m_Skills.INT->mWPW;
        break;
// PSI Skills ------------------------
    case SK_PPU:
        return m_Skills.PSI->mPPU;
        break;

    case SK_APU:
        return m_Skills.PSI->mAPU;
        break;

    case SK_MST:
        return m_Skills.PSI->mMST;
        break;

    case SK_PPW:
        return m_Skills.PSI->mPPW;
        break;

    case SK_PSR:
        return m_Skills.PSI->mPSR;
        break;
    };
    return 0;
}

int PSkillHandler::GetSKPCost(SUB_SKILLS Skill)
{
    int skillvalue = GetSubSkill(Skill);

    if(skillvalue < 50)
    {
        return 1;
    }
    else if(skillvalue > 49 && skillvalue < 75)
    {
        return 2;
    }
    else if(skillvalue > 74 && skillvalue < 100)
    {
        return 3;
    }
    else if(skillvalue > 99)
    {
        return 5;
    }
    else
    {
        return 1;
    }
}

unsigned short PSkillHandler::GetSP(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTRsp;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEXsp;
        break;

    case MS_CON:
        return m_Skills.Main->mCONsp;
        break;

    case MS_INT:
        return m_Skills.Main->mINTsp;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSIsp;
        break;
    };
    return 0;
}

float PSkillHandler::GetXP(MAIN_SKILLS Skill)
{
    switch (Skill)
    {
    case MS_STR:
        return m_Skills.Main->mSTRxp;
        break;

    case MS_DEX:
        return m_Skills.Main->mDEXxp;
        break;

    case MS_CON:
        return m_Skills.Main->mCONxp;
        break;

    case MS_INT:
        return m_Skills.Main->mINTxp;
        break;

    case MS_PSI:
        return m_Skills.Main->mPSIxp;
        break;
    };
    return 0;
}
