#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define YEAR_OFFSET 1980

/* direntry.name */
#define	ENTRY_EMPTY	0x00		/* Entry is available and no subsequent entry is in use. */
#define	ENTRY_E5		0x05		/* Initial character is actually 0xE5*/
#define ENTRY_DOT   0x2E        /* 'Dot' entry; either "." or ".." (since MS-DOS 1.40 and PC DOS 2.0) */
#define	ENTRY_DELETED	0xe5	/* Entry has been previously erased and/or is available */

/* direntry.attributes */
#define	ENTRY_NORMAL	0x00		/* normal file */
#define	ENTRY_READONLY	0x01	/* file is readonly */
#define	ENTRY_HIDDEN	0x02		/* file is hidden */
#define	ENTRY_SYSTEM	0x04		/* file is a system file */
#define	ENTRY_VOLUME	0x08		/* volume label */
#define	ENTRY_DIRECTORY	0x10	/* directory name */
#define	ENTRY_ARCHIVE	0x20	/* 	Archive. */
#define ENTRY_DEVICE 0x40 /*Device, must not be changed by disk tools*/


/*
 * Structure of a directory entry.
 */
struct directoryEntry {
	uint8_t	name[8];	/* Short file name (padded with spaces) */
	uint8_t	extension[3];	/* Short file extension (padded with spaces) */
	uint8_t	attributes;	/* file attributes */
	uint8_t	dontCare1;
	uint8_t	dontCare2;
	uint8_t	creatTime[2];	/* create time */
	uint8_t	creatDate[2];	/* create date */
	uint8_t	dontCare3[2];
	uint8_t	dontCare4[2];
	uint8_t	modifiedTime[2];	/* Last modified time  */
	uint8_t	modifiedDate[2];	/* last modified date */
	uint8_t	startClusters[2]; /* starting cluster of file */
	uint8_t	size[4];	/* size of file in bytes */
};

typedef union
{
	struct
	{
		uint16_t day : 5;
		uint16_t month : 4;
		uint16_t year : 7;
	};
	struct
	{
		uint8_t date[2];
	};
} Date;

typedef union
{
	struct
	{
		uint16_t seconds : 5;
		uint16_t minutes : 6;
		uint16_t hours : 5;
	};
	struct
	{
		uint8_t time[2];
	};
} Time;

/*get size of file in KB*/
int GetSizeofFile(struct directoryEntry* entry)
{
	uint32_t bytes = (entry->size[0]) | ((entry->size[0]) << 8) | ((entry->size[1]) << 16) | ((entry->size[1]) << 24);
	return (bytes / 1024);
}

/* GetName retrieves the filename from a directory entry */
void GetName(char* fullname, struct directoryEntry* entry)
{
	char name[9];
	char extension[4];
	int i;

	name[8] = ' ';
	extension[3] = ' ';
	memcpy(name, &(entry->name[0]), 8);
	memcpy(extension, entry->extension, 3);

	/* names are space padded - remove the padding */
	i = 8;
	while ((i > 0) && (name[i] == ' '))
	{
		name[i] = '\0';
		i--;
	}

	/* extensions are space padded - remove the padding */
	i = 3;
	while ((i > 0) && (extension[i] == ' '))
	{
		extension[i] = '\0';
		i--;
	}

	strcpy(fullname, name);

	/* append the extension if it's not a directory */
	if (entry->attributes != ENTRY_DIRECTORY) {
		strcat(fullname, ".");
		strcat(fullname, extension);
	}
}

void GetFileType(char* extension, struct directoryEntry* entry)
{
	int i;
	extension[3] = ' ';
	memcpy(extension, entry->extension, 3);

	/* extensions are space padded - remove the padding */
	i = 3;
	while ((i > 0) && (extension[i] == ' '))
	{
		extension[i] = '\0';
		i--;
	}
}

int main()
{
	char name[20];
	char extention[20];
	int i;
	int size;
	Date date;
	Time time;
	struct directoryEntry entry = { 0x53, 0x41, 0x4D, 0x50, 0x4C, 0x45, 0x31, 0x20,\
		0x54, 0x58, 0x54,\
		0x00, \
		0x10, 0xA9, \
		0xBA, 0x78, \
		0xBA, 0x38, \
		0x00, 0x00, 0x00, 0x00, \
		0xE3, 0x78,\
		0xBA, 0x38,\
		0x03, 0x00,\
		0x33, 0x00, 0x00, 0x00 };

	printf("Name_________________Date modified__________Type__________Size__________\n");

	GetName(name, &entry);
	GetFileType(extention, &entry);
	size = GetSizeofFile(&entry);
	date.date[0] = entry.modifiedDate[0];
	date.date[1] = entry.modifiedDate[1];

	printf("%-20s %2d/%2d/%4d %15s %12d KB\n", name, date.day, date.month, date.year + YEAR_OFFSET, extention, size);


	return 0;
}