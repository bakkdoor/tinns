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
*/

#ifndef CHARS_H
#define CHARS_H

#ifdef _MSC_VER
	#pragma once
#endif

class PChar
{
	private :
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
            c_hlt, // In SQL, this is STILL c_htl. Maxx hasnt changed it yet
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
            c_con_xp
        };

		u32 mID;
		u32 mAccount;
		std::string mName;
		u32 mType;
		u32 mProfession;
		u32 mFaction;
		u32 mModel;
		u32 mLocation;
		u32 mCash;

		u16 mHealth;
		u16 mMana;
		u16 mStamina;

		struct PCharCoordinates {
		    u16 mX;     // X-Position in world
		    u16 mY;     // Y-Position in world
            u16 mZ;     // Z-Position in world
            u8 mUD;     // Up - Mid - Down (d6 - 80 - 2a)
            u8 mLR;     // Compass direction (S..E..N..W..S [0-45-90-135-180])
            u8 mACT;
            // mACT:
            // 0x00: Client in background (user alt-tabbed out)
            // 0x
            // .....

            inline PCharCoordinates()
            {
                mX = mY = mZ = mUD = mLR = mACT = 0;
            }
		};

        bool mIsOnline;
		bool mDirtyFlag;
	protected :
		friend class PChars;
		inline void SetID(u32 ID) { mID = ID; }
		inline void SetAccount(u32 Account) { mAccount = Account; }
		inline void SetName(const std::string &Name) { mName = Name; }
		inline void SetType(u32 Type) { mType = Type; }
		inline void SetProfession(u32 Profession) { mProfession = Profession; }
		inline void SetFaction(u32 Faction) { mFaction = Faction; }
		inline void SetModel(u32 Model) { mModel = Model; }

		inline u16 GetMaxHealth() { return mHealth; };
		inline u16 GetMaxMana() { return mMana; };
		inline u16 GetMaxStamina() { return mStamina; };

	public :
		PChar();

        PSkillHandler *Skill;
        PCharCoordinates Coords;

        //Skill = new PSkillHandler;
		inline u32 GetID() const { return mID; }
		inline u32 GetAccount() const { return mAccount; }
		inline const std::string &GetName() const { return mName; }
		inline u32 GetType() const { return mType; }
		inline u32 GetProfession() const { return mProfession; }
		inline u32 GetFaction() const { return mFaction; }
		inline u32 GetModel() const { return mModel; }
		inline u32 GetLocation() const { return mLocation; }

		inline bool IsDirty() const { return mDirtyFlag; }
        inline bool IsOnline() { return mIsOnline; }

        void SetOnlineStatus(bool IsOnline);

		//bool Load(TiXmlElement *Node);
		bool SQLLoad(int CharID);
		void FillinCharDetails(u8 *Packet);

		void Save();
		inline void SetLocation(u32 Location) { mLocation = Location; }//WAS PROTECTED
		inline void SetDirtyFlag(bool Dirty) { mDirtyFlag = Dirty; }//WAS PROTECTED
};

class PChars
{
	private :
		typedef std::map<u32, PChar*> CharMap;
		CharMap mChars;
		u32 mLastID;
		std::clock_t mLastSave;

	public :
		PChars();
		~PChars();

		void Load();
		void SQLLoad();

		void Save();

		PChar *GetChar(u32 CharID) const;
		PChar *GetChar(const std::string &Name) const;
		PChar *CreateChar(u32 Account, const std::string &Name, u32 Type, u32 Model);

		void Update();
};

#endif

