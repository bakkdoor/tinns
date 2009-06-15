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
	message.h - a data message & message pool class for tcp and udp connections (and maybe more later)

	Authors:
	- Hammag

	MODIFIED: 15 jul 2006 Hammag
	REASON: - Creation
*/
#ifndef MESSAGE_H
#define MESSAGE_H

#define MESSAGE_SIZES_LIST 32, 64, 128, 256, 512, 1024, 4096
#define MESSAGE_POOL_INIT NULL, NULL, NULL, NULL, NULL, NULL, NULL
#define MESSAGE_POOL_COUNT_INIT 0, 0, 0, 0, 0, 0, 0
#define MESSAGE_SIZES_NB 7
#define MESSAGE_ALLOC_NB 4

struct PMsgData
{
    PMsgData* mNextMsgData;
    u8* mBuffer; // NB: no need to manage buffer deletion atm, as they will stay until server end
};
//NB: putting mPoolId & mMaxSize in PMsgData rather than PMessage would be cleaner, but would put more mem overhead on each buffer
//    Doesn't matter much as PMsgData management is done only through PMessage

class PMessage
{
    private:
        static const u16 smMsgSizes[MESSAGE_SIZES_NB];
        static PMsgData* smMsgPoolHead[MESSAGE_SIZES_NB];
        static int smMsgPoolCount[MESSAGE_SIZES_NB];
        static int smMsgCount; //Used to trace unreleased messages with CheckMsgCount()

        u8 mPoolId;
        u16 mMaxSize;
        PMsgData* mData;
        u16 mUsedSize;
        u16 mNextByteOffset;

        void GetMsgBuffer(u16 nRequestedSize = 0); // the requested size is just a hint to avoid internal reaffectation of buffer
        void ReleaseMsgBuffer();
        void CheckAndExtend(u16 nRequestedSize);  // This is SIZE checked, not max OFFSET
        inline void UpdateUsedSize() { if (mNextByteOffset > mUsedSize) mUsedSize = mNextByteOffset; }

    public:
        static void CheckMsgCount(); //To be used in a place where no new message should remain between calls

        PMessage(u16 nRequestedSize = 0); // max size will be extended as needed in later write accesses (up to max configured size)
        PMessage(PMessage& nMessage); // creates a (size optimized, offset reset) copy of nMessage
        inline ~PMessage() { ReleaseMsgBuffer(); --smMsgCount; }

        void SetNextByteOffset(u16 nPos);
        inline void IncreaseNextByteOffset(u16 nIncrement) { SetNextByteOffset(mNextByteOffset + nIncrement); }
        inline void SetNextByteAtEnd() { mNextByteOffset = mUsedSize; }
        inline u16 GetNextByteOffset() { return mNextByteOffset; }
        void ForceSize(u16 nUsedSize);
        inline u16 GetSize() { return mUsedSize; }
        inline u16 GetMaxSize() { return mMaxSize; }
        inline bool EOM() {return (mNextByteOffset >= mUsedSize);} // End Of Message

      // Writing methods
        u8* GetMessageDataPointer(u16 nUsedSize); // extends buffer as needed by nUsedSize, and sets UsedSize at min nUsedSize
        PMessage& Fill(u8 Value = 0, u16 StartOffset = 0, u16 FillSize = 0); // !!! Does NOT update UsedSize, fills only up to current maxSize
        inline PMessage& Clear() { mNextByteOffset = mUsedSize = 0; return *this; }
        PMessage& Write(const void* nData, u16 nLength);
        PMessage& operator << (PMessage& nMessage);
        PMessage& operator << (const char* nString); //for null terminated string ! Copies includes ending \0
        inline PMessage& operator << (std::string& nString) { return (*this << nString.c_str()); }
        PMessage& operator << (u8 nU8);
        inline PMessage& operator << (char nChar) { return (*this << (u8) nChar);}
        PMessage& operator << (u16 nU16);
        PMessage& operator << (u32 nU32);
        PMessage& operator << (f32 nF32);

      // Mixt methods

        //The next 3 methods do NOT update NextByteOffset, but DO increase message size (UsedSize ans MaxSize) as needed by nOffset.
        u8& U8Data(u16 nOffset);
        u16& U16Data(u16 nOffset);
        u32& U32Data(u16 nOffset);
        f32& F32Data(u16 nOffset);

        //     *** didn't managed to overload [] operator :-/
        inline u8& operator [] (u16 nOffset) { return U8Data(nOffset); }
        //inline u16& operator [] (u16 nOffset) { return U16Data(nOffset); }
        //u32& operator [] (u16 nOffset);

        // Really makes a different message instance, with all data copied (no data shared)
        PMessage& operator = (PMessage& nMessage);

      // Reading methods
        // ChunkNumber count from 0, return NULL for empty chunk (ie StartOffset is over UsedSize). NextByteOffset NOT updated
        PMessage* GetChunk(u16 StartOffset, u16 ChunkSize, u16 ChunkNumber = 0);

        // Return pointer to the START of message data.
        inline u8 const* GetMessageData() { return mData->mBuffer; }

        //Following methods do NOT extend message or Used, and return 0/empty string if over UsedSize
        PMessage& operator >> (std::string& nString); //read up to null or EOM
        PMessage& operator >> (u8& nU8);
        inline PMessage& operator >> (char& nChar) { return (*this >> (u8&) nChar);}
        PMessage& operator >> (u16& nU16);
        PMessage& operator >> (u32& nU32);
        PMessage& operator >> (f32& nF32);

        // info/debug methods
        static void ListPools();
        static void DumpPools();
        void Dump();
        void DumpHead(char* nComment = "");
};

#endif
