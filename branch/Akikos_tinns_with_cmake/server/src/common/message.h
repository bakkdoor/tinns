#pragma once

#include <iostream>

#define MESSAGE_SIZES_LIST 32, 64, 128, 256, 512, 1024, 4096
#define MESSAGE_POOL_INIT NULL, NULL, NULL, NULL, NULL, NULL, NULL
#define MESSAGE_POOL_COUNT_INIT 0, 0, 0, 0, 0, 0, 0
#define MESSAGE_SIZES_NB 7
#define MESSAGE_ALLOC_NB 4

struct PMsgData {
    PMsgData* mNextMsgData;
    uint8_t* mBuffer; // NB: no need to manage buffer deletion atm, as they will stay until server end
};
//NB: putting mPoolId & mMaxSize in PMsgData rather than PMessage would be cleaner, but would put more mem overhead on each buffer
//    Doesn't matter much as PMsgData management is done only through PMessage

class PMessage {
    private:
        static const uint16_t smMsgSizes[MESSAGE_SIZES_NB];
        static PMsgData* smMsgPoolHead[MESSAGE_SIZES_NB];
        static int smMsgPoolCount[MESSAGE_SIZES_NB];
        static int smMsgCount; //Used to trace unreleased messages with CheckMsgCount()

        uint8_t mPoolId;
        uint16_t mMaxSize;
        PMsgData* mData;
        uint16_t mUsedSize;
        uint16_t mNextByteOffset;

        void GetMsgBuffer(uint16_t nRequestedSize = 0); // the requested size is just a hint to avoid internal reaffectation of buffer
        void ReleaseMsgBuffer();
        void CheckAndExtend(uint16_t nRequestedSize);  // This is SIZE checked, not max OFFSET
        inline void UpdateUsedSize() { if (mNextByteOffset > mUsedSize) mUsedSize = mNextByteOffset; }

    public:
        static void CheckMsgCount(); //To be used in a place where no new message should remain between calls

        PMessage(uint16_t nRequestedSize = 0); // max size will be extended as needed in later write accesses (up to max configured size)
        PMessage(PMessage& nMessage); // creates a (size optimized, offset reset) copy of nMessage
        inline ~PMessage() { ReleaseMsgBuffer(); --smMsgCount; }

        void SetNextByteOffset(uint16_t nPos);
        inline void IncreaseNextByteOffset(uint16_t nIncrement) { SetNextByteOffset(mNextByteOffset + nIncrement); }
        inline void SetNextByteAtEnd() { mNextByteOffset = mUsedSize; }
        inline uint16_t GetNextByteOffset() { return mNextByteOffset; }
        void ForceSize(uint16_t nUsedSize);
        inline uint16_t GetSize() { return mUsedSize; }
        inline uint16_t GetMaxSize() { return mMaxSize; }
        inline bool EOM() {return (mNextByteOffset >= mUsedSize);} // End Of Message

      // Writing methods
        uint8_t* GetMessageDataPointer(uint16_t nUsedSize); // extends buffer as needed by nUsedSize, and sets UsedSize at min nUsedSize
        PMessage& Fill(uint8_t Value = 0, uint16_t StartOffset = 0, uint16_t FillSize = 0); // !!! Does NOT update UsedSize, fills only up to current maxSize
		inline PMessage& Reset() { mNextByteOffset = mUsedSize = 0; return *this; }
		inline PMessage& Clear() { return this->Fill(0).Reset(); }
        PMessage& Write(const void* nData, uint16_t nLength);
        PMessage& operator << (PMessage& nMessage);
        PMessage& operator << (const char* nString); //for null terminated string ! Copies includes ending \0
        inline PMessage& operator << (std::string& nString) { return (*this << nString.c_str()); }
        PMessage& operator << (uint8_t nU8);
        inline PMessage& operator << (char nChar) { return (*this << (uint8_t) nChar);}
        PMessage& operator << (uint16_t nU16);
        PMessage& operator << (uint32_t nU32);
        PMessage& operator << (float nF32);

      // Mixt methods

        //The next 3 methods do NOT update NextByteOffset, but DO increase message size (UsedSize ans MaxSize) as needed by nOffset.
        uint8_t& U8Data(uint16_t nOffset);
        uint16_t& U16Data(uint16_t nOffset);
        uint32_t& U32Data(uint16_t nOffset);
        float& F32Data(uint16_t nOffset);

        //     *** didn't managed to overload [] operator :-/
        inline uint8_t& operator [] (uint16_t nOffset) { return U8Data(nOffset); }
        //inline u16& operator [] (u16 nOffset) { return U16Data(nOffset); }
        //u32& operator [] (u16 nOffset);

        // Really makes a different message instance, with all data copied (no data shared)
        PMessage& operator = (PMessage& nMessage);

      // Reading methods
        // ChunkNumber count from 0, return NULL for empty chunk (ie StartOffset is over UsedSize). NextByteOffset NOT updated
        PMessage* GetChunk(uint16_t StartOffset, uint16_t ChunkSize, uint16_t ChunkNumber = 0);

        // Return pointer to the START of message data.
        inline uint8_t const* GetMessageData() { return mData->mBuffer; }

        //Following methods do NOT extend message or Used, and return 0/empty string if over UsedSize
        PMessage& operator >> (std::string& nString); //read up to null or EOM
        PMessage& operator >> (uint8_t& nU8);
        inline PMessage& operator >> (char& nChar) { return (*this >> (uint8_t&) nChar);}
        PMessage& operator >> (uint16_t& nU16);
        PMessage& operator >> (uint32_t& nU32);
        PMessage& operator >> (float& nF32);

        // info/debug methods
        static void ListPools();
        static void DumpPools();
        void Dump();
        void DumpHead(char* nComment = "");
};
