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
	message.cpp - a data message & message pool class for tcp and udp connections (and maybe more later)


	Authors:
	- Hammag

	MODIFIED: 15 jul 2006 Hammag
	REASON: - Creation
*/

/*
#include "../game/main.h"
#include "message.h"
*/
#include "main.h"

const u16 PMessage::smMsgSizes[] = {MESSAGE_SIZES_LIST};
PMsgData* PMessage::smMsgPoolHead[] = {MESSAGE_POOL_INIT};
int PMessage::smMsgPoolCount[] = {MESSAGE_POOL_COUNT_INIT};
int PMessage::smMsgCount = 0;

void PMessage::CheckMsgCount()
{
  static int MaxMsgCount = 0;

  if (smMsgCount > MaxMsgCount)
  {
    Console->Print("%s Max In-use messages number increasing : %d (+%d)", Console->ColorText(YELLOW, BLACK, "[Notice]"), smMsgCount, smMsgCount-MaxMsgCount);
    MaxMsgCount = smMsgCount;
  }
}

PMessage::PMessage(u16 nRequestedSize)
{
  GetMsgBuffer(nRequestedSize);
  mUsedSize = 0;
  mNextByteOffset = 0;
  ++smMsgCount;
  //Console->Print("Created msgnum %d", smMsgCount);
}


void PMessage::GetMsgBuffer(u16 nRequestedSize) //no optimisation to try to used larger unused buffer
{
//Console->Print("Allocating buffer for size %d", nRequestedSize);
  for (mPoolId = 0; mPoolId < MESSAGE_SIZES_NB; mPoolId++)
  {
    if (smMsgSizes[mPoolId] >= nRequestedSize)
      break;
  }
  if (mPoolId == MESSAGE_SIZES_NB)
  {
    Console->Print(RED, BLACK, "[PANIC] PMessage::GetDataBuffer: requested size %d too large. Aborting", nRequestedSize);
    exit(-1);
  }
  mMaxSize = smMsgSizes[mPoolId];
//Console->Print("Using Pool n° %d (size %d)", mPoolId, smMsgSizes[mPoolId]);

  if ((mData = smMsgPoolHead[mPoolId]) == NULL)
  {
//Console->Print("Pool Empty, creating new buffers");
    mData = new PMsgData[MESSAGE_ALLOC_NB];
    for (int i = 0; i < MESSAGE_ALLOC_NB; i++)
      mData[i].mBuffer = new u8[mMaxSize];

    if (MESSAGE_ALLOC_NB > 1)
    {
      smMsgPoolHead[mPoolId] = mData + 1;
      for (int i = 1; i < MESSAGE_ALLOC_NB-1; i++)
        mData[i].mNextMsgData = mData + i + 1;
      mData[MESSAGE_ALLOC_NB-1].mNextMsgData = NULL;
    }
    
    smMsgPoolCount[mPoolId] += MESSAGE_ALLOC_NB;
  }
  else
  {
    smMsgPoolHead[mPoolId] = mData->mNextMsgData;
  }

//Console->Print("Buffer allocated 0x%08x", mData);
}

void PMessage::ReleaseMsgBuffer()
{
  mData->mNextMsgData = smMsgPoolHead[mPoolId];
  smMsgPoolHead[mPoolId] = mData;
//Console->Print("Buffer %08xd back in pool %d.", mData, mPoolId);
}

void PMessage::CheckAndExtend(u16 nRequestedSize) // This is SIZE checked, not max OFFSET
{
//Console->Print("Checking size: max %d, req %d", mMaxSize, nRequestedSize);
  if (nRequestedSize > mMaxSize)
  {
//Console->Print("Extension needed (max %d, req %d)", mMaxSize, nRequestedSize);
     PMsgData* SaveMsgBuffer = mData;
     ReleaseMsgBuffer();
     GetMsgBuffer(nRequestedSize);
     memcpy((void*)mData->mBuffer, (void*)SaveMsgBuffer->mBuffer, (size_t)mUsedSize);
//Console->Print("Original data (%d bytes) copied from %08xd to %08xd ", mUsedSize, SaveMsgBuffer, mData);
  }
}

void PMessage::SetNextByteOffset(u16 nPos)
{
  if (nPos >= mMaxSize)
    CheckAndExtend(nPos+1);
  mNextByteOffset = nPos;
}

void PMessage::ForceSize(u16 nUsedSize)
{
  if (nUsedSize > mMaxSize)
    CheckAndExtend(nUsedSize);
  mUsedSize = nUsedSize;
}

