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
	world_datstruct.h
	.dat world files internal structures

	MODIFIED: 29 Sep 2006 Hammag
	REASON: - Creation
*/

#ifndef WORLD_DATSTRUCT_H
#define WORLD_DATSTRUCT_H

struct PWorldFileHeader
{
  u32 mHeaderSize; // must be 08 00 00 00, = header size after this field
  u32 mHeaderSig; // must be CF CF 0F 00
  u32 mSection; // must be 01 00 00 00
};

struct PSectionHeader
{
    u32 mHeaderSize; // must be 0C 00 00 00
    u32 mHeaderSig; // must be CF FF 00 00
    u32 mSection; // 00 00 00 00 means end
    u32 mDataSize;
};

struct PSec2ElemHeader
{
    u32 mHeaderSize; // must be 0C 00 00 00
    u32 mHeaderSig; // must be F1 FE FE 0F
    u32 mElementType; // 1000003, 1000005 or 1000014
    u32 mDataSize;
};

struct PSec2ElemType3 //static object ?
{
  f32 mPosY; //= u16 PosY - 32000
  f32 mPosZ;
  f32 mPosX;
  f32 mRotY;
  f32 mRotZ;
  f32 mRotX;
  f32 mScale; //00 00 80 3F ? = float(1.000000) !!! => scale factor ?????
  u32 mUnknown2; //01 00 00 00 ?
  u16 mModelID; // points to models.ini
  u32 mUnknown3; //00 00 00 00 ?
  u32 mUnknown4; //00 00 00 00 ?
  u16 mWorldmodelID; // points to worldmodel.def
  u16 mUnknown5; //12 00 ?
  u32 mObjectID;
  f32 mBoxLowerY; //Bounding box, for useflag "64 - selfconstructing colisionbox"
  f32 mBoxLowerZ; // s32 or u32 ?
  f32 mBoxLowerX;
  f32 mBoxUpperY;
  f32 mBoxUpperZ;
  f32 mBoxUpperX;
};

struct PSec2ElemType5Start //door
{
  u16 mUnknown1; //18 00
  u16 mUnknown1bis; //00 00 ? varies
  u32 mUnknown2; //f32 mPosY ?
  u32 mUnknown3; //f32 mPosZ ?
  u32 mUnknown4; //00 80 FB 43 ? f32 mPosX ?
  u8 mActorSize; //string size with ending 0
  u8 mParamSize; //string size with ending 0
  u16 mUnknown5; //00 00 ? second byte varies
  u16 mDoorID; // but what is the link with ObjectID sent in Use message (can't find the base offset .. or 0x80 for doors ???)
  u16 mWorldmodelID; //door type from worldmodel.def
};
  //Actor As String //null terminated string
  //Params As String //null terminated string - for DDOOR, 2nd param is the ID of the other (half)door (*)
                   //param1 = 2 => simple lateral move ?, 3 => door frontal+lateral move (as at Typherra memorial) ?
                   //last param = 0/1 for lateral move direction ? no ...
                   //(*) here is the bug(?) that makes open only one half of a double door


struct PSec2ElemType6Start //npc
{
  u16 mUnknown1; //20 00 ?
  u16 mUnknown2; //12 00 ?
  f32 mPosY;
  f32 mPosZ;
  f32 mPosX;
  u32 mNPCTypeID; //npc type in npc.def
  u8 mActorSize; //string size with ending 0
  u8 mParamSize; //string size with ending 0
  u16 mNpcID; // kind of ?
  u32 mUnknown3; //01 00 00 00 ?
  u16 mUnknown4; //00 00 ?
  u16 mUnknown5; //04 00 ?
};
  //Actor As String //null terminated string
  //Params As String //null terminated string - Seem to be the facing angle in degres
struct PSec2ElemType6End
{
  f32 mPosY2; //second position for movement ?
  f32 mPosZ2; //
  f32 mPosX2; //
};


//  u16 mStrSize; //non-zero terminated string size
//  Name As String //non-zero terminated string
struct PSec2ElemType15End //area definition/sound ?
{
  f32 mUnknown1;
  f32 mUnknown2;
  f32 mUnknown3;
  f32 mUnknown4;
  f32 mUnknown5;
};

#endif

