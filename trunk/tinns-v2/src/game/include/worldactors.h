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

 worldactors.h - Management class for dynamic worldactors

 CREATION: 02 Jan 2007 Namikon

 MODIFIED:
 REASON: -

*/

#ifndef WORLDACTORS_H
#define WORLDACTORS_H

// Start from this offset (00 00 80 00)
#define DYNACTORIDSTART 8388608
#define DummyValue 20202

class PWorldActors
{
    private:
	// SQL Layout
        enum
        {
            wa_id,
            wa_actor_id,
            wa_actor_map,
            wa_actor_model,
            wa_actor_type,
            wa_posX,
            wa_posY,
            wa_posZ,
            wa_rotX,
            wa_rotY,
            wa_rotZ,
            wa_option1,
            wa_option2,
            wa_option3
        };

        void SpawnWA(u32 nWorld, u16 nActorID, u16 nFunctionID, u32 nWOID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotX);
        void VanishWA(u32 nWorld, u32 nWAid);

        // Get next availeable WorldactorID. First, try to find ID in known .dat files, then
        // Get the currently highest ID from SQL and add +1
        // select distinct wi_worlditem_id from world_items order by wi_worlditem_id desc limit 1;
        u32 GetNextFreeWAID();

    public:
        PWorldActors();
        ~PWorldActors();

        // Sends initial zone setup to client (after zoning-in)
        void InitWorld(PClient* nClient);

        // Add new worldactor to database and spawn it. Returns created WorldID
        // Position is taken from nClient, same as the worldID and calls the mainfunction
        // The function values are optional (used for interactive objects, gogo or genrep)
        u32 AddWorldActor(PClient* nClient, u16 nActorID, u16 nFuncID, u16 nOpt1 = 0, u16 nOpt2 = 0, u16 nOpt3 = 0);

        // Add new worldactor to database and spawn it. Returns created WorldID
        // The function values are optional (used for interactive objects, gogo or genrep)
        u32 AddWorldActor(u32 nWorldID, u16 nActorID, u16 nFuncID, u16 nPosX, u16 nPosY, u16 nPosZ, u8 nRotX, u8 nRotY, u8 nRotZ, u16 nOpt1 = 0, u16 nOpt2 = 0, u16 nOpt3 = 0);

        // Remove worldactor in given world from SQL and game
        void DelWorldActor(PClient* nClient, u32 nWAid);

        // Get functionvalues for worldactor
        void GetWAoption(u32 nWAid, u16 nWorld, u16 &nValue1, u16 &nValue2, u16 &nValue3);

        // Get SQL ID from world and worldID
        int GetWASQLID(u32 nWAid, u32 nWorld);

        // Check if actorID is dynamic
        bool IsDynamicActor(u32 nWAid);
};

#endif
