#include "common/message.h"

#include <cstring>
#include "common/console.h"

const uint16_t PMessage::smMsgSizes[] = {MESSAGE_SIZES_LIST};
PMsgData* PMessage::smMsgPoolHead[] = {MESSAGE_POOL_INIT};
int PMessage::smMsgPoolCount[] = {MESSAGE_POOL_COUNT_INIT};
int PMessage::smMsgCount = 0;

void PMessage::CheckMsgCount()
{
  static int MaxMsgCount = 0;

  if (smMsgCount > MaxMsgCount)
  {
    Console->Print("%s Max In-use messages number increasing : %d (+%d)", Console->ColorText(GREEN, BLACK, "[Info]"), smMsgCount, smMsgCount-MaxMsgCount);
    MaxMsgCount = smMsgCount;
  }
}

PMessage::PMessage(uint16_t nRequestedSize)
{
  GetMsgBuffer(nRequestedSize);
  mUsedSize = 0;
  mNextByteOffset = 0;
  ++smMsgCount;
  //Console->Print("Created msgnum %d", smMsgCount);
}

PMessage::PMessage(PMessage& nMessage)
{
  GetMsgBuffer(nMessage.mUsedSize);
  mUsedSize = nMessage.mUsedSize;
  mNextByteOffset = 0;
  memcpy((void*)(mData->mBuffer), (void*)(nMessage.mData->mBuffer), (size_t)mUsedSize);
  ++smMsgCount;
}

