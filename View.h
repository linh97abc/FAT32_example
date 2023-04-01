#ifndef _VIEW_H_
#define _VIEW_H_

#include "FAT.h"

/*!
 * @brief <ShowFolder and select entry to open>
 *
 * @param startCluster <Start of file in clusters>.
 *
 * @return <index entry selected>.
 */
int ShowFolder(unsigned int startCluster);

/*!
 * @brief <print data in file whose information is specified in the parameter entry>
 *
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void PrintFile(DirectoryEntry *entry);
#endif