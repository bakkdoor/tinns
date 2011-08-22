/*
	TinNS (TinNS is not a Neocron Server)
	pak_decompress - pak file decompression tool
	Copyright (C) 2005 Akiko <akiko@gmx.org>

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
	pak_decompress - a decompression tool for single pak files

	MODIFIED: 23 Sep 2005 Akiko
	REASON: - started this tool
	MODIFIED: 26 Sep 2005 Akiko
	REASON:	- added printing of some information
	MODIFIED: 26 Oct 2005 Akiko
	REASON: - fixed an issue belonging to portability
	MODIFIED: 06 Dec 2005 Akiko
	REASON: - compression ration fixed
	MODIFIED: 09 Dec 2005 Akiko
	REASON: - fixed typo
		- compression ratio
	MODIFIED: 21 Dec 2006 Namikon
	REASON: - Added errorcheck for in/output file
		- Added check for NC and normal zLib files
	MODIFIED: 15 Jun 2009 Akiko
	REASON: - fixed warning

	TODO:	- time needed for decompression
		- ideas for a compression tool
*/

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <zlib.h>

#define CHUNK 262144

int inf(FILE *source, FILE *dest, bool details)
{
	int ret;
	char check;
	unsigned have;
	z_stream strm;
	unsigned char in[CHUNK];
	unsigned char out[CHUNK];

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;
	strm.avail_in = 0;
	strm.next_in = Z_NULL;
	ret = inflateInit(&strm);

	if (ret != Z_OK)
        	return(ret);

	check = fgetc(source);
	if(check == 'x')
	{
		if(details == true) std::cout << "Found zLibfile" << std::endl;
		fseek(source, 0, SEEK_SET);
	}
	else
	{
		fseek(source, 16, SEEK_SET);
		check = fgetc(source);
		if(check == 'x')
		{
			if(details == true) std::cout << "Found Neocron file" << std::endl;
			fseek(source, 16, SEEK_SET);
		}
		else
		{
			if(details == true) std::cout << "Error: No compatible file!" << std::endl;
			return -3;
		}
	}

	do {
		strm.avail_in = fread(in, 1, CHUNK, source);

		if (ferror(source)) {
			(void)inflateEnd(&strm);

			return(Z_ERRNO);
		}

		if (strm.avail_in == 0)
			break;
		strm.next_in = in;

		do {
			strm.avail_out = CHUNK;
			strm.next_out = out;
			ret = inflate(&strm, Z_NO_FLUSH);
			assert(ret != Z_STREAM_ERROR);

			switch(ret) {
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					return(ret);
				}

				have = CHUNK - strm.avail_out;

				if (fwrite(out, 1, have, dest) != have || ferror(dest)) {
					(void)inflateEnd(&strm);

				return(Z_ERRNO);
			}
		} while (strm.avail_out == 0);

		assert(strm.avail_in == 0);
	} while (ret != Z_STREAM_END);

	(void)inflateEnd(&strm);
	
	return(ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR);
}

void zerr(int ret)
{
	fputs("zpipe: ", stderr);

	switch(ret) {
		case Z_ERRNO:
			if (ferror(stdin))
				fputs("error reading stdin\n", stderr);
			if (ferror(stdout))
				fputs("error writing stdout\n", stderr);
			break;
		case Z_STREAM_ERROR:
			fputs("invalid compression level\n", stderr);
			break;
		case Z_DATA_ERROR:
			fputs("invalid or incomplete deflate data\n", stderr);
			break;
		case Z_MEM_ERROR:
			fputs("out of memory\n", stderr);
			break;
		case Z_VERSION_ERROR:
			fputs("zlib version mismatch!\n", stderr);
	}
}

int main(int argc, char **argv) {
	int ret;
	long inSize;
	long outSize;
	bool details = false;
	FILE *inFile;
	FILE *outFile;
	std::string src;
	std::string dst;

	if(argc == 2) {
		src = argv[1];
		dst = strcat(argv[1], ".decompressed");
	}
	else if(argc > 2 && strcmp(argv[1], argv[2])) {
		src = argv[1];
		dst = argv[2];
	}
	else {
		std::cout << "Usage: pak_decompress source <dest != source> <detailedoutput 1/0>" << std::endl;
		return(0);
	}
	if(argc == 4)
	{
		if(argv[3][0] == '1') details = true;
		else if(argv[3][0] == '0') details = false;
	}

	inFile = fopen(src.c_str(), "rb");
	outFile = fopen(dst.c_str(), "wb");

	if(inFile == NULL)
	{
		std::cout << "Cannot open InFile" << std::endl;
		return(-1);
	}
	if(outFile == NULL)
	{
		std::cout << "Cannot open OutFile" << std::endl;
		return(-2);
	}
	ret = inf(inFile, outFile, details);

	/* just to be sure - we got to the end */
	fseek(inFile, 0, SEEK_END);
	fseek(outFile, 0, SEEK_END);
	inSize = ftell(inFile);
	outSize = ftell(outFile);
	
	fclose(outFile);
	fclose(inFile);
	
	if(ret == Z_OK && details == true)
	{
		std::cout << "bytes read:        " << inSize << std::endl;
		std::cout << "bytes written:     " << outSize << std::endl;
		std::cout << "compression ratio: " << (100-((float)inSize/(float)outSize*100)) << "%" << std::endl;
	}
	else if(ret == Z_OK && details == false)
	{
		std::cout << "[OK]    " << src.c_str() << std::endl;
	}
	else if(ret != Z_OK && details == false)
	{
		std::cout << "[ERROR] " << src.c_str() << std::endl;
	}

	return(ret);
}


