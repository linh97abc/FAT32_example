#ifndef _HAL_H_
#define _HAL_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/*******************************************************************************
 * API
 ******************************************************************************/

/*!
 * @brief <Read 1 sector and store them in the block of temporary memory>
 *
 * @param _sectorPosition <sector position>.
 *
 * @return <Pointer to a block of memory>.
 */
void *GetSector(unsigned int _sectorPosition);

/*!
 * @brief <Read 1 sector and store them in the block of memory specified by _sector>
 *
 * @param _sector <Pointer to a block of memory with a size of at least (bytePerSector) bytes>.
 * @param _sectorPosition <sector position>.
 *
 * @return <none>.
 */
void ReadSector(void *_sector, unsigned int _sectorPosition);

/*!
 * @brief <Read _count sector and store them in the block of memory specified by sector>
 *
 * @param _sector <Pointer to a block of memory with a size of at least (bytePerSector) bytes>.
 * @param _sectorPosition <sector position>.
 * @param _count <number of sector>
 *
 * @return <none>.
 */
void ReadNSectors(void *_sector, unsigned int _sectorPosition, unsigned int _count);

/*!
 * @brief <open file img whose name is specified in the parameter filename>
 *
 * @param fileName <string containing the name of the file to be opened>.
 *
 * @return <none>.
 */
void OpenImg(const char *fileName);

/*!
 * @brief <Close file img>
 *
 * @param <none>.
 *
 * @return <none>.
 */
void CloseImg();

#endif