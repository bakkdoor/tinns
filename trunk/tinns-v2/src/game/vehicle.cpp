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
	vehicle.cpp - Vehicle handling

	Authors:
	- Namikon

        MODIFIED: 08 Jan 2006 Namikon
        REASON: - initial release by Namikon
*/

#include "main.h"

PVehicles::PVehicles()
{
}

PVehicles::~PVehicles()
{
}

void PVehicles::SpawnVehicle(unsigned int loc, unsigned char type, unsigned int worldID, unsigned int owner)
{
    int i;

    for (i=0;i<100;i++)
    {
        if(mSpawnedVHC[i].WorldID != 0)
            continue;
        else
        {
            mSpawnedVHC[i].Location = loc;
            mSpawnedVHC[i].Owner = owner;
            mSpawnedVHC[i].WorldID = worldID;
            mSpawnedVHC[i].Type = type;
        }
    }
}

void PVehicles::GetSpawnedVehicle(PSpawnedVehicles *VHC, unsigned int worldID)
{
    int i;

    for (i=0;i<100;i++)
    {
        if(mSpawnedVHC[i].WorldID != 0)
            continue;
        else if (mSpawnedVHC[i].WorldID != worldID)
            continue;
        else
        {
            *VHC = mSpawnedVHC[i];
            break;
        }
    }
}
