#include "FAT.h"
#include "View.h"
#include <stdio.h>
#include <stdlib.h>

enum State
{
	_ShowFolder,
	_GetEntry,
	_PrintfFile,
	_EXIT
};

int main()
{
	unsigned int i;
	unsigned int currentFolder = 0;
	int state = _ShowFolder;

	DirectoryEntry entry;
	FatInit("floppy.img");

	while (state != _EXIT)
	{
		system("cls");
		switch (state)
		{
		case _ShowFolder:
			i = ShowFolder(currentFolder);
			if (i == 0)
			{
				state = _EXIT;
			}
			else
			{
				state = _GetEntry;
			}

			break;

		case _GetEntry:
			GetEntry(&entry, i, currentFolder);
			if (entry.attributes & ENTRY_DIRECTORY)
			{
				state = _ShowFolder;
				currentFolder = ReadNumber(2, entry.startClusters);
			}
			else
			{
				state = _PrintfFile;
			}

			break;

		case _PrintfFile:
			PrintFile(&entry);
			system("pause");
			state = _ShowFolder;
			break;
		default:
			break;
		}
	}

	FatDeInit();
}
