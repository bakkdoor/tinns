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
npctemplate.cpp - NPC template class

MODIFIED: 21 Jun 2009 Namikon
REASON: - creation

*/


#include "main.h"
#include "npctemplate.h"

PNPCTemplate::PNPCTemplate()
{
    mUnknown1 = 0;
    mPosY = 0.0f;
    mPosZ = 0.0f;
    mPosX = 0.0f;
    mNPCTypeID = 0;
    mActorStringSize = 0;
    mAngleStringSize = 0;
    mNpcID = 0;
    mHasAdditionalCoords = 0;
    mUnknown2a = 0;
    mUnknown2b = 0;
    mUnknown2c = 0;
    mUnknown3 = 0;
    mUnknown4 = 0;
    mActorName = "";
    mAngle = "";
}

PNPCTemplate::~PNPCTemplate()
{
    // Cleanup waypoints
    for ( PWaypointMap::iterator i = mWayPoints.begin(); i != mWayPoints.end(); i++ )
        delete i->second;
}

void PNPCTemplate::AddWayPoint(f32 nX, f32 nY, f32 nZ, u8 nID)
{
    struct s_f32coords *tWayPoint = new s_f32coords;
    tWayPoint->mX = nX;
    tWayPoint->mY = nY;
    tWayPoint->mZ = nZ;

    if ( mWayPoints.insert( std::make_pair( nID, tWayPoint)).second )
        if ( gDevDebug ) Console->Print( "%s Waypoint %d (X %0.1f Y %0.1f Z %0.1f) added to NPC", Console->ColorText( CYAN, BLACK, "[DEBUG]" ), nID, nX, nY, nZ );
}
