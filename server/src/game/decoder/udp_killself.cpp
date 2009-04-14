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

 udp_killself.cpp - decoder classe for UDP /set kill_self 1 messages

 CREATION: 13 Apr 2009 Hammag

*/


#include "main.h"
#include "udp_killself.h"

PUdpKillSelf::PUdpKillSelf( PMsgDecodeData* nDecodeData ) : PUdpMsgAnalyser( nDecodeData )
{
  nDecodeData->mName << "/0x10";
}

PUdpMsgAnalyser* PUdpKillSelf::Analyse()
{
  mDecodeData->mName << "=Char Kill self";

  mDecodeData->mState = DECODE_ACTION_READY | DECODE_FINISHED;
  return this;
}

bool PUdpKillSelf::DoAction()
{
  Chat->send(mDecodeData->mClient, CHAT_DIRECT, "System", "Suicide is not an accepted behavior here");
  mDecodeData->mState = DECODE_ACTION_DONE | DECODE_FINISHED;
  return true;

/* Server resp:
13:a8:00:a8:e2:
16: 03:a5:00:1b:01:10:00:80:24:ea:7c:cb:80:25:77:80:86:80:62:00:01:00: // Spwan belt object (?)
09: 03:a6:00:1f:01:00:25:23:1c: // "UndefinedUseMsg" with arg 0x1c
0b: 03:a7:00:1f:01:00:16:00:00:00:00: // ? Char Dead appareance ?
0e: 03:a8:00:1f:01:00:25:13:10:19:02:02:0d:00 // Inventory delete item (=item dropped to belt)
*/
}
