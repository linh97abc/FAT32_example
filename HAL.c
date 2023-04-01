#include "HAL.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define BYTEPERSEC_OFFSET 0x00B /*sector offset, Bytes per logical sector*/

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
FILE *g_img = NULL;
void *g_tempSector = NULL;
unsigned int g_tempSectorPos;
uint16_t g_bytePerSector = 0;
/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief <Read 1 sector and store them in the block of temporary memory>
 *
 * @param _sectorPosition <sector position>.
 *
 * @return <Pointer to a block of memory>.
 */
void *GetSector(unsigned int _sectorPosition)
{
    if (_sectorPosition != g_tempSectorPos)
    {
        ReadSector(g_tempSector, _sectorPosition);
    }

    return g_tempSector;
}

/*!
 * @brief <open file img whose name is specified in the parameter filename>
 *
 * @param fileName <string containing the name of the file to be opened>.
 *
 * @return <none>.
 */
void OpenImg(const char *fileName)
{
    if (g_img != NULL)
    {
        CloseImg();
    }

    //g_img = fopen(fileName, "rb");
    fopen_s(&g_img, fileName, "rb");
    if (g_img == NULL)
    {
        exit(1);
    }

    fseek(g_img, BYTEPERSEC_OFFSET, SEEK_SET);
    fread(&g_bytePerSector, sizeof(g_bytePerSector), 1, g_img);

    g_tempSector = (void *)malloc(g_bytePerSector);
    ReadSector(g_tempSector, 0);
    g_tempSectorPos = 0;
}

/*!
 * @brief <Close file img>
 *
 * @param <none>.
 *
 * @return <none>.
 */
void CloseImg()
{
    free(g_tempSector);
    fclose(g_img);
    g_img = NULL;
}

/*!
 * @brief <Read 1 sector and store them in the block of memory specified by _sector>
 *
 * @param _sector <Pointer to a block of memory with a size of at least (bytePerSector) bytes>.
 * @param _sectorPosition <sector position>.
 *
 * @return <none>.
 */
void ReadSector(void *_sector, unsigned int _sectorPosition)
{
    long offset;
    offset = g_bytePerSector * _sectorPosition;
    fseek(g_img, offset, SEEK_SET);
    fread(_sector, 1, g_bytePerSector, g_img);
}

/*!
 * @brief <Read _count sector and store them in the block of memory specified by sector>
 *
 * @param _sector <Pointer to a block of memory with a size of at least (bytePerSector) bytes>.
 * @param _sectorPosition <sector position>.
 * @param _count <number of sector>
 *
 * @return <none>.
 */
void ReadNSectors(void *_sector, unsigned int _sectorPosition, unsigned int _count)
{
    long offset;
    offset = g_bytePerSector * _sectorPosition;
    fseek(g_img, offset, SEEK_SET);
    fread(_sector, 1, g_bytePerSector * _count, g_img);
}
