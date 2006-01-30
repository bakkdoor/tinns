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
	vehicle.h - Vehicle handling

	Authors:
	- Namikon

        MODIFIED: 08 Jan 2006 Namikon
        REASON: - initial release by Namikon
*/

#ifndef VEHICLE_H
#define VEHICLE_H

struct PSpawnedVehicles
{
    unsigned int Location;
    unsigned int Owner;
    unsigned short WorldID;
    unsigned char Type;
};

class PVehicles
{
    private:
        PSpawnedVehicles mSpawnedVHC[100];

    public:
        PVehicles();
        ~PVehicles();

        void SpawnVehicle(unsigned int loc, unsigned char type, unsigned int worldID, unsigned int owner);
        void GetSpawnedVehicle(PSpawnedVehicles *VHC, unsigned int worldID);
};

#endif
