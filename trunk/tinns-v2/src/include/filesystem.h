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
	filesystem.h

	MODIFIED: 25 Dec 2005 Namikon
	REASON: - Added GPL

  MODIFIED: 08 Oct 2006 Hammag
  REASON: - added ClearCache() methode to clear pak cache when .pak access is not used anymore

*/

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
		void ClearCache();
};

#endif

