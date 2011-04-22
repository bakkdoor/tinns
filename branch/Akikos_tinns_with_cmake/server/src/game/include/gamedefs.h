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
 gamedefs.h

 MODIFIED: 25 Dec 2005 Namikon
 REASON: - Added GPL
 MODIFIED: 21 Sep 2005 Hammag
 REASON: - Added PDefWorldModel related stuff
         - Added PDefAppPlace related stuff
  MODIFIED: 22 Sep 2005 Hammag
 REASON: - Added PDefAppartement related stuff
         - Added PDefRespawn related stuff
  MODIFIED: 28 Sep 2005 Hammag
 REASON: - Added PDefWorldFile related stuff

  MODIFIED: 07 Oct 2005 Hammag
 REASON: - Added methods to get const iterators on Appartment and WorldFile maps

*/

#ifndef GAMEDEFS_H
#define GAMEDEFS_H

#include "defs.h"

typedef PDefMap<PDefActionMod> PDefActionModsMap;
typedef PDefMap<PDefAmmo> PDefAmmosMap;
class PDefAppartementsMap; // Derived classe for addition of specific members
typedef PDefMap<PDefAppPlace> PDefAppPlacesMap; // No derived class needed here
typedef PDefMap<PDefBlueprintPieces> PDefBlueprintPiecesMap;
typedef PDefMap<PDefCharacter> PDefCharactersMap;
typedef PDefMap<PDefCharAction> PDefCharActionsMap;
typedef PDefMap<PDefCharKind> PDefCharKindsMap;
typedef PDefMap<PDefDamage> PDefDamagesMap;
typedef PDefMap<PDefDrug> PDefDrugsMap;
typedef PDefMap<PDefFaction> PDefFactionsMap;
//typedef PDefMap<PDefHack> PDefHacksMap; // File not used by KK
typedef PDefMap<PDefImplant> PDefImplantsMap;
typedef PDefMap<PDefItemContainer> PDefItemContainersMap;
typedef PDefMap<PDefItemMod> PDefItemModsMap;
typedef PDefMap<PDefItemRestriction> PDefItemRestrictionsMap;
class PDefItemsMap;
typedef PDefMap<PDefMission> PDefMissionsMap;
typedef PDefMap<PDefNpcArmor> PDefNpcArmorsMap;
typedef PDefMap<PDefNpcGroupSpawn> PDefNpcGroupSpawnsMap;
typedef PDefMap<PDefNpc> PDefNpcsMap;
typedef PDefMap<PDefOutpost> PDefOutpostsMap;
typedef PDefMap<PDefRecycle> PDefRecyclesMap;
class PDefRespawnsMap;
typedef PDefMap<PDefShot> PDefShotsMap;
typedef PDefMap<PDefSkill> PDefSkillsMap;
typedef PDefMap<PDefSubSkill> PDefSubSkillsMap;
typedef PDefMap<PDefTrader> PDefTradersMap;
typedef PDefMap<PDefVhc> PDefVhcsMap;
typedef PDefMap<PDefVhcSeat> PDefVhcSeatsMap;
typedef PDefMap<PDefWeapon> PDefWeaponsMap;
typedef PDefMap<PDefWeather> PDefWeathersMap;
typedef PDefMap<PDefWorld> PDefWorldsMap;
class PDefWorldFilesMap;
typedef PDefMap<PDefWorldModel> PDefWorldModelsMap;

class PDefScriptsMap;
//typedef PDefMap<PDefScripts> PDefScriptsMap;
// ___Add new entries here in alpĥabetical order___


