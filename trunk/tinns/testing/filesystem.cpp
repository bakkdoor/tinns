#include "tinns.h"

/*
	implements file access semantics for Neocron .pak files
	supports both single packed files and file archives

	how neocron files are accessed:
	- if path/filename.ext exists, the file is opened
	- (else) if path/pak_filename.ext exists, the file is opened
	- (else) if an archive named path.pak exists, filename.ext is opened from the archive

	MODIFIED: 31 August 2005 Akiko
	REASON: - modified the path handling of the open function
*/

PFile::PFile()
{
	mDataSize=0;
	mDataOffs=0;
}

PFile::~PFile()
{
}

bool PFile::ReadData(std::FILE *F, u32 Size)
{
	mBuffer.reserve(Size);
	std::fread(&mBuffer[0], 1, Size, F);
	mDataSize = Size;
	return true;
}

bool PFile::ReadUnpakData(std::FILE *F, u32 Size, u32 UncSize)
{
	std::vector<u8> temp;
	temp.reserve(Size);
	mBuffer.reserve(UncSize);

	std::fread(&temp[0], 1, Size, F);

	unsigned long us=UncSize;
	unsigned long s=Size;
	uncompress(&mBuffer[0], &us, &temp[0], s);
	mDataSize=us;
	return true;
}

int PFile::Read(void *Dest, u32 DestSize)
{
	int m = min(mDataSize-mDataOffs, DestSize);
	memcpy(Dest, &mBuffer[mDataOffs], m);
	mDataOffs+=m;
	return m;
}

void PFile::Seek(u32 Offset)
{
	mDataOffs = min(mDataSize-1, Offset);	
}

std::string PFile::ReadString()
{
	int l=0;
	int s=mDataOffs;
	char last=0;
	while(mDataOffs < mDataSize && last!='\n')
	{
		last=mBuffer[mDataOffs++];
		l++;
	}
	
	char *str = new char[l+1];
	std::memcpy(str, &mBuffer[s], l);
	str[l]=0;
	
	std::string Result = str;
	delete str;
	return Result;
}

// ---------------

PFileSystem::PFileSystem()
{
}

PFileSystem::~PFileSystem()
{
	for(PPakFiles::iterator i=mPaks.begin(); i!=mPaks.end(); i++)
	{
		PPakFileList *list = i->second;
		for(PPakFileList::iterator j=list->begin(); j!=list->end(); j++)
			delete j->second;
		delete i->second;
	}
}

PFileSystem::PPakFileList* PFileSystem::CachePak(const std::string &Pak, std::FILE *F)
{
	PPakFiles::iterator n = mPaks.find(Pak);
	if(n != mPaks.end())
		return n->second;
		
	if(!F)
		return 0;

	PPakFileList *Result = 0;

	int off = std::ftell(F);
	std::fseek(F, 0, SEEK_SET);
	PPakHeader header;
	std::fread(&header, 1, sizeof(header), F);
	if(header.mID==0x3d458cde && header.mNumFiles > 0)
	{
		Result = new PPakFileList();
		for(int i=0; i<header.mNumFiles; i++)
		{
			PPakFileHeader *h = new PPakFileHeader();
			std::fread(h, 1, 20, F);
			h->mFilename = new char[h->mNameLen+1];
			std::fread(h->mFilename, 1, h->mNameLen, F);
			Result->insert(std::make_pair(h->mFilename, h));
		}
		mPaks.insert(std::make_pair(Pak, Result));
		Console->Print("%s: %i files registered", Pak.c_str(), header.mNumFiles);
	} else
	{
		Console->Print("%s: invalid pakfile", Pak.c_str());
	}
	std::fseek(F, off, SEEK_SET);
	return Result;
}

void splitpath(const string &file, string &path, string &name, string &ext)
{
	const s8 DELIM = '/';
	unsigned long pos = file.rfind(DELIM);

	if (pos == string::npos)
	{
		path = "";
		name = file;
	}
	else
	{
		path = file.substr(0, pos);
		name = file.substr(pos + 1);
	}

	pos = name.rfind('.');
	if (pos == string::npos)
	{
		ext = "";
	}
	else
	{
		ext = name.substr(pos + 1);
		name = name.substr(0, pos);
	}
}

PFile *PFileSystem::Open(const std::string &Package, const char *File)
{
	std::string name = "";
        std::string ext = "";
        std::string path = "";
        splitpath(File, path, name, ext);

        std::string pak = Package;
        if(pak=="")
                pak=path;

        std::stringstream package;
        package << pak << ".pak" << '\0';
        std::stringstream fname;
        fname << pak << '/' << name << "." << ext << '\0';
        std::stringstream pakname;
        pakname << pak << "/pak_" << name << "." << ext << '\0';

        std::FILE *f = std::fopen(fname.str().c_str(), "rb");
        if(f)
        {
                PFile *Result = new PFile();
                std::fseek(f, 0, SEEK_END);
                int s = std::ftell(f);
                std::fseek(f, 0, SEEK_SET);
                Result->ReadData(f, s);
                std::fclose(f);
                return Result;
        }

        f = std::fopen(pakname.str().c_str(), "rb");
        if(f)
        {
                PFile *Result = new PFile();
                std::fseek(f, 0, SEEK_END);
                int s = std::ftell(f)-16;
                std::fseek(f, 12, SEEK_SET);
                int os;
                std::fread(&os, 1, 4, f);
                Result->ReadUnpakData(f, s, os);
                std::fclose(f);
                return Result;
        }

        f = std::fopen(package.str().c_str(), "rb");
        if(f)
        {
                std::string filename = name;
		filename.append(ext);

                PPakFileHeader *file = 0;
                PPakFileList *list = CachePak(std::string(package.str()), f);
                for(PPakFileList::iterator i=list->begin(); i!=list->end(); i++)
                {
                        PPakFileHeader *h = i->second;
                        if(i->first == filename)
                        {
                                file = h;
                                break;
                        }
                }
                PFile *Result = 0;
                if(list && file)
                {
                        Result = new PFile();
                        std::fseek(f, file->mOffset, SEEK_SET);
                        Result->ReadUnpakData(f, file->mCompressedSize, file->mUncompressedSize);
                } else
                {
                        if(!list)
                                Console->Print("%s, %s: pak error", Package.c_str(), File);
                        else
                        if(!file)
                                Console->Print("%s, %s: file not found", Package.c_str(), File);
                }

                std::fclose(f);
                return Result;
        }

        return 0;
}

bool PFileSystem::Close(PFile *File)
{
	delete File;
	return true;
}

