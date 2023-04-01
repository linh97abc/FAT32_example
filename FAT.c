#include "FAT.h"
#include "HAL.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define DAY_SHIFT 0
#define MONTH_SHIFT 5
#define YEAR_SHIFT 9
#define DAY_MASK (0x1F << DAY_SHIFT)
#define MONTH_MASK (0X0F << MONTH_SHIFT)
#define YEAR_MASK (0X7F << YEAR_SHIFT)

#define SECONDS_SHIFT 0
#define MINUTES_SHIFT 5
#define HOURS_SHIFT 11
#define SECONDS_MASK (0x1F << SECONDS_SHIFT)
#define MINUTES_MASK (0X1F << MINUTES_SHIFT)
#define HOURS_MASK (0X7F << HOURS_SHIFT)
 /*******************************************************************************
  * Prototypes
  ******************************************************************************/

  /*******************************************************************************
   * Variables
   ******************************************************************************/
BIOSParam g_biosParam;

/*******************************************************************************
 * Code
 ******************************************************************************/

 /*!
  * @brief <get size of file>
  *
  * @param entry <Pointer to a entry object>.
  *
  * @return <File size in bytes>.
  */
unsigned int GetSizeofFile(DirectoryEntry* entry)
{
	uint32_t bytes = ReadNumber(4, entry->size);
	return (bytes);
}

/*!
 * @brief
 * <Get filename from a directory entry
 * and stores them in the block of memory specified by fullname>
 *
 * @param fullname <Pointer to a block of memory with a size of at least 14 bytes>.
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void GetName(char* fullname, DirectoryEntry* entry)
{
	char name[9];
	char extension[4];
	int i;

	name[8] = ' ';
	memcpy(name, &(entry->name[0]), 8);

	/* names are space padded - remove the padding */
	i = 8;
	while ((i > 0) && (name[i] == ' '))
	{
		name[i] = '\0';
		i--;
	}

	GetFileType(extension, entry);

	strcpy(fullname, name);

	/* append the extension if it's not a directory */
	if (!(entry->attributes & ENTRY_DIRECTORY))
	{
		strcat(fullname, ".");
		strcat(fullname, extension);
	}
}

/*!
 * @brief
 * <Get short file extension from a directory entry
 * and stores them in the block of memory specified by extension>
 *
 * @param extension <Pointer to a block of memory with a size of at least 4 bytes>.
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void GetFileType(char* extension, DirectoryEntry* entry)
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

/*!
 * @brief
 * <Get last modified date from a directory entry
 * and stores them in the block of memory specified by modifiedDate>
 *
 * @param modifiedDate <Pointer to a Date object>.
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void GetFileModifiedDate(Date* modifiedDate, DirectoryEntry* entry)
{
	uint16_t date = (entry->modifiedDate[0]) | ((entry->modifiedDate[1]) << 8);
	modifiedDate->day = (date & DAY_MASK) >> DAY_SHIFT;
	modifiedDate->month = (date & MONTH_MASK) >> MONTH_SHIFT;
	modifiedDate->year = (date & YEAR_MASK) >> YEAR_SHIFT;
}

/*!
 * @brief
 * <Get last modified time from a directory entry
 * and stores them in the block of memory specified by modifiedDate>
 *
 * @param modifiedTime <Pointer to a Time object>.
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void GetFileModifiedTime(Time* modifiedTime, DirectoryEntry* entry)
{
	uint16_t time = (entry->modifiedTime[0]) | ((entry->modifiedTime[1]) << 8);
	modifiedTime->seconds = (time & SECONDS_MASK) >> SECONDS_SHIFT;
	modifiedTime->minutes = (time & MINUTES_MASK) >> MINUTES_SHIFT;
	modifiedTime->hours = (time & HOURS_MASK) >> HOURS_SHIFT;
}

/*!
 * @brief <read value of number _count bytes from the location pointed to by _source>
 *
 * @param _count <Number of bytes to read>.
 * @param _source <Pointer to the source of data to be read>.
 *
 * @return <value of number>.
 */
uint64_t ReadNumber(int _count, const void* _source)
{
	uint64_t ans = 0;
	uint8_t* source = (uint8_t*)_source;
	int i;
	for (i = _count - 1; i >= 0; i--)
	{
		ans = ans << 8;
		ans = ans | source[i];
	}

	return ans;
}

/*!
 * @brief <Get start sector in root directory>
 *
 * @param none
 *
 * @return <Position of start sector in root directory>.
 */
unsigned int GetStartSectorRoot()
{
	unsigned int index;
	unsigned int numFAT = g_biosParam.numFAT;
	unsigned int sectorPerFAT = ReadNumber(2, g_biosParam.sectorPerFAT);
	unsigned int numReservedSector = ReadNumber(2, g_biosParam.numReservedSector);

	index = numReservedSector + numFAT * sectorPerFAT;

	return index;
}

