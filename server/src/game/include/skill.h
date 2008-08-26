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
	MODIFIED: 19 Sep 2006 Hammag
	REASON: - Added destructor for struct PSkills in order to free allocated memory
	            and avoid memory leak.
	MODIFIED: 25 Nov 2006 Hammag
	REASON: - Changed from enums (MAIN_SKILLS & SUB_SKILLS) to define, because there is no
	          simple way to call methods with enum-type params when using arbitrary data (from the client in the present case).
	          Values should even be taken from def_skills & def_subskills, with global constants

*/

#ifndef SKILL_H
#define SKILL_H

#define SUB_SKILLS u8
#define SK_MC 1
#define SK_HC 2
#define SK_TRA 3
#define SK_FOR 22
#define SK_PC 10
#define SK_RC 11
#define SK_TC 12
#define SK_VHC 13
#define SK_AGL 14
#define SK_REP 15
#define SK_REC 16
#define SK_RCL 17
#define SK_ATL 20
#define SK_END 21
#define SK_FIR 23
#define SK_ENR 24
#define SK_XRR 25
#define SK_POR 26
#define SK_HLT 27
#define SK_HCK 30
#define SK_BRT 31
#define SK_PSU 32
#define SK_WEP 33
#define SK_CST 34
#define SK_RES 35
#define SK_IMP 36
#define SK_PPU 40
#define SK_APU 41
#define SK_MST 42
#define SK_PPW 43
#define SK_PSR 44
#define SK_WPW 45

#define MAIN_SKILLS u8
#define MS_STR 1
#define MS_DEX 2
#define MS_CON 3
#define MS_INT 4
#define MS_PSI 5


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
            mINTsp = mCONsp = mSTRsp = mDEXsp = mPSIsp = 0;
            mINTxp = mCONxp = mSTRxp = mDEXxp = mPSIxp = 0.f;
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
        
        inline ~PSkills()
        {
            delete Main;
            delete INT;
            delete CON;
            delete STR;
            delete DEX;
            delete PSI;
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
