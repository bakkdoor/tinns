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
 def_trader.h

    CREATED: 03 Apr 2009 Hammag
*/

#ifndef DEF_TRADER_H
#define DEF_TRADER_H

#include "def.h"
#define DEF_TRADER_MAXENTRIES 17

class PDefTrader : public PDef
{
  private :
    //int mIndex;
    int mType;
    float mMaxWealth;
    float mWealthRespawn;
    int mQuality;
    int mItemId[DEF_TRADER_MAXENTRIES]; // <0: item group
    float mItemPriceScale[DEF_TRADER_MAXENTRIES]; // sometime string like "1/5" ??? <=> 0.2 or Sell:1 / Buy 5 ????

  public :
    PDefTrader();
    //~PDefTrader();

    bool LoadFromDef( PTokenList *Tokens );

    inline int GetType() const { return mType; }
    inline float GetMaxWealth() const { return mMaxWealth; }
    inline float GetWealthRespawn() const { return mWealthRespawn; }
    inline int GetQuality() const { return mQuality; }
    inline int GetItemId(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < DEF_TRADER_MAXENTRIES)) ? mItemId[nIdx] : 0 ) ; }
    inline float GetItemPriceScale(int nIdx) const { return ( ((nIdx >= 0) && (nIdx < DEF_TRADER_MAXENTRIES)) ? mItemPriceScale[nIdx] : 0 ) ; }
};
/* Trader/Buyer mType
//	1	weapons	
//	2	ammo	
//	3	armor	
//	4	tools	
//	5	psi equipment	
//	
//	10	chemicals	
//	11	item parts	
//	12	implants	
//	13	bone enforcements	
//	
//	16	Buy anything	
//	
//	20	refreshments	
//	21	drugs	
//	22	medicaments	
//	23	real estates	
//	24	vehicles	
//	
*/
#endif

