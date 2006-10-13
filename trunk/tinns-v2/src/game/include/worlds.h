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
  worlds.h - world class and world map class

	MODIFIED: 06 Oct 2006 Hammag
	REASON: - creation
	
*/


#ifndef WORLDS_H
#define WORLDS_H

class PWorld
{
  private:
    bool mIsAppartment;
    
  public:
    PWorld();
    ~PWorld();
    
    bool Create(u32 nWorldID);
    inline bool IsAppartment() { return mIsAppartment; }
};


typedef std::map<u32, PWorld*> PWorldsMap;
class PWorldDataTemplate;
typedef std::map<std::string, PWorldDataTemplate*> PWorldDataTemplatesMap;

class PWorlds
{
  private:
    bool mPreloadWorldsTemplates;
    bool mPreloadStaticWorlds;
    
    PWorldsMap mStaticWorldsMap; // lists all valid static worlds, with second=NULL if not loaded
    PWorldsMap mOnDemandWorldsMap; // mostly appartments. Could be used for instance dungeons too, but instance dungeons are crap :p
    PWorldDataTemplatesMap mWorldDataTemplatesMap;

    bool LeaseWorldDataTemplate(const std::string& nWorldName, const std::string& nFileName, const bool nPreloadPhase = false);
    void ReleaseWorldDataTemplate(const std::string& nWorldName);
    void UnloadWorldDataTemplate(const std::string& nWorldName);
    
  public:
    PWorlds();
    ~PWorlds();
    
    bool LoadWorlds();
    bool LeaseWorld(u32 nWorldID);
    PWorld* GetWorld(u32 nWorldID);
    void ReleaseWorld(u32 nWorldID);
    bool IsAppartment(u32 nWorldID);
    
};

#endif
