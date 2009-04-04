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
 def_mission.h

    CREATED: 02 Apr 2009 Hammag
*/

#ifndef DEF_MISSION_H
#define DEF_MISSION_H

#include "def.h"

class PDefMission : public PDef
{
  private :
    //int mIndex;
	int mSourceId;
    std::string mStartDialog;
	int mDescTextId;
	int mNpcType[4];
    std::string mNpcDialog[4];
	int mNpcDialogStartState[4];
    int mTargetType[4];
    int mTargetValue[4][3];
    int mEndMoney;
    int mEndXp;
    int mMaxTime;
    int mDifficulty;
    int mMinFactionValue;
    int mPoints;
    int mFlags;

  public :
    PDefMission();
    //~PDefMission();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetSourceId() const { return mSourceId; }
    inline const std::string &GetStartDialog() const { return mStartDialog; }
    inline int GetDescTextId() const { return mDescTextId; }
    inline int GetNpcType(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcType[nIdx] : 0) ; }
	inline const std::string &GetNpcDialog(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcDialog[nIdx] : EmptyString) ; }
    inline int GetNpcDialogStartState(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 4)) ? mNpcDialogStartState[nIdx] : 0) ; }
	inline int GetTargetType(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < 4)) ? mTargetType[nIdx] : 0) ; }
    inline int GetTargetValue(int nIdx, int nValIdx) const { return ( ((nIdx >= 0) && (nIdx < 4) && (nValIdx >= 0) && (nValIdx < 3)) ? mTargetValue[nIdx][nValIdx] : 0) ; }
	inline int GetEndMoney() const { return mEndMoney; }
    inline int GetEndXp() const { return mEndXp; }
	inline int GetMaxTime() const { return mMaxTime; }
    inline int GetDifficulty() const { return mDifficulty; }
    inline int GetMinFactionValue() const { return mMinFactionValue; }
	inline int GetPoints() const { return mPoints; }
    inline int GetFlags() const { return mFlags; }
};

/*
//mission source ID	> 0 NPC Type for mission source	< 0 Terminal ID for mission source
	
//mission NPC Type	> 0 NPC Type for mission NPC	< 0 Fraction ID for mission NPC		
//mission target Type	1 - kill NPC	2 - kill NPC type	"3 - dialogtrigger"	4 - kill NPC type range	5-dialogtrigger + counter	"6 - conquer outpost"	7 - conquer outpost + counter	8 - conquer + hold outpost		9-KillPlayer	10 - LootNPC	11 - LootNPCType

//target value 1	ID of related mission NPC	> 0 NPC Type of NPCs to kill    < 0 FractionID of NPCs to kill	"ID of related mission NPC"	> 0 NPC Type of NPCs to kill 	ID of related mission NPC	"> 0 outpost Type < 0 current outpost factionid"	> 0 outpost type < 0 original outpost faction type	> 0 outpost Type < 0 current outpost factionid		PlayerFaction	ID of related Mission NPC(0-4)	> 0 NPC Type of NPCs to kill    < 0 FractionID of NPCs to kill
	
//target value 2		Count of NPCs to kill	"ID of DialogTrigger"	Count of NPCs to kill	ID of DialogTrigger		Count of outposts to conquer	time to hold outpost		Kill Cnt	ItemID	NPC Cnt

//target value 3				Range of NPC Type	Trigger Counter - must reach 0 to be successfull					Max Leveldiff		Item ID
//missionflags	(bit field)
// NPC0 No Search 	1	
// NPC1 No Search 	2	
// NPC2 No Search 	4
// NPC3 No Search 	8
*/
#endif
