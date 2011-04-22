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

 gamedefs - this part loads "all" the definitions found in defs/

 MODIFIED: 30 August 2005 Akiko
 REASON: - added this fency header
  - changed the path of every def file to fellow the TinNS way (loading by the config.xml)
  - reformated code for easier reading
 MODIFIED: 31 August 2005 Akiko
 REASON: - updated paths for the def files
 MODIFIED: 25 Dec 2005 Namikon
 REASON: - Added GPL
 MODIFIED: 06 Jan 2006
 REASON: - Added GetCharKindDef(), was missing somehow
 MODIFIED: 21 Sep 2005 Hammag
 REASON: - Added PDefWorldModel related stuff
         - Added PDefAppPlace related stuff
         - completed PGameDefs destructor
  MODIFIED: 22 Sep 2005 Hammag
 REASON: - Added PDefAppartement related stuff
         - Added PDefRespawn related stuff
  MODIFIED: 28 Sep 2005 Hammag
 REASON: - Added PDefWorldFile related stuff

  MODIFIED: 07 Oct 2005 Hammag
 REASON: - Added (inline) methods to get const iterators on Appartment and WorldFile maps

 --------------------------------------------------------------------
 WARNING:
 When adding new .def support, don't forget to add required stuff in PGameDefs::Init()
   (compiler won't complain if you don't add that)
 --------------------------------------------------------------------

 NOTA: as PGameDefs uses the PDefParser class, which uses the PFileSystem and PFile classes,
   files are searched in the same way as the NC client does:
     - first try to load the file in unpacked form from the given directory,
     - else tries to load the file in packed form from the given directory,
         appending pak_ to its name,
     - else tries to load the packed file from the .pak archive which name is
         derived from the last part of the path.

*/

#include "main.h"

PGameDefs::PGameDefs() {}
PGameDefs::~PGameDefs() {}

bool PGameDefs::Init()
{
  Console->Print( "Initializing game defs..." );
  bool Res = true;

  Res |= mActionModsDefs.Load ("Action mods", "defs/actionmod.def");
  Res |= mAmmosDefs.Load ("Ammos", "defs/ammo.def");
  Res |= mAppartementsDefs.Load ("Apartments", "defs/appartements.def");
  Res |= mAppPlacesDefs.Load ("App Places", "defs/appplaces.def");
  Res |= mBlueprintPiecesDefs.Load ("Blueprint pieces", "defs/blueprintpieces.def");
  Res |= mCharsDefs.Load ("Chars", "defs/characters.def");
  Res |= mCharActionsDefs.Load ("Char actions", "defs/charaction.def");
  Res |= mDamagesDefs.Load ("Damage", "defs/damage.def");
  Res |= mDrugsDefs.Load ("Drugs", "defs/drugs.def");
  Res |= mFactionsDefs.Load ("Factions", "defs/fractions.def");
  //Res |= mHacksDefs.Load ("Hack", "defs/hack.def");
  Res |= mImplantsDefs.Load ("Implants", "defs/implants.def");
  Res |= mItemContainersDefs.Load ("Item containers", "defs/itemcontainer.def");
  Res |= mItemModsDefs.Load ("Item mods", "defs/itemmod.def");
  Res |= mItemRestrictionsDefs.Load ("Item restrictions", "defs/itemres.def");
  Res |= mItemsDefs.Load ("Items", "defs/items.def");
  Res |= mMissionsDefs.Load ("Missions", "defs/missionbase.def");
  Res |= mNpcArmorsDefs.Load ("NPC Armors", "defs/npcarmor.def");
  Res |= mNpcGroupSpawnsDefs.Load ("NPC group spawns", "defs/npcgroupspawn.def");
  Res |= mNpcsDefs.Load ("NPC", "defs/npc.def");
  Res |= mOutpostsDefs.Load ("Outposts", "defs/outposts.def");
  Res |= mRecyclesDefs.Load ("Recycles", "defs/recycles.def");
  Res |= mRespawnsDefs.Load ("Respawns", "defs/respawn.def");
  Res |= mShotsDefs.Load ("Shots", "defs/shots.def");
  Res |= mSubSkillsDefs.Load ("Subskills", "defs/subskill.def"); // Loading before skills is required
  Res |= mSkillsDefs.Load ("Skills", "defs/skills.def");
  Res |= mCharKindsDefs.Load ("Char kinds", "defs/charkinds.def"); // Loading after skills is required
  Res |= mTradersDefs.Load ("Traders", "defs/trader.def");
  Res |= mVhcsDefs.Load ("Vehicles", "defs/vehicles.def");
  Res |= mVhcSeatsDefs.Load ("Vehicle seats", "defs/vehiclesits.def");
  Res |= mWeaponsDefs.Load ("Weapons", "defs/weapons.def");
  Res |= mWeathersDefs.Load ("Weathers", "defs/weather.def");
  Res |= mWorldsDefs.Load ("Worldinfo", "defs/worldinfo.def");
  Res |= mWorldFilesDefs.Load ("World files", "worlds/worlds.ini");
  Res |= mWorldModelsDefs.Load ("World models", "defs/worldmodel.def");
  Res |= mScriptDefs.Load ("Script defs", "defs/scripts.def");

  return ( Res );
}
