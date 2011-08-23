#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <map>

class PFile {
	friend class PFileSystem;
private:
	std::vector<int8_t> mBuffer;
	uint32_t mDataSize;
	uint32_t mDataOffs;
	bool ReadData(FILE *F, uint32_t Size);
	bool ReadUnpakData(FILE *F, uint32_t Size, uint32_t UncSize);

public:
	PFile();
	~PFile();
	inline bool Eof() { return mDataOffs>=mDataSize; }
	int Read(void *Dest, uint32_t DestSize);
	void Seek(uint32_t Offset);
	std::string ReadString();
	inline uint32_t GetSize() const { return mDataSize; }
};

#pragma pack(push, 1)
//#pragma pack(1)
struct PPakHeader {
	int mID;
	int mNumFiles;
};

struct PPakFileHeader {
	int mUnknown0;
	int mOffset;
	int mCompressedSize;
	int mUncompressedSize;
	int mNameLen;	// including 0
	char *mFilename;
};
#pragma pack(pop)

class PFileSystem {
private:
	typedef std::map<std::string, PPakFileHeader*> PPakFileList;
	typedef std::map<std::string, PPakFileList*> PPakFiles;
	PPakFiles mPaks;
	PPakFileList *CachePak(const std::string &Pak, FILE *F);

public:
	PFileSystem();
	~PFileSystem();
	PFile *Open(const std::string &Package, const char *File, std::string BasePath);
	bool Close(PFile *File);
	void ClearCache();
};
