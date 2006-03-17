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
	skill.h

	Authors:
	- Namikon
	- bakkdoor

	MODIFIED: 17 Dec 2005 Namikon
	REASON: - Initial Release
	MODIFIED: 17 Dec 2005 bakkdoor
	REASON: - Fixed some typos and small bugs
	MODIFIED: 18 Dec 2005 Namikon
	REASON: - Moved Structs to new file, added vars for XP and SkillPoints
                - Added initial skillhandling
	MODIFIED: 01 Jan 2006 Namikon
	REASON: - Added SetMainSkill()
            - Added SetSubSkill()
            - Added SetXP()
            - Added SetSP()
            - Removed IncreaseSubSkill and IncreaseMainSkill
            - Fixed typo in SUB_SKILLS enum

*/

#ifndef SKILL_H
#define SKILL_H

enum SUB_SKILLS
{
    SK_MC = 1,
    SK_HC = 2,
    SK_TRA = 3,
    SK_FOR = 22,
    SK_PC = 10,
    SK_RC = 11,
    SK_TC = 12,
    SK_VHC = 13,
    SK_AGL = 14,
    SK_REP = 15,
    SK_REC = 16,
    SK_RCL = 17,
    SK_ATL = 20,
    SK_END = 21,
    SK_FIR = 23,
    SK_ENR = 24,
    SK_XRR = 25,
    SK_POR = 26,
    SK_HLT = 27,
    SK_HCK = 30,
    SK_BRT = 31,
    SK_PSU = 32,
    SK_WEP = 33,
    SK_CST = 34,
    SK_RES = 35,
    SK_IMP = 36,
    SK_PPU = 40,
    SK_APU = 41,
    SK_MST = 42,
    SK_PPW = 43,
    SK_PSR = 44,
    SK_WPW = 45
};

enum MAIN_SKILLS
{
    MS_STR = 1,
    MS_DEX = 2,
    MS_CON = 3,
    MS_INT = 4,
    MS_PSI = 5
};

class PSkillHandler
{
    private:
    struct PMainSkills
    {
        int mINT;
        int mCON;
        int mSTR;
        int mDEX;
        int mPSI;
        unsigned short mINTsp;
        unsigned short mCONsp;
        unsigned short mSTRsp;
        unsigned short mDEXsp;
        unsigned short mPSIsp;
        float mINTxp;
        float mCONxp;
        float mSTRxp;
        float mDEXxp;
        float mPSIxp;

        inline PMainSkills()
        {
            mINT = mCON = mSTR = mDEX = mPSI = 0;
            mINTsp = mINTsp = mINTsp = mINTsp = mINTsp = 0;
            mINTxp = mINTxp = mINTxp = mINTxp = mINTxp = 0.f;
        }
    };

    struct PSubSkillINT
    {
        int mHCK;
        int mBRT;
        int mPSU;
        int mWEP;
        int mCST;
        int mRES;
        int mIMP;
        int mWPW;

        inline PSubSkillINT()
        {
            mHCK = mBRT = mPSU = mWEP = mCST = mRES = mIMP = mWPW = 0;
        }
    };

    struct PSubSkillCON
    {
        int mATL;
        int mEND;
        int mFIR;
        int mENR;
        int mXRR;
        int mPOR;
        int mHLT;

        inline PSubSkillCON()
        {
            mATL = mEND = mFIR = mENR = mXRR = mPOR = mHLT = 0;
        }
    };

    struct PSubSkillSTR
    {
        int mMC;
        int mHC;
        int mTRA;
        int mFOR;

        inline PSubSkillSTR()
        {
            mMC = mHC = mTRA = mFOR = 0;
        }
    };

    struct PSubSkillDEX
    {
        int mPC;
        int mRC;
        int mTC;
        int mVHC;
        int mAGL;
        int mREP;
        int mREC;
        int mRCL;

        inline PSubSkillDEX()
        {
            mPC = mRC = mTC = mVHC = mAGL = mREP = mREC = mRCL = 0;
        }
    };

    struct PSubSkillPSI
    {
        int mPPU;
        int mAPU;
        int mMST;
        int mPPW;
        int mPSR;

        inline PSubSkillPSI()
        {
            mPPU = mAPU = mMST = mPPW = mPSR = 0;
        }
    };

    struct PSkills
    {
        PMainSkills *Main;
        PSubSkillINT *INT;
        PSubSkillCON *CON;
        PSubSkillSTR *STR;
        PSubSkillDEX *DEX;
        PSubSkillPSI *PSI;

        inline PSkills()
        {
            Main = new PMainSkills();
            INT = new PSubSkillINT();
            CON = new PSubSkillCON();
            STR = new PSubSkillSTR();
            DEX = new PSubSkillDEX();
            PSI = new PSubSkillPSI();
        }
    };

    PSkills m_Skills;

    public:

    void SetSubSkill(SUB_SKILLS Skill, int value);
    void SetMainSkill(MAIN_SKILLS Skill, int value);
    void SetXP(MAIN_SKILLS Skill, float value);
    void SetSP(MAIN_SKILLS Skill, short value);

    int GetMainSkill(MAIN_SKILLS Skill);
    int GetSubSkill(SUB_SKILLS Skill);
    int GetSKPCost(SUB_SKILLS Skill);

    unsigned short GetSP(MAIN_SKILLS Skill);
    float GetXP(MAIN_SKILLS Skill);
};

#endif
