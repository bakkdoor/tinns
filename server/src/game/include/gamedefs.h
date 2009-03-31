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

#ifdef _MSC_VER
#pragma once
#endif

#include "defs.h"

typedef PDefMap<PDefCharacter> PDefCharactersMap;
typedef PDefMap<PDefSkill> PDefSkillsMap;
typedef PDefMap<PDefSubSkill> PDefSubSkillsMap;
typedef PDefMap<PDefCharKind> PDefCharKindsMap;
typedef PDefMap<PDefWorld> PDefWorldsMap;
typedef PDefMap<PDefFaction> PDefFactionsMap;
typedef PDefMap<PDefHack> PDefHacksMap;
class PDefItemsMap; // Derived classes for addition of specific members
typedef PDefMap<PDefWorldModel> PDefWorldModelsMap;
typedef PDefMap<PDefAppPlace> PDefAppPlacesMap;
class PDefAppartementsMap;
class PDefRespawnsMap;
class PDefWorldFilesMap;
typedef PDefMap<PDefWeapon> PDefWeaponsMap;
// ___Add new entries here___


class PGameDefs
{
  private :
    PDefCharactersMap mCharsDefs;
    PDefSkillsMap mSkillsDefs;
    PDefSubSkillsMap mSubSkillsDefs;
    PDefCharKindsMap mCharKindsDefs;
    PDefWorldsMap mWorldsDefs;
    PDefFactionsMap mFactionsDefs;
    PDefHacksMap mHacksDefs;
    PDefItemsMap mItemsDefs;
    PDefWorldModelsMap mWorldModelsDefs;
    PDefAppPlacesMap mAppPlacesDefs;
    PDefAppartementsMap mAppartementsDefs;
    PDefRespawnsMap mRespawnsDefs;
    PDefWorldFilesMap mWorldFilesDefs;
    PDefWeaponsMap mWeaponsDefs;
    // ___Add new entries here___


  public :
    PGameDefs();
    ~PGameDefs();

    bool Init();

    inline const PDefCharactersMap* Chars() const { return &mCharsDefs; }
    inline const PDefSkillsMap* Skills() const { return &mSkillsDefs; }
    inline const PDefSubSkillsMap* SubSkills() const { return &mSubSkillsDefs; }
    inline const PDefCharKindsMap* CharKinds() const { return &mCharKindsDefs; }
    inline const PDefWorldsMap* Worlds() const { return &mWorldsDefs; }
    inline const PDefFactionsMap* Factions() const { return &mFactionsDefs; }
    inline const PDefHacksMap* Hacks() const { return &mHacksDefs; }
    inline const PDefItemsMap* Items() const { return &mItemsDefs; }
    inline const PDefWorldModelsMap* WorldModels() const { return &mWorldModelsDefs; }
    inline const PDefAppPlacesMap* AppPlaces() const { return &mAppPlacesDefs; }
    inline const PDefAppartementsMap* Appartements() const { return &mAppartementsDefs; }
    inline const PDefRespawnsMap* Respawns() const { return &mRespawnsDefs; }
    inline const PDefWeaponsMap* Weapons() const { return &mWeaponsDefs; } 
    inline const PDefWorldFilesMap* WorldFiles() const { return &mWorldFilesDefs; }
    // ___Add new entries here___

};

#endif
