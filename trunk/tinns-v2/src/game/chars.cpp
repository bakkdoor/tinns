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

    ToDo:
    - Remove ALL parts of the old XML way and replace it with SQL
    - Add missing values for char, like sex and model details (head, torso, body)
*/

#include "main.h"

PChar::PChar()
{
	mID = 0;
	mAccount = 0;
	mProfession = 1;
	mFaction = 1;
	mModel = 0;
	mType = 0;
	mLocation = 1;
	mCash = 0;

    mIsOnline = false;
	mDirtyFlag = false;

	Skill = new PSkillHandler();
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

        // Profession
        int profvalue = std::atoi(row[c_profession]);
            if(GameDefs->GetCharDef(profvalue))
                mProfession = static_cast<u32>(profvalue);
        else
            mProfession = 1;

        // Faction
        //int facvalue = std::atoi(row[c_faction]);
        //    if(GameDefs->GetFactionDef(facvalue))
        //        mFaction = static_cast<u32>(facvalue);
        //else
            mFaction = 1;

        // Model
        //int modvalue = std::atoi(row[c_model]);
        //mModel = static_cast<u32>(modvalue);
        mModel = 1;

        // Type
        //int typevalue = std::atoi(row[c_type]);
        //mType = static_cast<u32>(typevalue);
        mType = 1;

        // Location
        int locvalue = std::atoi(row[c_location]);
        mLocation = static_cast<u32>(locvalue);

        // Cash
        float cashvalue = std::atof(row[c_cash]);
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
    }
    return true;
}

/*bool PChar::Load(TiXmlElement *Node)
{
	if(!Node)
		return false;

	const char *name = Node->Attribute("name");
	const char *id = Node->Attribute("id");

	if(name && id)
	{
		SetID(std::atoi(id));
		SetName(name);

		std::stringstream fname;
		fname << "./database/playerchars/" << GetID() << ".xml" << '\0';

		bool Error=false;
		TiXmlDocument doc(fname.str().c_str());
		if(doc.LoadFile())
		{
			TiXmlElement *Root = doc.RootElement();
			if(Root)
			{
				TiXmlElement *profession = Root->FirstChildElement("profession");
				TiXmlElement *faction = Root->FirstChildElement("faction");
				TiXmlElement *model = Root->FirstChildElement("model");
				TiXmlElement *type = Root->FirstChildElement("type");
				TiXmlElement *location = Root->FirstChildElement("location");

				if(profession)
				{
					int profvalue = 0;
					profession->Attribute("value", &profvalue);
					if(GameDefs->GetCharDef(profvalue))
						mProfession = static_cast<u32>(profvalue);
					else
						mProfession = 1;
				}

				if(faction)
				{
					int facvalue = 0;
					faction->Attribute("value", &facvalue);
					if(GameDefs->GetFactionDef(facvalue))
						mFaction = static_cast<u32>(facvalue);
					else
						mFaction = 1;
				}

				if(model)
				{
					int modvalue = 0;
					model->Attribute("value", &modvalue);
					mModel = static_cast<u32>(modvalue);
				}

				if(type)
				{
					int typevalue = 0;
					type->Attribute("value", &typevalue);
					mType = static_cast<u32>(typevalue);
				}

				if(location)
				{
					int locvalue = 1;
					location->Attribute("value", &locvalue);//NEW corrected
					mLocation = static_cast<u32>(locvalue);
				}

			} else
				Error=true;
		} else
			Error = true;

		if(Error)
		{
			Console->Print("PChar: could not load char %s (%d)", mName.c_str(), mID);
			return false;
		}
		return true;
	}

	return false;
}
*/
void PChar::Save() // TO BE REWRITTEN!!!!
{
    return;
/*
	std::stringstream fname;
	fname << "./database/playerchars/" << GetID() << ".xml" << '\0';
	std::stringstream tempname;
	tempname << "./database/playerchars/" << GetID() << ".tmp" << '\0';

	std::stringstream bakname;
	bakname << "./database/playerchars/" << GetID() << ".bak" << '\0';

	std::remove(tempname.str().c_str());

	TiXmlDocument doc;
	TiXmlElement Root("playerchar");
	Root.SetAttribute("id", GetID());
	Root.SetAttribute("name", mName.c_str());

	TiXmlElement type("type");
	type.SetAttribute("value", mType);
	Root.InsertEndChild(type);

	TiXmlElement prof("profession");
	prof.SetAttribute("value", mProfession);
	Root.InsertEndChild(prof);

	TiXmlElement fac("faction");
	fac.SetAttribute("value", mFaction);
	Root.InsertEndChild(fac);

	TiXmlElement model("model");
	model.SetAttribute("value", mModel);
	Root.InsertEndChild(model);

	TiXmlElement loc("location");
	loc.SetAttribute("value", mLocation);
	Root.InsertEndChild(loc);

	doc.InsertEndChild(Root);
	if(doc.SaveFile(tempname.str().c_str()))
	{
		std::remove(bakname.str().c_str());
		std::rename(fname.str().c_str(), bakname.str().c_str());
		std::rename(tempname.str().c_str(), fname.str().c_str());
	} else
	{
		Console->Print("PChar: could not save char %s (%d)", mName.c_str(), mID);
	}

	mDirtyFlag = false;
*/
}

