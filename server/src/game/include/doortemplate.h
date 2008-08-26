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
  doortemplate.h - world door template class

	MODIFIED: 05 Nov 2006 Hammag
	REASON: - creation
	
*/


#ifndef DOORTEMPLATE_H
#define DOORTEMPLATE_H

#include "def_worldmodels.h"

class PDoorTemplate
{
  friend class PWorldDatParser;
  
  private:
    u16 mDoorID;

    // The commented out values are not loaded from dat file atm because they are not used yet.
    //u16 mUnknown1; //18 00
    //u16 mUnknown1bis; //00 00 ? varies
    f32 mPosY;
    f32 mPosZ;
    f32 mPosX;
    //u16 mUnknown5; //00 00 ? second byte varies
    u16 mWorldmodelID; //door type from worldmodel.def

    std::string mDoorTypeName;  
    int mDoorParameters[4];
    bool mIsDoubleDoor;
    bool mIsTriggeredDoor;
    
    const PDefWorldModel* mDefWorldModel;

  public:
    PDoorTemplate();
    ~PDoorTemplate();

    inline const u16 GetID() { return mDoorID; }
    inline const u16 GetUseFlags() { return (mDefWorldModel ? mDefWorldModel->GetUseFlags() : 0); }
    inline const u16 GetFunctionType() { return (mDefWorldModel ? mDefWorldModel->GetFunctionType() : 0); }
    inline const int GetFunctionValue() { return (mDefWorldModel ?  mDefWorldModel->GetFunctionValue() : 0); }
    inline const std::string& GetName() const { return (mDefWorldModel ?  mDefWorldModel->GetName() : EmptyString ); } /// !!!!
    inline const PDefWorldModel* GetDefWorldModel() const { return mDefWorldModel; }
    
    inline void GetPos(f32* nPosX, f32* nPosY, f32* nPosZ) const { *nPosY = mPosY; *nPosZ = mPosZ; *nPosX = mPosX;}
    inline u16 GetOtherDoorID() { return ( mIsDoubleDoor ? mDoorParameters[1] : 0 ); }
    inline bool IsDoubleDoor() const { return mIsDoubleDoor; }
    inline bool IsTriggeredDoor() const { return mIsTriggeredDoor; }
    
    void SetDoorTypeName(char* nDoorTypeName);
    void SetDoorParameters(char* nDoorParametersString);
};

#endif
