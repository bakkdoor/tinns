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

/** TinNS include file. Do NOT compile **/

/*
	MODIFIED: 27 Jul 2006 Hammag
	REASON: - Baseline message building now uses PMessage class
*/
    
    PMessage* BaselineMsg = new PMessage(512);
    PMessage SectionMsg(256);
    
    PChar *nChar = Database->GetChar(Client->GetCharID());
    const PDefCharKind *def = GameDefs->GetCharKindDef(nChar->GetProfession());
    PSkillHandler *Skill = nChar->Skill;
    
    Client->IncreaseTransactionID(3);
    
    // Head
    *BaselineMsg << (u8)0x22;
    *BaselineMsg << (u8)0x02; // ?
    *BaselineMsg << (u8)0x01; // ?
    
    // ---- Section 1 ----
    *BaselineMsg << (u8)0x01; // section id
    
    SectionMsg << (u8)0xfa; // ?  // // section content at offset 3
    SectionMsg << (u8)nChar->GetProfession();
    SectionMsg << (u16)Client->GetTransactionID(); // Transaction ID ? 0x8aa0
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
    SectionMsg << (u8)0x00; // ?
    SectionMsg << (u8)0x04; // pos X
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
    
    SectionMsg << (u8)0x01; // QB/Armor/Implants items nb  // section content at offset 3
    
    SectionMsg << (u16)0x06; // data size of item
    SectionMsg << (u8)0x00; // pos (X) in inv (QB slot 0)
    SectionMsg << (u8)0x00; // pos Y (0)
    SectionMsg << (u16)0x0051; // item id (torch)
    SectionMsg << (u16)0x01;  // Qty ... strange size ... and where is Type ??? High byte ?
    
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
    
    SectionMsg << (u16)0x00; // nb GR tagged
    SectionMsg << (u8)0x04;
    // For each GR tagged by char
    //SectionMsg << (u16)genrep.g_worldid;
    //SectionMsg << (u16)genrep.g_stationid;

    SectionMsg << (u8)0x04;
    SectionMsg << (u8)0x04;
    SectionMsg << (u8)0x00;
    SectionMsg << (u8)0;				//Boolean if anyone in direct ID follows
    //SectionMsg << (u32)Direct CharID if in direct ?
    SectionMsg << (u8)0;					//Number of Buddies
    SectionMsg << (u8)0x00; // ? or High byte of nb of buddies ?
    // For each buddy ?
    //SectionMsg << (u32)buddy CharID ?
    
    SectionMsg << (u16) Client->GetTransactionID(); // doesn't work yet // 0x00e0; ??  ? link with transaction ID ????
    SectionMsg << (u32)0x00000000;
    SectionMsg << (u32)0x00000000; // Epic status ?
    SectionMsg << (u16) nSkin;
    SectionMsg << (u8) nHead;
    SectionMsg << (u8) nTorso;
    SectionMsg << (u8) nLegs;
    SectionMsg << (u8)0x00; // Rank
    SectionMsg << (u32)(4 + 100000); // (nChar->GetBaseApartment() + 100000); // 0x22, 0x00, 0x00, 0x00, //Primary Apartment (GR activated) ???
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
    SectionMsg << (f32) 96.0f*9000.0f; // SL
    // Faction Sympathies
    SectionMsg << (f32) 1000.0f*1024.0f; // City Admin
    SectionMsg << (f32) 1000.0f*1024.0f; // Diamond
    SectionMsg << (f32) 1000.0f*1024.0f; // Next
    SectionMsg << (f32) 1000.0f*1024.0f; // Tangent 
    SectionMsg << (f32) 1000.0f*1024.0f; // Biotech
    SectionMsg << (f32) 1000.0f*1024.0f; // ProtoPharm
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
    
    SectionMsg << (u32)0x00; // Epic done Data
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
    SectionMsg << (u16) Client->GetTransactionID(); // ?? TransactionID ? 0x8aa0
    SectionMsg << (u32)nChar->GetID();
    
    *BaselineMsg << (u16)SectionMsg.GetSize();
    *BaselineMsg << SectionMsg;
    SectionMsg.Clear();
    
    
    // Message chunking & sending
    
    PMessage* ChunkBuffer;
    PMessage* ChunkMsg;
    const u16 ChunkSize = 220;
    u16 ChunksNum = (BaselineMsg->GetSize() + ChunkSize - 1)/ChunkSize;
    
    for (u16 ChunkID = 0; ChunkID < ChunksNum; ChunkID++)
    {
      ChunkBuffer = BaselineMsg->GetChunk(0, ChunkSize, ChunkID);
      if (ChunkBuffer == NULL)
      {
        Console->Print(RED, BLACK, "Baseline: Bad chunk number calculation: %d for size %d", ChunksNum, BaselineMsg->GetSize());
        break;
      }
      
      ChunkMsg = new PMessage(ChunkSize + 15);
      //Client->IncreaseUDP_ID();
      Client->SetUDP_ID(Client->GetUDP_ID()+1);
      
      *ChunkMsg << (u8)0x13;
      *ChunkMsg << (u16) Client->GetUDP_ID();
      *ChunkMsg << (u16) Client->GetSessionID();
      *ChunkMsg << (u8) (9 + ChunkBuffer->GetSize());
      *ChunkMsg << (u8)0x03;
      *ChunkMsg << (u16) Client->GetUDP_ID();
      *ChunkMsg << (u8)0x07;
      *ChunkMsg << (u16) ChunkID;
      *ChunkMsg << (u16) ChunksNum;
      *ChunkMsg << (u8)0x19;
      *ChunkMsg << *ChunkBuffer;
      
//Console->Print(GREEN, BLACK, "Baseline: ----------- chunk %d/%d", ChunkID+1, ChunksNum);
//ChunkMsg->Dump();
      
      delete ChunkBuffer;
      Client->getUDPConn()->SendMessage(ChunkMsg);
    }
    delete BaselineMsg;
    
    // Build & send CharInfo3
    const char UDPCharInfo3[] = {0x13, 0x04, 0x00, 0x21, 0x94, 
    		0x10,//                 |  Possible Time     |
    		0x03, 0x04, 0x00, 0x0d, 0x8b, 0xdd, 0x0b, 0x00, 0x47, 0xc0, 
    		0x22, 0x00, 0xe5, 0x0a, 0xbb, 0x00};	
    PMessage* CharInfo3Msg = new PMessage(22);
    CharInfo3Msg->Write(UDPCharInfo3, 22);
    
    //Client->IncreaseUDP_ID();
    Client->SetUDP_ID(Client->GetUDP_ID()+1);
    
    CharInfo3Msg->SetNextByteOffset(1);
    *CharInfo3Msg << (u16) Client->GetUDP_ID();
    *CharInfo3Msg << (u16) Client->GetSessionID();
    CharInfo3Msg->U16Data(7) = (u16) Client->GetUDP_ID();
    CharInfo3Msg->U32Data(10) = (u32) GameServer->GetGameTime(); // offset 10 (NeoX) or 11 ????
//Console->Print(GREEN, BLACK, "Baseline: ----------- time");
//CharInfo3Msg->Dump(); 
    Client->getUDPConn()->SendMessage(CharInfo3Msg);
    //Console->Print("GThex <%2X %2X %2X %2X> GTint <%d> GTfloat <%f>", packet2[11],  packet2[12],  packet2[13],  packet2[14], GameServer->GetGameTime(), (float)GameServer->GetGameTime());

