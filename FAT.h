#ifndef _FAT_H_
#define _FAT_H_

#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BIOS_PARAM_OFFSET 0x00B

/* End of cluster */
#define EOC 0xFFF

#define YEAR_OFFSET 1980

/* directoryEntry.name */
#define ENTRY_EMPTY 0x00   /* Entry is available and no subsequent entry is in use. */
#define ENTRY_E5 0x05      /* Initial character is actually 0xE5*/
#define ENTRY_DOT 0x2E     /* 'Dot' entry; either "." or ".." (since MS-DOS 1.40 and PC DOS 2.0) */
#define ENTRY_DELETED 0xe5 /* Entry has been previously erased and/or is available */

/* directoryEntry.attributes */
#define ENTRY_NORMAL 0x00    /* normal file */
#define ENTRY_READONLY 0x01  /* file is readonly */
#define ENTRY_HIDDEN 0x02    /* file is hidden */
#define ENTRY_SYSTEM 0x04    /* file is a system file */
#define ENTRY_VOLUME 0x08    /* volume label */
#define ENTRY_DIRECTORY 0x10 /* directory name */
#define ENTRY_ARCHIVE 0x20   /* Archive. */
#define ENTRY_DEVICE 0x40    /* Device, must not be changed by disk tools*/
#define ENTRY_NAME 0x0F      /* entry contain length name */

/*
 * Structure of a directory entry.
 */
typedef struct _directoryEntry
{
    uint8_t name[8];      /* Short file name (padded with spaces) */
    uint8_t extension[3]; /* Short file extension (padded with spaces) */
    uint8_t attributes;   /* file attributes */
    uint8_t dontCare1;
    uint8_t dontCare2;
    uint8_t creatTime[2]; /* create time */
    uint8_t creatDate[2]; /* create date */
    uint8_t dontCare3[2];
    uint8_t dontCare4[2];
    uint8_t modifiedTime[2];  /* Last modified time  */
    uint8_t modifiedDate[2];  /* last modified date */
    uint8_t startClusters[2]; /* starting cluster of file */
    uint8_t size[4];          /* size of file in bytes */
} DirectoryEntry;

/*
 * BIOS Parameter Block structure (FAT12/16)
 */
typedef struct _tagBIOSParam
{
    uint8_t bytePerSector[2];     /* bytes per sector */
    uint8_t secPerCluster;        /* sectors per cluster */
    uint8_t numReservedSector[2]; /* Count of reserved logical sectors */
    uint8_t numFAT;               /* Number of File Allocation Tables. */
    uint8_t maxNumRootEntry[2];   /* Maximum number of FAT12 or FAT16 root directory entries */
    uint8_t totalSectors[2];      /* Total logical sectors. 0 for FAT32 */
    uint8_t mediaType;            /* Media descriptor */
    uint8_t sectorPerFAT[2];      /* Logical sectors per File Allocation Table */
    uint8_t sectorPerTrack[2];    /* Physical sectors per track */
    uint8_t numHeads[2];          /* Number of heads for disks */
    uint8_t numHiddenSectors[4];  /* Count of hidden sectors */
    uint8_t numSectors[4];        /* Total logical sectors including hidden sectors. */
} BIOSParam;

/*
 * Date structure
 */
typedef struct
{
    uint16_t day;
    uint16_t month;
    uint16_t year;
} Date;

/*
 * Time structure
 */
typedef struct
{
    uint16_t seconds;
    uint16_t minutes;
    uint16_t hours;
} Time;

#define F_SEEK_SET 0
#define F_SEEK_CUR 1
/*
 * File structure
 */
typedef struct
{
    //unsigned int *clusters;
    unsigned int startCluster;
    unsigned int currentCluster; //current cluster in disk
    unsigned int currentSector; //currentSector in cluster, range 0 to sectorPerCluster
    unsigned int currentByte; //current byte in sector, range 0 to bytePerSector
    //unsigned int seek; //current
} File;

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief <get size of file>
 *
 * @param entry <Pointer to a entry object>.
 *
 * @return <File size in bytes>.
 */
unsigned int GetSizeofFile(DirectoryEntry *entry);

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
void GetName(char *fullname, DirectoryEntry *entry);

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
void GetFileType(char *extension, DirectoryEntry *entry);

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
void GetFileModifiedDate(Date *modifiedDate, DirectoryEntry *entry);

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
void GetFileModifiedTime(Time *modifiedTime, DirectoryEntry *entry);

/*!
 * @brief <Get start sector in root directory>
 *
 * @param none
 *
 * @return <Position of start sector in root directory>.
 */
unsigned int GetStartSectorRoot();

/*!
 * @brief <read value of number _count bytes from the location pointed to by _source>
 *
 * @param _count <Number of bytes to read>.
 * @param _source <Pointer to the source of data to be read>.
 *
 * @return <value of number>.
 */
uint64_t ReadNumber(int _count, const void *_source);

/*!
 * @brief <Opens the file whose info is specified in the parameter entry>
 *
 * @param entry <Pointer to a entry object>.
 *
 * @return <a pointer to a File object>.
 */
File *OpenFile(DirectoryEntry *entry);

/*!
 * @brief <Close file>
 *
 * @param _file <Pointer to a File object>.
 *
 * @return <none>.
 */
void CloseFile(File *_file);

/*!
 * @brief <Get number of sectors in root directory>
 *
 * @param <none>.
 *
 * @return <number of sectors in root directory>.
 */
unsigned int SectorPerRoot();

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
 * if the end-of-file was reached while reading -> return 0
 * else return non-zero
 */
unsigned int Fread(void *_Buffer, unsigned int _ElementSize, unsigned int _ElementCount, File *_file);

void FatInit(const char *_imgName);

void FatDeInit();


unsigned int GetNextCluster(unsigned int current);


int GetEntry(void *entry, unsigned int i, unsigned int startCluster);

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
void Fseek(File* _file, unsigned int _offset, int _origin);
#endif