void PMessage::GetMsgBuffer(uint16_t nRequestedSize) //no optimisation to try to used larger unused buffer
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
//Console->Print("Using Pool n� %d (size %d)", mPoolId, smMsgSizes[mPoolId]);

  if ((mData = smMsgPoolHead[mPoolId]) == NULL)
  {
//Console->Print("Pool Empty, creating new buffers");
    mData = new PMsgData[MESSAGE_ALLOC_NB];
    for (int i = 0; i < MESSAGE_ALLOC_NB; i++)
      mData[i].mBuffer = new uint8_t[mMaxSize];

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

void PMessage::CheckAndExtend(uint16_t nRequestedSize) // This is SIZE checked, not max OFFSET
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

void PMessage::SetNextByteOffset(uint16_t nPos)
{
  if (nPos >= mMaxSize)
    CheckAndExtend(nPos+1);
  mNextByteOffset = nPos;
}

void PMessage::ForceSize(uint16_t nUsedSize)
{
  if (nUsedSize > mMaxSize)
    CheckAndExtend(nUsedSize);
  mUsedSize = nUsedSize;
}

// Writing methods
uint8_t* PMessage::GetMessageDataPointer(uint16_t nUsedSize)
{
  if (nUsedSize > mMaxSize)
    CheckAndExtend(nUsedSize);
  if (nUsedSize > mUsedSize)
    mUsedSize = nUsedSize;
  return (uint8_t*) (mData->mBuffer);
}

PMessage& PMessage::Fill(uint8_t Value, uint16_t StartOffset, uint16_t FillSize)
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

PMessage& PMessage::Write(const void* nData, uint16_t nLength)
{
  uint16_t tmpOffset = mNextByteOffset + nLength;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  memcpy((void*)(mData->mBuffer + mNextByteOffset), (void*)nData, (size_t)nLength);
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (PMessage& nMessage)
{
  uint16_t tmpOffset = mNextByteOffset + nMessage.mUsedSize;
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
  uint16_t tmpOffset = mNextByteOffset + StringL;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  memcpy((void*)(mData->mBuffer + mNextByteOffset), (void*)nString, (size_t)StringL);
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (uint8_t nU8)
{
  uint16_t tmpOffset = mNextByteOffset+1;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  mData->mBuffer[mNextByteOffset] = nU8;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (uint16_t nU16)
{
  uint16_t tmpOffset = mNextByteOffset+2;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(uint16_t*)(mData->mBuffer + mNextByteOffset) = nU16;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (uint32_t nU32)
{
  uint16_t tmpOffset = mNextByteOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(uint32_t*)(mData->mBuffer + mNextByteOffset) = nU32;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

PMessage& PMessage::operator << (float nF32)
{
  uint16_t tmpOffset = mNextByteOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  *(float*)(mData->mBuffer + mNextByteOffset) = nF32;
  mNextByteOffset = tmpOffset;
  UpdateUsedSize();
  return *this;
}

// Mixt methods
uint8_t& PMessage::U8Data(uint16_t nOffset)
{
  uint16_t tmpOffset = nOffset+1;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return mData->mBuffer[nOffset];
}

uint16_t& PMessage::U16Data(uint16_t nOffset)
{
  uint16_t tmpOffset = nOffset+2;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(uint16_t*)(mData->mBuffer + nOffset);
}

uint32_t& PMessage::U32Data(uint16_t nOffset)
{
  uint16_t tmpOffset = nOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(uint32_t*)(mData->mBuffer + nOffset);
}

float& PMessage::F32Data(uint16_t nOffset)
{
  uint16_t tmpOffset = nOffset+4;
  if (tmpOffset > mMaxSize)
    CheckAndExtend(tmpOffset);
  if (tmpOffset > mUsedSize)
    mUsedSize = tmpOffset;

  return *(float*)(mData->mBuffer + nOffset);
}

PMessage& PMessage::operator = (PMessage& nMessage)
{
  if(&nMessage != this) {
    if (mPoolId != nMessage.mPoolId)
    {
  //Console->Print("Adjusting buffer before message copy: old pool %d => new pool %d", mPoolId, nMessage.mPoolId);
      ReleaseMsgBuffer();
      GetMsgBuffer(nMessage.mMaxSize);
    }
  
    mUsedSize = nMessage.mUsedSize;
    mNextByteOffset = nMessage.mNextByteOffset;
    memcpy((void*)(mData->mBuffer), (void*)(nMessage.mData->mBuffer), (size_t)mMaxSize);
  }
  
  return *this;
}

// Reading methods
PMessage* PMessage::GetChunk(uint16_t StartOffset, uint16_t ChunkSize, uint16_t ChunkNumber)
{
  uint16_t ReqStartOffset = StartOffset + ChunkNumber * ChunkSize;
  if (ReqStartOffset >= mUsedSize)
    return NULL;
  uint16_t RealChunkSize = (ChunkSize < mUsedSize - ReqStartOffset) ? ChunkSize : mUsedSize - ReqStartOffset;

  PMessage* MsgChunk = new PMessage(RealChunkSize);
  memcpy((void*)(MsgChunk->mData->mBuffer), (void*)(mData->mBuffer + ReqStartOffset), (size_t)RealChunkSize);
  MsgChunk->mUsedSize = RealChunkSize;

  return MsgChunk;
}

PMessage& PMessage::operator >> (std::string& nString) //read up to null or EOM
{
  int i;
  uint8_t* StringStart = mData->mBuffer + mNextByteOffset;
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

PMessage& PMessage::operator >> (uint8_t& nU8)
{
  uint16_t tmpOffset = mNextByteOffset+1;
  nU8 = (tmpOffset > mUsedSize) ? 0 : mData->mBuffer[mNextByteOffset];
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (uint16_t& nU16)
{
  uint16_t tmpOffset = mNextByteOffset+2;
  nU16 = (tmpOffset > mUsedSize) ? 0 : *(uint16_t*)(mData->mBuffer + mNextByteOffset);
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (uint32_t& nU32)
{
  uint16_t tmpOffset = mNextByteOffset+4;
  nU32 = (tmpOffset > mUsedSize) ? 0 : *(uint32_t*)(mData->mBuffer + mNextByteOffset);
  mNextByteOffset = tmpOffset;
  return *this;
}

PMessage& PMessage::operator >> (float& nF32)
{
  uint16_t tmpOffset = mNextByteOffset+4;
  nF32 = (tmpOffset > mUsedSize) ? 0 : *(float*)(mData->mBuffer + mNextByteOffset);
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
  char* tmpBuff = (char*) GetMessageData();

  Console->Print("--- Message data (MsgData 0x%08x) ---", mData);
  Console->Print("Buffer from pool %d (max size %d), used 0x%04hx (%d) , data at 0x%08x", mPoolId, smMsgSizes[mPoolId], mUsedSize, mUsedSize, tmpBuff);
  for (i = 0; i < mUsedSize; i += 16)
  {
    snprintf(tmpStr, 64, "\t%04hx:",i);
    sDump = tmpStr;
    sAsciiDump = "";
    for (j = 0; (j < 16) && ((i+j) < mUsedSize); j++)
    {
      snprintf(tmpStr, 64, " %02hx",(uint8_t)tmpBuff[i+j]);
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

  char* tmpBuff = (char*) GetMessageData();

  Console->Print("%s T:%02hx UID:%04hx UHID:%04hx Fnct:%02hx Seq:%04hx Cmd:%02hx Cmd2:%02hx Cmd3:%02hx",
      nComment, (uint8_t)tmpBuff[0], *(uint16_t*)&tmpBuff[1], *(uint16_t*)&tmpBuff[3], (uint8_t)tmpBuff[6], *(uint16_t*)&tmpBuff[7], (uint8_t)tmpBuff[9], (uint8_t)tmpBuff[10], (uint8_t)tmpBuff[12] );

}
