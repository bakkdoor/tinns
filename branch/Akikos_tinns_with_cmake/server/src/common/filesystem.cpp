#include "common/filesystem.h"

#include <sstream>
#include <cstring>
#include <zlib.h>
#include "common/console.h"

const int8_t DELIM = '/';

PFile::PFile()
{
	mDataSize=0;
	mDataOffs=0;
}

PFile::~PFile()
{
}

bool PFile::ReadData(FILE *F, uint32_t Size)
{
	mBuffer.reserve(Size);
	std::fread(&mBuffer[0], 1, Size, F);
	mDataSize = Size;
	return true;
}

bool PFile::ReadUnpakData(FILE *F, uint32_t Size, uint32_t UncSize)
{
	std::vector<int8_t> temp;
	temp.reserve(Size);
	mBuffer.reserve(UncSize);

	std::fread(&temp[0], 1, Size, F);

	unsigned long us=UncSize;
	unsigned long s=Size;
	uncompress(reinterpret_cast<Bytef *>(&mBuffer[0]), &us, reinterpret_cast<const Bytef *>(&temp[0]), s);
	mDataSize=us;
	return true;
}

int PFile::Read(void *Dest, uint32_t DestSize)
{
	int m = std::min(mDataSize-mDataOffs, DestSize);
	if (m <= 0)
	  return 0;
	memcpy(Dest, &mBuffer[mDataOffs], m);
	mDataOffs+=m;
	return m;
}

void PFile::Seek(uint32_t Offset)
{
	mDataOffs = std::min(mDataSize-1, Offset);
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

PFileSystem::PPakFileList* PFileSystem::CachePak(const std::string &Pak, FILE *F)
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
//Console->Print("%s: %i files registered", Pak.c_str(), header.mNumFiles);
	} else
	{
		Console->Print("%s: invalid pakfile", Pak.c_str());
	}
	std::fseek(F, off, SEEK_SET);
	return Result;
}

void PFileSystem::ClearCache()
{
  PFileSystem::PPakFiles::iterator nPak;
  PFileSystem::PPakFileList::iterator nFList;
  PPakFileHeader* FHeader;
  PFileSystem::PPakFileList* FList;
  
  for( nPak = mPaks.begin(); nPak != mPaks.end(); nPak++)
  {
    FList = nPak->second;
    for( nFList = FList->begin(); nFList != FList->end(); nFList++)
    {
      FHeader = nFList->second;
      FList->erase(nFList);
      delete[] FHeader->mFilename;
      delete FHeader;
    }
    mPaks.erase(nPak);
    delete FList;
  }
}

void splitpath(const std::string& file, std::string& path, std::string& name, std::string& ext)
{
	unsigned long pos = file.rfind(DELIM);

	if (pos == std::string::npos)
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
	if (pos == std::string::npos)
	{
		ext = "";
	}
	else
	{
		ext = name.substr(pos + 1);
		name = name.substr(0, pos);
	}
}

PFile *PFileSystem::Open(const std::string& Package, const char *File, std::string BasePath)
{
	      std::string name = "";
        std::string ext = "";
        std::string path = "";
        splitpath(File, path, name, ext);

        std::string pak2;
        std::string name2;

        if ((BasePath == ".") || (BasePath == "./"))
        {
          BasePath = "";
        }
        else
        {
          if (BasePath.substr(0, 2) == "./")
          {
            BasePath = BasePath.substr(2, BasePath.length() -2);
          }
          if ( BasePath[BasePath.length()-1] != '/' )
          {
            BasePath += '/';
          }
        }
//Console->Print("Basepath:%s", BasePath.c_str());
     
        std::string pak = Package;
        if(pak=="")
        {
           pak=path;
           if (path.substr(0, 2) == "./")
           {
              path = path.substr(2, path.length() -2);
           }
           unsigned long pos = path.find(DELIM);
          	if (pos == std::string::npos)
          	{
          		pak2 = path;
          		name2 = name;
          	}
          	else
          	{
          		pak2 = path.substr(0, pos);
          		name2 = path.substr(pos + 1) + '\\' + name;
          		pos = name2.find(DELIM);
          		while (pos != std::string::npos)
          		{
          		  name2[pos] = '\\';
          		  pos = name2.find(DELIM);
          		}
          	}
        }          

        std::stringstream package;
        //package << pak << ".pak" << '\0';
        package << BasePath << pak2 << ".pak" << '\0';
        std::stringstream fname;
        fname << BasePath << pak << '/' << name << "." << ext << '\0';
        std::stringstream pakname;
        pakname << BasePath << pak << "/pak_" << name << "." << ext << '\0';

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
//else
//  Console->Print("File not found:%s", fname.str().c_str());
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
//else
//  Console->Print("Pak_ not found:%s", pakname.str().c_str());
  
//Console->Print("Serching package %s, file %s.%s", package.str().c_str(), name2.c_str(), ext.c_str());
        f = std::fopen(package.str().c_str(), "rb");
        if(f)
        {
                //std::string filename = name;
                std::string filename = name2 + "." + ext;
		            //filename.append(ext);

                PPakFileHeader *file = 0;
                PPakFileList *list = CachePak(std::string(package.str()), f);
                for(PPakFileList::iterator i=list->begin(); i!=list->end(); i++)
                {
                        PPakFileHeader *h = i->second;
                        if(!strcasecmp(i->first.c_str(), filename.c_str()))
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
//Console->Print("%s, %s: file found", package.str().c_str(), filename.c_str());
                }
                else
                {
                        if(!list)
                                //Console->Print("%s, %s: pak error", Package.c_str(), File);
                                Console->Print("%s, %s: pak error", package.str().c_str(), File);
//else
//if(!file)
//Console->Print("%s, %s: file not found", Package.c_str(), File);
//  Console->Print("%s, %s: file not found", package.str().c_str(), filename.c_str());
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