class PGameDefs
{
  private :
    PDefActionModsMap mActionModsDefs;
    PDefAmmosMap mAmmosDefs;
    PDefAppartementsMap mAppartementsDefs;
    PDefAppPlacesMap mAppPlacesDefs;
    PDefBlueprintPiecesMap mBlueprintPiecesDefs;
    PDefCharactersMap mCharsDefs;
    PDefCharActionsMap mCharActionsDefs;
    PDefCharKindsMap mCharKindsDefs;
    PDefDamagesMap mDamagesDefs;
    PDefDrugsMap mDrugsDefs;
    PDefFactionsMap mFactionsDefs;
    //PDefHacksMap mHacksDefs;
    PDefImplantsMap mImplantsDefs;
    PDefItemContainersMap mItemContainersDefs;
    PDefItemModsMap mItemModsDefs;
    PDefItemRestrictionsMap mItemRestrictionsDefs;
    PDefItemsMap mItemsDefs;
    PDefMissionsMap mMissionsDefs;
    PDefNpcArmorsMap mNpcArmorsDefs;
    PDefNpcGroupSpawnsMap mNpcGroupSpawnsDefs;
    PDefNpcsMap mNpcsDefs;
    PDefOutpostsMap mOutpostsDefs;
    PDefRecyclesMap mRecyclesDefs;
    PDefRespawnsMap mRespawnsDefs;
    PDefShotsMap mShotsDefs;
    PDefSkillsMap mSkillsDefs;
    PDefSubSkillsMap mSubSkillsDefs;
    PDefTradersMap mTradersDefs;
    PDefVhcsMap mVhcsDefs;
    PDefVhcSeatsMap mVhcSeatsDefs;
    PDefWeaponsMap mWeaponsDefs;
    PDefWeathersMap mWeathersDefs;
    PDefWorldsMap mWorldsDefs;
    PDefWorldFilesMap mWorldFilesDefs;
    PDefWorldModelsMap mWorldModelsDefs;
    PDefScriptsMap mScriptDefs;
    // ___Add new entries here___

  public :
    PGameDefs();
    ~PGameDefs();

    bool Init();

    inline const PDefActionModsMap* ActionMods() const { return &mActionModsDefs; }
    inline const PDefAmmosMap* Ammos() const { return &mAmmosDefs; }
    inline const PDefAppartementsMap* Appartements() const { return &mAppartementsDefs; }
    inline const PDefAppPlacesMap* AppPlaces() const { return &mAppPlacesDefs; }
    inline const PDefBlueprintPiecesMap* BlueprintPieces() const { return &mBlueprintPiecesDefs; }
    inline const PDefCharactersMap* Chars() const { return &mCharsDefs; }
    inline const PDefCharActionsMap* CharActions() const { return &mCharActionsDefs; }
    inline const PDefCharKindsMap* CharKinds() const { return &mCharKindsDefs; }
    inline const PDefDamagesMap* Damages() const { return &mDamagesDefs; }
    inline const PDefDrugsMap* Drugs() const { return &mDrugsDefs; }
    inline const PDefFactionsMap* Factions() const { return &mFactionsDefs; }
    //inline const PDefHacksMap* Hacks() const { return &mHacksDefs; }
    inline const PDefImplantsMap* Implants() const { return &mImplantsDefs;}
    inline const PDefItemContainersMap* ItemContainers() const { return &mItemContainersDefs; }
    inline const PDefItemModsMap* ItemMods() const { return &mItemModsDefs; }
    inline const PDefItemRestrictionsMap* ItemRestrictions() const { return & mItemRestrictionsDefs; }
    inline const PDefItemsMap* Items() const { return &mItemsDefs; }
    inline const PDefMissionsMap* Missions() const { return &mMissionsDefs; }
    inline const PDefNpcArmorsMap* NpcArmors() const { return & mNpcArmorsDefs; }
    inline const PDefNpcGroupSpawnsMap* GroupSpawns() const { return &mNpcGroupSpawnsDefs; }
    inline const PDefNpcsMap* Npcs() const { return &mNpcsDefs; }
    inline const PDefOutpostsMap* Outposts() const { return &mOutpostsDefs; }
    inline const PDefRecyclesMap* Recycles() const { return &mRecyclesDefs; }
    inline const PDefRespawnsMap* Respawns() const { return &mRespawnsDefs; }
    inline const PDefShotsMap* Shots() const { return &mShotsDefs; }
    inline const PDefSkillsMap* Skills() const { return &mSkillsDefs; }
    inline const PDefSubSkillsMap* SubSkills() const { return &mSubSkillsDefs; }
    inline const PDefTradersMap* Traders() const { return &mTradersDefs; }
    inline const PDefVhcsMap* Vhcs() const { return &mVhcsDefs; }
    inline const PDefVhcSeatsMap* VhcSeats() const { return &mVhcSeatsDefs; }
    inline const PDefWeaponsMap* Weapons() const { return &mWeaponsDefs; }
    inline const PDefWeathersMap* Weathers() const { return &mWeathersDefs; }
    inline const PDefWorldsMap* Worlds() const { return &mWorldsDefs; }
    inline const PDefWorldFilesMap* WorldFiles() const { return &mWorldFilesDefs; }
    inline const PDefWorldModelsMap* WorldModels() const { return &mWorldModelsDefs; }
    inline const PDefScriptsMap* Scripts() const { return &mScriptDefs; }
    // ___Add new entries here___

};

#endif
