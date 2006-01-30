#ifndef FILESYSTEM_H
#define FILESYSTEM_H

class PFile 
{
	friend class PFileSystem;
	private :
		std::vector<u8> mBuffer;
		u32 mDataSize;
		u32 mDataOffs;
		bool ReadData(std::FILE *F, u32 Size);
		bool ReadUnpakData(std::FILE *F, u32 Size, u32 UncSize);
	public :
		PFile();
		~PFile();
		inline bool Eof() { return mDataOffs>=mDataSize; }
		int Read(void *Dest, u32 DestSize);
		void Seek(u32 Offset);
		std::string ReadString();
		inline u32 GetSize() const { return mDataSize; }
};

#pragma pack(push, 1)
//#pragma pack(1)
struct PPakHeader
{
	int mID;
	int mNumFiles;
};

struct PPakFileHeader
{
	int mUnknown0;
	int mOffset;
	int mCompressedSize;
	int mUncompressedSize;
	int mNameLen;	// including 0
	char *mFilename;
};
#pragma pack(pop)

class PFileSystem
{
	private :
		typedef std::map<std::string, PPakFileHeader*> PPakFileList;
		typedef std::map<std::string, PPakFileList*> PPakFiles;
		PPakFiles mPaks;
		PPakFileList *CachePak(const std::string &Pak, std::FILE *F);
	public :
		PFileSystem();
		~PFileSystem();
		PFile *Open(const std::string &Package, const char *File);
		bool Close(PFile *File);
};

#endif