void PChar::SetOnlineStatus(bool IsOnline)
{
    // Deactivated, until Maxx added c_isonline row to `characters`
    //char query[255];
    int onlinestatus = 0;

    if(IsOnline)
    {
        onlinestatus = 0;
    }
    else
    {
        onlinestatus = 1;
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
    const PDefCharKind *def = GameDefs->GetCharKindDef(mType);

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
    Packet[83] = (u8)def->GetSkillInfo(MS_DEX).mGrow;
    Packet[84] = (u8)def->GetSkillInfo(MS_DEX).mMax;

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

// ===================================

PChars::PChars()
{
	mLastID = 0;
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
    Console->Print("%s Loaded %i player chars", Console->ColorText(GREEN, BLACK, "[Success]"), nChars);
	mLastSave = std::clock();
}

/*void PChars::Load()
{
	Console->Print("Loading player chars...");
	int nChars=0;
	TiXmlDocument doc("./database/chars.xml");
	if(doc.LoadFile())
	{
		TiXmlElement *Root = doc.RootElement();
		if(Root)
		{
			TiXmlElement *List = Root->FirstChildElement("charlist");
			while(List)
			{
				int AccId = 0;
				List->Attribute("account", &AccId);
				if(AccId != 0)
				{
					PAccount *Account = Database->GetAccount(AccId);
					if(Account)
					{
						TiXmlElement *Ch = List->FirstChildElement("char");
						while(Ch)
						{
							PChar *info = new PChar();
							if(info->Load(Ch))
							{
								info->SetAccount(Account->GetID());
								mLastID = max(mLastID, info->GetID());
								if(!mChars.insert(std::make_pair(info->GetID(), info)).second)
								{
									Console->Print("Duplicate char id found: %s (%i)", info->GetName().c_str(), info->GetID());
									delete info;
								} else
								{
									if(!Account->AddChar(info->GetID()))
										Console->Print("Could not add char '%s' to account '%s'", info->GetName().c_str(), Account->GetName().c_str());
									++nChars;
								}
							} else
							{
								Console->Print("Invalid char entry found in database. Ignored.");
								delete info;
							}


							Ch=Ch->NextSiblingElement("char");
						}
					} else
						Console->Print("Ignoring chars of inexistant account %i", AccId);
				}

				List = List->NextSiblingElement("charlist");
			}
		}
	}

	Console->Print("Loaded %i player chars", nChars);
	mLastSave = std::clock();
}
*/
void PChars::Save()
{
    return;
/*	// saves all dirty-flagged chars and maintains global character list
	int nChars = 0;
	typedef std::map<u32, std::list<PChar*>*> AccCharList;
	AccCharList CharList;
	for(CharMap::const_iterator i=mChars.begin(); i!=mChars.end(); i++)
	{
		PChar *Char = i->second;
		AccCharList::iterator Acc = CharList.find(Char->GetAccount());
		if(Acc==CharList.end())
		{
			std::list<PChar*> *List = new std::list<PChar*>();
			List->push_back(Char);
			CharList.insert(std::make_pair(Char->GetAccount(), List));
		} else
		{
			Acc->second->push_back(Char);
		}
		if(Char->IsDirty())
		{
			Char->Save();
			++nChars;
		}
	}

	TiXmlDocument CList("./database/chars.xml");
	TiXmlElement Root("chars");
	for(AccCharList::const_iterator i = CharList.begin(); i!= CharList.end(); i++)
	{
		TiXmlElement AccList("charlist");
		AccList.SetAttribute("account", i->first);
		for(std::list<PChar*>::const_iterator j=i->second->begin(); j!=i->second->end(); j++)
		{
			TiXmlElement CharElement("char");
			CharElement.SetAttribute("name", (*j)->GetName().c_str());
			CharElement.SetAttribute("id", (*j)->GetID());
			AccList.InsertEndChild(CharElement);
		}
		delete i->second;
		Root.InsertEndChild(AccList);
	}
	CList.InsertEndChild(Root);
	if(!CList.SaveFile())
	{
		Console->Print("ERROR: could not save char list");
	}
	Console->Print("%i characters saved", nChars);
*/
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
	std::clock_t t = std::clock();

	// autosave characters every 10 minutes
	//NEW CHANGED TO 5 min
	if((t-mLastSave)/CLOCKS_PER_SEC >= 300)
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
			Console->Print("Autosaving characters...");
			Save();
			Console->Print("Autosave done.");
		}
		mLastSave = t;
	}
}

PChar *PChars::CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model)
{
	PChar *Char = new PChar();
	Char->SetName(Name);
	Char->SetModel(Model);
	Char->SetType(Type);
	Char->SetAccount(Account);
	mChars.insert(std::make_pair(++mLastID, Char));
	Char->SetID(mLastID);
	Char->SetDirtyFlag(true);
	Save();

	return Char;
}