/*!
 * @brief <Opens the file whose info is specified in the parameter entry>
 *
 * @param entry <Pointer to a entry object>.
 *
 * @return <a pointer to a File object>.
 */
File* OpenFile(DirectoryEntry* entry)
{
	File* _file = (File*)malloc(sizeof(File));

	_file->currentByte = 0;
	_file->currentSector = 0;
	_file->startCluster = ReadNumber(2, entry->startClusters);
	_file->currentCluster = _file->startCluster;

	
	return _file;
}

/*!
 * @brief <Close file>
 *
 * @param _file <Pointer to a File object>.
 *
 * @return <none>.
 */
void CloseFile(File* _file)
{
	/*do something*/

	free(_file);
}

/*!
 * @brief <Get number of sectors in root directory>
 *
 * @param <none>.
 *
 * @return <number of sectors in root directory>.
 */
unsigned int SectorPerRoot()
{
	const unsigned int bytePerSector = ReadNumber(2, g_biosParam.bytePerSector);
	const unsigned int maxNumRootEntry = ReadNumber(2, g_biosParam.maxNumRootEntry);
	unsigned int nSectorRoot;

	nSectorRoot = 1 + ((sizeof(DirectoryEntry) * maxNumRootEntry) - 1) / bytePerSector;
	return nSectorRoot;
}

/*!
 * @brief
 * <Reads an array of count elements,
 * each one with a size of size bytes,
 * from the _file and stores them in the block of memory specified by _Buffer>
 *
 * @param _Buffer <Pointer to a block of memory with a size of at least (size*count) bytes>.
 * @param _ElementSize <Size, in bytes, of each element to be read>.
 * @param _ElementCount <Number of elements>
 * @param _file <Pointer to a File object>
 *
 * @return
 * if the end-of-file was reached while reading -> return zero
 * else return non-zero
 */
unsigned int Fread(void* _Buffer, unsigned int _ElementSize, unsigned int _ElementCount, File* _file)
{
	int isNotEOF = 1;

	/* BIOS Parameter */
	const unsigned int numReservedSector = ReadNumber(2, g_biosParam.numReservedSector);
	const unsigned int sectorPerFAT = ReadNumber(2, g_biosParam.sectorPerFAT);
	const unsigned int numFAT = g_biosParam.numFAT;
	const unsigned int sectorPerCluster = g_biosParam.secPerCluster;

	const unsigned int startSectorOfCluster2 = numReservedSector + numFAT * sectorPerFAT + SectorPerRoot();

	uint8_t* buffer = (uint8_t*)_Buffer;
	const unsigned int size = _ElementSize * _ElementCount;
	unsigned int i = 0;

	while ((i < size) && isNotEOF)
	{
		unsigned int sectorStart;
		unsigned int sectorIndex;
		unsigned int currentSectorInCluster = _file->currentSector;
		const uint8_t* sector;

		/**/
		if (_file->currentCluster != EOC)
		{
			sectorStart = startSectorOfCluster2 +
				(_file->currentCluster - 2) * sectorPerCluster;

			sectorIndex = sectorStart + currentSectorInCluster;

			sector = (const uint8_t*)GetSector(sectorIndex);

			while ((i < size) && (_file->currentSector == currentSectorInCluster))
			{
				buffer[i] = sector[_file->currentByte];
				i++;
				Fseek(_file, 1, F_SEEK_CUR);
			}
		}
		else
		{
			while (i < size)
			{
				buffer[i] = 0;
				i++;
			}

			isNotEOF = 0;
		}
	}

	return isNotEOF;
}

/*!
 * @brief <Sets the position indicator to a new position>
 *
 * @param _file <Pointer to a FILE object>.
 * @param _offset <Number of bytes to _offset from _origin>
 * @param _origin <Position used as reference for the _offset>
 * F_SEEK_SET: Beginning of file
 * F_SEEK_CUR: Current position of the file pointer
 *
 * @return <none>
 */
void Fseek(File* _file, unsigned int _offset, int _origin)
{
	const unsigned int bytePerSec = ReadNumber(2, g_biosParam.bytePerSector);
	int i = 0;

	if (_origin == F_SEEK_SET)
	{
		_file->currentByte = 0;
		_file->currentSector = 0;
		_file->startCluster = _file->startCluster;
	}

	for (i = 0; i < _offset; i++)
	{
		if (_file->currentByte < bytePerSec - 1)
		{
			(_file->currentByte)++;
		}
		else
		{
			_file->currentByte = 0;

			if (_file->currentSector < g_biosParam.secPerCluster - 1)
			{
				(_file->currentSector)++;
			}
			else
			{
				_file->currentSector = 0;

				if (_file->currentCluster != EOC)
				{
					_file->currentCluster = GetNextCluster(_file->currentCluster);
				}

			}
		}
	}
}

