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
  worlddatatemplate.cpp - world data template (from worlds .dat files) class

	MODIFIED: 04 Oct 2006 Hammag
	REASON: - creation
	
*/


#ifndef WORLDDATATEMPLATE_H
#define WORLDDATATEMPLATE_H

class PFurnitureItemTemplate;
typedef std::map<u32, PFurnitureItemTemplate*> PFurnitureItemsMap;

class PWorldDataTemplate
{
  private:
    std::string mName; // relative path+filename without leading ./ or ./worlds/ nor .dat extension
    PFurnitureItemsMap mFurnitureItems;
  
    void DatFileDataCleanup();
    
  public:
    PWorldDataTemplate();
    ~PWorldDataTemplate();
    
    bool LoadDatFile(std::string& nFilename, bool nTestAccesOnly = false);
    inline const std::string& GetName() { return mName; }
    u32 AddFurnitureItem(PFurnitureItemTemplate* nItem);
    
};

#endif
