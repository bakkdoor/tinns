/*
	TinNS (TinNS is not a Neocron Server)
	Copyright (C) 2005 Linux Addicted Community

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
  furnituretemplate.h - world furniture template class

	MODIFIED: 04 Oct 2006 Hammag
	REASON: - creation
	
*/


#ifndef FURNITURETEMPLATE_H
#define FURNITURETEMPLATE_H

#include "def_worldmodels.h"

enum // Furniture Use flags (cumlative)
{
  ufTouchable = 1,
  ufUsable = 2,
  ufNoCollision = 4,
  ufChair = 8,
  ufToolTarget = 16,
  ufSelfCollisionBox = 64,
  ufGraphicalEffect = 128,
  ufNoSelectionBox = 256  
};

class PFurnitureItemTemplate
{
  friend class PWorldDatParser;
  
  private:
    u32 mObjectID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    f32 mPosY; //= mPosY from dat file + 32000, to be coherent with char Pos scale
    f32 mPosZ;
    f32 mPosX;
    //f32 mRotY;
    f32 mRotZ;
    //f32 mRotX;
    //u32 mScale; //00 00 80 3F ?  = float(1.0000) scale factor ? // mostly used by holoscreens (passiv object)
    //u32 mUnknown2; //01 00 00 00 ?
    u16 mModelID; // points to models.ini
    //u32 mUnknown3; //00 00 00 00 ?
    //u32 mUnknown4; //00 00 00 00 ?
    u16 mWorldmodelID; // points to worldmodel.def
    //u16 mUnknown5; //12 00 ? // changes sometime (ex: c288 ...)

    //f32 mBoxLowerY; //Bounding box, for use when ufSelfCollisionBox is set in mUseFlags.
    //f32 mBoxLowerZ;
    //f32 mBoxLowerX;
    //f32 mBoxUpperY;
    //f32 mBoxUpperZ;
    //f32 mBoxUpperX;
    
    u16 mFrontPosY;
    u16 mFrontPosZ;
    u16 mFrontPosX;
    u8 mFrontLR;
    
    const PDefWorldModel* mDefWorldModel;

    u32 mLinkedObjectID; // for buttons, stores the corresponding triggered door
                         // fo GR, stores order of the GR entity (spawn point) to later choose from respawn.def data
    
  public:
    PFurnitureItemTemplate();
    ~PFurnitureItemTemplate();

    inline const u32 GetID() { return mObjectID; }
    inline const u16 GetUseFlags() { return (mDefWorldModel ? mDefWorldModel->GetUseFlags() : 0); }
    inline const u16 GetFunctionType() { return (mDefWorldModel ? mDefWorldModel->GetFunctionType() : 0); }
    inline const int GetFunctionValue() { return (mDefWorldModel ?  mDefWorldModel->GetFunctionValue() : 0); }
    inline const std::string& GetName() { return (mDefWorldModel ?  mDefWorldModel->GetName() : EmptyString ); } /// !!!!
    inline const PDefWorldModel* GetDefWorldModel() const { return mDefWorldModel; }
    inline const u8 GetFrontLR() const { return mFrontLR; }
    inline void GetFrontPos(u16* nFrontPosX, u16* nFrontPosY, u16* nFrontPosZ) const { *nFrontPosY = mFrontPosY; *nFrontPosZ = mFrontPosZ; *nFrontPosX = mFrontPosX;}
    inline void GetPos(f32* nPosX, f32* nPosY, f32* nPosZ) const { *nPosY = mPosY; *nPosZ = mPosZ; *nPosX = mPosX;}
    
    inline void SetLinkedObjectID(u32 nID) { mLinkedObjectID = nID; }
    inline const u32 GetLinkedObjectID() const { return mLinkedObjectID; }

};

#endif

// *** from worldmodel.def ***

//function Type	
//	0 - none	
// 	1 - Itemcontainer	
//	2 - Terminal	
//	3 - Outfitter	
//	4 - Trader	
//	5 - Mineral	
//	6 - Respawn Station	
//	7 - GoGuardian	
//	8 - Hackterminal	
//	9 - Appartement Eingang	
//	10 - Appartement Ein/Ausgang	
//	11 - Appartement Klingel/Öffner	
//	12 - Standard Button	
//	13 - Hack Button	
//	14 - HOLOMATCH ENTRANCE	
//	15 - HOLOMATCH EXIT	
//	16 - HOLOMATCH REFRESH	
//	17 - HOLOMATCH HEAL	
//	18 - WORLDCHANGEACTOR	
//	19 - CLANTERMINAL	
//	20 - DATFILE WORLDCHANGE ACTOR	
//	21 - LOCATION FOR 20	
//	22 - 	
//	23 - EINTRITTSGELD BUTTON	
//	24- TUTORIALEXIT	
//	25 - EXPLOSIVE	
//	26 - Outpost Switch	
//	27 - Old goguardian	
//	28 - Fahrzeug Depot Interface	
//	29 - Underground Exit	
//	30 - Static FX (Value=Type. 1=Fire 2=Smoke 3=Steam 4=Sparkle)	
//	31 - Venture Warp Station	
//	32 - functionvalue+100 gibt eine Meldung aus der Text.ini [MISC] an. 	
//