void FatInit(const char* _imgName)
{
	uint8_t* sector;
	uint8_t* buffer = (uint8_t*)&g_biosParam;
	int i;

	OpenImg(_imgName);
	sector = (uint8_t*)GetSector(0);
	for (i = 0; i < sizeof(BIOSParam); i++)
	{
		buffer[i] = sector[BIOS_PARAM_OFFSET + i];
	}
}

void FatDeInit()
{
	CloseImg();
}

unsigned int GetNextCluster(unsigned int current)
{
	int next;
	const uint8_t* sector;

	/* BIOS Param */
	const unsigned int bytePerSector = ReadNumber(2, g_biosParam.bytePerSector);
	const unsigned int numReservedSector = ReadNumber(2, g_biosParam.numReservedSector);
	const unsigned int secPerCluster = g_biosParam.secPerCluster;

	unsigned int offset_FAT; /* vi tri cua phan tu can doc trong bang FAT */
	unsigned int odd;
	unsigned int offset_sector; /* vi tri cua phan tu can doc trong sector */
	unsigned int byte_l;
	unsigned int byte_h;
	unsigned int offset_sector_FAT; /* vi tri cua sector trong bang FAT*/

	/**/
	odd = (current * 3) % 2;
	offset_FAT = (current * 3) / 2;
	offset_sector_FAT = offset_FAT / bytePerSector;	
	offset_sector = offset_FAT % bytePerSector;

	/* get byte low, byte hight */
	sector = (uint8_t*)GetSector(numReservedSector + offset_sector_FAT);
	byte_l = sector[offset_sector];

	if (offset_sector == bytePerSector - 1)
	{
		sector = (const uint8_t*)GetSector(numReservedSector + offset_sector_FAT + 1);
		byte_h = sector[0];
	}
	else
	{
		byte_h = sector[offset_sector + 1];
	}

	/**/
	if (odd == 1)
	{
		next = (byte_h << 4) | ((byte_l & 0xF0) >> 4);
	}
	else
	{
		next = byte_l | ((byte_h & 0x0F) << 8);
	}

	/**/
	return next;
}

int GetEntry(void* entry, unsigned int entryIndex, unsigned int startCluster)
{
	unsigned int bytePerSec = ReadNumber(2, g_biosParam.bytePerSector);

	unsigned int byteOffset = 0;
	unsigned int SecOffset;
	unsigned int i = 0;

	unsigned int endSector = 0;
	DirectoryEntry* tempEntry;
	uint8_t* sector;

	int isEntryNotFound = 1;

	if (startCluster == 0) /*Root Directory*/
	{
		SecOffset = GetStartSectorRoot();
		endSector = SecOffset + SectorPerRoot();
	}
	else
	{
		const unsigned int numReservedSector = ReadNumber(2, g_biosParam.numReservedSector);
		const unsigned int sectorPerFAT = ReadNumber(2, g_biosParam.sectorPerFAT);
		const unsigned int numFAT = g_biosParam.numFAT;

		const unsigned int sectorPerCluster = g_biosParam.secPerCluster;
		const unsigned int bytePerCluster = sectorPerCluster * bytePerSec;
		SecOffset = numReservedSector +
			numFAT * sectorPerFAT + SectorPerRoot() +
			(startCluster - 2) * sectorPerCluster;
		endSector = SecOffset + g_biosParam.secPerCluster;
	}

	sector = (uint8_t*)GetSector(SecOffset);
	tempEntry = (DirectoryEntry*)sector;

	while (isEntryNotFound && (SecOffset <= endSector))
	{
		if (byteOffset > bytePerSec)
		{
			SecOffset++;
			byteOffset = 0;
			sector = (uint8_t*)GetSector(SecOffset);
			tempEntry = (DirectoryEntry*)sector;
		}
		else
		{
			byteOffset += sizeof(DirectoryEntry);

			if (tempEntry->attributes != ENTRY_NAME) /*length name of file*/
			{
				i++;
			}

			if (i < entryIndex)
			{
				tempEntry++;
				
			}
			else
			{
				int j;
				uint8_t* tempBuffer = (uint8_t*)tempEntry;
				uint8_t* buffer = (uint8_t*)entry;
				for (j = 0; j < sizeof(DirectoryEntry); j++)
				{
					buffer[j] = tempBuffer[j];
				}

				isEntryNotFound = 0;
			}
		}
	}



	return isEntryNotFound;
}