// Writing methods
char* PMessage::GetMessageDataPointer(u16 nUsedSize)
{
  if (nUsedSize > mMaxSize)
    CheckAndExtend(nUsedSize);
  if (nUsedSize > mUsedSize)
    mUsedSize = nUsedSize;
  return (char*) (mData->mBuffer);
}

PMessage& PMessage::Fill(u8 Value, u16 StartOffset, u16 FillSize)
{
  if (FillSize == 0)
    FillSize = mMaxSize;
  if (StartOffset >= mMaxSize)
    return *this;
  if (StartOffset + FillSize > mMaxSize)
    FillSize = mMaxSize - StartOffset;
  memset((void*)(mData->mBuffer + StartOffset), Value, (size_t)FillSize);
  return *this;
}

PMessage& PMessage::Write(const void* nData, u16 nLength)
{
  u16 tmpOffset = mNextByteOffset + nLength;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  memcpy((void*)(mData->mBuffer + mNextByteOffset), (void*)nData, (size_t)nLength);
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (PMessage& nMessage)
{
  u16 tmpOffset = mNextByteOffset + nMessage.mUsedSize;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  memcpy((void*)(mData->mBuffer + mNextByteOffset), (void*)(nMessage.mData->mBuffer), (size_t)(nMessage.mUsedSize));
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (const char* nString) //for null terminated string ! Copies includes ending \0
{
  int StringL = strlen(nString)+1;
  u16 tmpOffset = mNextByteOffset + StringL;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  memcpy((void*)(mData->mBuffer + mNextByteOffset), (void*)nString, (size_t)StringL);
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (u8 nU8)
{
  u16 tmpOffset = mNextByteOffset+1;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  mData->mBuffer[mNextByteOffset] = nU8;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (u16 nU16)
{
  u16 tmpOffset = mNextByteOffset+2;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(u16*)(mData->mBuffer + mNextByteOffset) = nU16;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (u32 nU32)
{
  u16 tmpOffset = mNextByteOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(u32*)(mData->mBuffer + mNextByteOffset) = nU32;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (f32 nF32)
{
  u16 tmpOffset = mNextByteOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(f32*)(mData->mBuffer + mNextByteOffset) = nF32;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

// Mixt methods
u8& PMessage::U8Data(u16 nOffset)
{
  u16 tmpOffset = nOffset+1;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return mData->mBuffer[nOffset];
}

u16& PMessage::U16Data(u16 nOffset)
{
  u16 tmpOffset = nOffset+2;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(u16*)(mData->mBuffer + nOffset);
}

u32& PMessage::U32Data(u16 nOffset)
{
  u16 tmpOffset = nOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(u32*)(mData->mBuffer + nOffset);
}

f32& PMessage::F32Data(u16 nOffset)
{
  u16 tmpOffset = nOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(f32*)(mData->mBuffer + nOffset);
}

PMessage& PMessage::operator = (PMessage& nMessage)
{
  if (mPoolId != nMessage.mPoolId)
  {
//Console->Print("Adjusting buffer before message copy: old pool %d => new pool %d", mPoolId, nMessage.mPoolId);
    ReleaseMsgBuffer();
    GetMsgBuffer(nMessage.mMaxSize);
  }

  mUsedSize = nMessage.mNextByteOffset;
  mNextByteOffset = nMessage.mNextByteOffset;
  memcpy((void*)(mData->mBuffer), (void*)(nMessage.mData->mBuffer), (size_t)mMaxSize);

  return *this;
}

// Reading methods
PMessage* PMessage::GetChunk(u16 StartOffset, u16 ChunkSize, u16 ChunkNumber)
{
  u16 ReqStartOffset = StartOffset + ChunkNumber * ChunkSize;
  if (ReqStartOffset >= mUsedSize)
    return NULL;
  u16 RealChunkSize = (ChunkSize < mUsedSize - ReqStartOffset) ? ChunkSize : mUsedSize - ReqStartOffset;

  PMessage* MsgChunk = new PMessage(RealChunkSize);
  memcpy((void*)(MsgChunk->mData->mBuffer), (void*)(mData->mBuffer + ReqStartOffset), (size_t)RealChunkSize);
  MsgChunk->mUsedSize = RealChunkSize;

  return MsgChunk;
}

PMessage& PMessage::operator >> (std::string& nString) //read up to null or EOM
{
  int i;
  u8* StringStart = mData->mBuffer + mNextByteOffset;
  char* FinalStringStart;
  for (i = 0; mNextByteOffset + i < mUsedSize; i++)
  {
    if (*(StringStart + i) == 0)
      break;
  }

  if (mNextByteOffset + i >= mUsedSize)
  {
    i = mUsedSize - mNextByteOffset +1;
    FinalStringStart = new char[i];
    memcpy((void*)FinalStringStart, (void*)StringStart, (size_t)i);
    mNextByteOffset = mUsedSize;
  }
  else
  {
    FinalStringStart = (char*)StringStart;
    mNextByteOffset = mNextByteOffset + i + 1;
  }

  nString.append(FinalStringStart);
  return *this;
}

PMessage& PMessage::operator >> (u8& nU8)
{
  u16 tmpOffset = mNextByteOffset+1;
  nU8 = (tmpOffset > mUsedSize) ? 0 : mData->mBuffer[mNextByteOffset];
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (u16& nU16)
{
  u16 tmpOffset = mNextByteOffset+2;
  nU16 = (tmpOffset > mUsedSize) ? 0 : *(u16*)(mData->mBuffer + mNextByteOffset);
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (u32& nU32)
{
  u16 tmpOffset = mNextByteOffset+4;
  nU32 = (tmpOffset > mUsedSize) ? 0 : *(u32*)(mData->mBuffer + mNextByteOffset);
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (f32& nF32)
{
  u16 tmpOffset = mNextByteOffset+4;
  nF32 = (tmpOffset > mUsedSize) ? 0 : *(f32*)(mData->mBuffer + mNextByteOffset);
  mNextByteOffset = tmpOffset;
  return *this;
}

/* Dump functions */

void PMessage::ListPools()
{
  PMsgData* iBuff;
  int n;

  Console->Print("--- Free buffers pool ---");

  for (int i = 0; i < MESSAGE_SIZES_NB; i++)
  {
    iBuff = smMsgPoolHead[i];
    n = 0;
    while (iBuff != NULL)
    {
      ++n;
      iBuff = iBuff->mNextMsgData;
    }
    //if (n > 0)
      Console->Print("\tBuffer pool %d (size %d): %d used - %d free buffers", i, smMsgSizes[i], smMsgPoolCount[i]-n, n);
    //else
    //  Console->Print("\tBuffer pool %d (size %d): Empty", i, smMsgSizes[i]);
  }
}

void PMessage::DumpPools()
{
  PMsgData* iBuff;
  int n;

  Console->Print("--- Buffers pool ---");

  for (int i = 0; i < MESSAGE_SIZES_NB; i++)
  {
    if ((iBuff = smMsgPoolHead[i]) != NULL)
    {
      n = 0;
      Console->Print("\tBuffer pool %d (size %d):", i, smMsgSizes[i]);
      while (iBuff != NULL)
      {
        Console->Print("\t\t%d : 0x%08x", n++, iBuff);
        iBuff = iBuff->mNextMsgData;
      }
    }
    else
      Console->Print("\tBuffer pool %d (size %d): Empty", i, smMsgSizes[i]);
  }
}

void PMessage::Dump()
{
  char tmpStr[64];
  std::string sDump;
  std::string sAsciiDump;
  int i, j;
  char* tmpBuff = GetMessageData();

  Console->Print("--- Message data (MsgData 0x%08x) ---", mData);
  Console->Print("Buffer from pool %d (max size %d), used 0x%04hx (%d) , data at 0x%08x", mPoolId, smMsgSizes[mPoolId], mUsedSize, mUsedSize, tmpBuff);
  for (i = 0; i < mUsedSize; i += 16)
  {
    sprintf(tmpStr, "\t%04hx:",i);
    sDump = tmpStr;
    sAsciiDump = "";
    for (j = 0; (j < 16) && ((i+j) < mUsedSize); j++)
    {
      sprintf(tmpStr, " %02hx",(u8)tmpBuff[i+j]);
      sDump += tmpStr;
      sAsciiDump += ((tmpBuff[i+j]>'\x19') && (tmpBuff[i+j]<'\x7F')) ? tmpBuff[i+j] : '.';
    }
    while (j++ < 16)
      sDump += "   ";
    Console->Print("%s  %s", sDump.c_str(),sAsciiDump.c_str());
  }
  Console->Print("\tNext byte offset 0x%04hx (%d)", mNextByteOffset, mNextByteOffset);
}

void PMessage::DumpHead(char* nComment)
{

  char* tmpBuff = GetMessageData();

  Console->Print("%s T:%02hx UID:%04hx UHID:%04hx Fnct:%02hx Seq:%04hx Cmd:%02hx Cmd2:%02hx Cmd3:%02hx",
      nComment, (u8)tmpBuff[0], *(u16*)&tmpBuff[1], *(u16*)&tmpBuff[3], (u8)tmpBuff[6], *(u16*)&tmpBuff[7], (u8)tmpBuff[9], (u8)tmpBuff[10], (u8)tmpBuff[12] );

}
