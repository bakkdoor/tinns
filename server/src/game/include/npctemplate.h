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
npctemplate.h - NPC template class

MODIFIED: 21 Jun 2009 Namikon
REASON: - creation

*/

#ifndef NPCTEMPLATE_H
#define NPCTEMPLATE_H
struct s_f32coords
{
    f32 mX;
    f32 mY;
    f32 mZ;
};
typedef std::map<u32, s_f32coords*> PWaypointMap;

class PNPCTemplate
{
    friend class PWorldDatParser;

private:

    PWaypointMap mWayPoints;

    u32       mUnknown1; // Is always 0x20001200, in every log. maybe header for NPCs?
    f32       mPosY;
    f32       mPosZ;
    f32       mPosX;
    u32       mNPCTypeID; //npc type in npc.def
    u8        mActorStringSize; //string size with ending 0
    u8        mAngleStringSize; //string size with ending 0
    u16       mNpcID;
    u8        mHasAdditionalCoords; // Seems to be the NUMBER OF additional Coords for the NPC ("Coords" = X/Y/Z in f32, not u16!!)
    u8        mUnknown2a;
    u8        mUnknown2b;
    u8        mUnknown2c;
    u16       mUnknown3; //00 00 ?
    u16       mUnknown4; //04 00 ?
    string    mActorName;
    string    mAngle;

    /* // Not sure about that. Commented out until someone finds out how to deal with those "extra" informations
    f32 mWaypoint1_Y;
    f32 mWaypoint1_Z;
    f32 mWaypoint1_X;
    f32 mWaypoint2_Y;
    f32 mWaypoint2_Z;
    f32 mWaypoint2_X;
    f32 mWaypoint3_Y;
    f32 mWaypoint3_Z;
    f32 mWaypoint3_X;
    f32 mWaypoint4_Y;
    f32 mWaypoint4_Z;
    f32 mWaypoint4_X;
    f32 mWaypoint5_Y;
    f32 mWaypoint5_Z;
    f32 mWaypoint5_X;
    */

public:
    PNPCTemplate();
    ~PNPCTemplate();

    void AddWayPoint(f32 nX, f32 nY, f32 nZ, u8 nID);
    // TODO: Add GetWaypoint function

    inline u32 GetUnknown1() const
    {
        return mUnknown1;
    };
    inline f32 GetPosX() const
    {
        return mPosY;
    };
    inline f32 GetPosY() const
    {
        return mPosZ;
    };
    inline f32 GetPosZ() const
    {
        return mPosX;
    };
    inline u32 GetNPCTypeID() const
    {
        return mNPCTypeID;
    };
    inline u8 GetActorStrSize() const
    {
        return mActorStringSize;
    };
    inline u8 GetAngleStrSize() const
    {
        return mAngleStringSize;
    };
    inline u16 GetNpcID() const
    {
        return mNpcID;
    };
    inline u8 GetHasAdditionalCoords() const
    {
        return mHasAdditionalCoords;
    };
    inline u8 GetUnknown2a() const
    {
        return mUnknown2a;
    };
    inline u8 GetUnknown2b() const
    {
        return mUnknown2b;
    };
    inline u8 GetUnknown2c() const
    {
        return mUnknown2c;
    };
    inline u16 GetUnknown3() const
    {
        return mUnknown3;
    };
    inline u16 GetUnknown4() const
    {
        return mUnknown4;
    };
    inline string GetActorName() const
    {
        return mActorName;
    };
    inline string GetAngle() const
    {
        return mAngle;
    };


    inline void SetUnknown1( u32 nValue )
    {
        mUnknown1 = nValue;
    };
    inline void SetPosX( f32 nValue )
    {
        mPosY = nValue;
    };
    inline void SetPosY( f32 nValue )
    {
        mPosZ = nValue;
    };
    inline void SetPosZ( f32 nValue )
    {
        mPosX = nValue;
    };
    inline void SetNPCTypeID( u32 nValue )
    {
        mNPCTypeID = nValue;
    };
    inline void SetActorStrSize( u8 nValue )
    {
        mActorStringSize = nValue;
    };
    inline void SetAngleStrSize( u8 nValue )
    {
        mAngleStringSize = nValue;
    };
    inline void SetNpcID( u16 nValue )
    {
        mNpcID = nValue;
    };
    inline void SetHasAdditionalCoords( u8 nValue )
    {
        mHasAdditionalCoords = nValue;
    };
    inline void SetUnknown2a( u8 nValue )
    {
        mUnknown2a = nValue;
    };
    inline void SetUnknown2b( u8 nValue )
    {
        mUnknown2b = nValue;
    };
    inline void SetUnknown2c( u8 nValue )
    {
        mUnknown2c = nValue;
    };
    inline void SetUnknown3( u16 nValue )
    {
        mUnknown3 = nValue;
    };
    inline void SetUnknown4( u16 nValue )
    {
        mUnknown4 = nValue;
    };
    inline void SetActorName( string nValue )
    {
        mActorName = nValue;
    };
    inline void SetAngle( string nValue )
    {
        mAngle = nValue;
    };
};

#endif
