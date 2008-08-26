/*
	TinNS (TinNS is not a Neocron Server)
	vfs_viewer.c - NC virutal file system decompression/browsing tool
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
	vfs_viewer.c

	Authors:
	- Akiko <akiko@gmx.org>

	ToDo:
	- remove memory leaks, a lot of memory gets reserved but not freed
	- adding decompressing
	- adding file selection for decompressing
	- adding a nice gui (kde/ncurses)

	MODIFIED: 09 Dec 2005 Akiko
	REASON: - initial release
		- just a output fix
*/

#include <stdio.h>
#include <stdlib.h>

typedef unsigned int u32;
typedef unsigned char u8;

int main(int argc, char *argv[]) {
	// structure of the file informtion inside the virtual FS
	struct t_file_header {
		u32 id;
		u32 offset;
		u32 csize;
		u32 dsize;
		u32 len_name;
		u8 *name;
	};
	// structure of the virtual FS
	struct t_vfs_header {
		u32 id;
		u32 file_count;
		struct t_file_header *files;
		u8 *data;
	};

	if (argc == 2) {
		FILE *fd;
		struct t_vfs_header vfs_file;

		fd = fopen(argv[1], "rb");

		// reading the vfs header including the amount of files in the virtual FS
		fread(&vfs_file.id, sizeof(u32), 1, fd);
		fread(&vfs_file.file_count, sizeof(u32), 1, fd);

		struct t_file_header files[vfs_file.file_count];

		// reading all information from the vfs into an array and print some of them
		u32 i;
		for (i = 0; i < vfs_file.file_count; i++) {
			fread(&files[i].id, sizeof(u32), 1, fd);
			fread(&files[i].offset, sizeof(u32), 1, fd);
			fread(&files[i].csize, sizeof(u32), 1, fd);
			fread(&files[i].dsize, sizeof(u32), 1 ,fd);
			fread(&files[i].len_name, sizeof(u32), 1, fd);

			files[i].name = (u8 *)malloc(sizeof(u8)*files[i].len_name);
			fread(files[i].name, sizeof(u8), files[i].len_name, fd);
			//fseek(fd, files[i].len_name, SEEK_CUR);

			printf("Filenumber %4i: %50s (%7i/%7i) ID-%2i\n", i, files[i].name, files[i].csize, files[i].dsize, files[i].id);
		}
		
		fclose(fd);
	} else {
		printf("Usage: vfs_viewer <nc1 vfs file>\n");
		exit(1);
	}

	exit(0);
}

