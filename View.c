#include "View.h"
#include "FAT.h"
#include <stdio.h>
#include <stdlib.h>

#define _1KB 1024
/**/
static void PrintTime(Time *time);

/* Print date DD/MM/YYYY */
static void PrintDate(Date *date);

static void PrintInfoEntry(DirectoryEntry *entry);

static void PrintInfoEntry(DirectoryEntry *entry)
{
    Date date;
    Time time;
    char name[20];
    char extension[20];
    unsigned int size;

    GetName(name, entry);
    GetFileType(extension, entry);
    GetFileModifiedDate(&date, entry);
    GetFileModifiedTime(&time, entry);
    size = GetSizeofFile(entry) / _1KB;    

    printf("%-20s ", name);
    PrintDate(&date);
    printf("  ");
    PrintTime(&time);
    printf("%7s", " ");

    if (entry->attributes & ENTRY_DIRECTORY)
    {
        printf("%-8s\n", "Folder");
    }
    else
    {
        printf("%-8s ", extension);

        if (size == 0)
        {
            size = 1;
        }
        printf("%6d KB\n", size);
    }

}

/*!
 * @brief <ShowFolder and select entry to open>
 *
 * @param startCluster <Start of file in clusters>.
 *
 * @return <index entry selected>.
 */
int ShowFolder(unsigned int startCluster)
{
    int i = 0;
    DirectoryEntry entry;
    int retVal;

    printf("    Name               | Date modified            | Type   | Size\n");
    do
    {
        GetEntry(&entry, i + 1, startCluster);
        if ((entry.attributes != ENTRY_NAME) &&
            (entry.name[0] != ENTRY_EMPTY) &&
            (entry.name[0] != ENTRY_DELETED))
        {
            i++;
            printf("%2d. ", i);
            
            PrintInfoEntry(&entry);
        }
    } while (entry.name[0] != ENTRY_EMPTY);

    printf("<Nhap [0] de thoat chuong trinh>\n");
    do
    {
        printf(">> Mo file: ");
        scanf_s("%d", &retVal);

        if (retVal > i)
        {
            printf("Khong co file nay\n");
        }
    } while (retVal > i);
   

    return retVal;
}

/*!
 * @brief <print data in file whose information is specified in the parameter entry>
 *
 * @param entry <Pointer to a entry object>.
 *
 * @return <none>.
 */
void PrintFile(DirectoryEntry *entry)
{
    File *myfile;    
    char name[13];
    int isNotEOF = 1;
   
#ifdef DEBUG
    FILE* log;
    GetName(name, entry);
    fopen_s(&log, name, "wb+");
    //log = fopen(name, "wb+");
#endif // DEBUG

    myfile = OpenFile(entry);
    

    while (isNotEOF)
    {
        uint8_t buff[512];
        isNotEOF = Fread(buff, 1, 512, myfile);
        if (isNotEOF)
        {
            for (int i = 0; i < 512; i++)
            {
                printf("%c", buff[i]);
            }

#ifdef DEBUG
            fwrite(buff, 1, 512, log);
#endif // DEBUG            
        }
    }
    CloseFile(myfile);

#ifdef DEBUG
    fclose(log);
#endif // DEBUG  
    
}

/* Print date DD/MM/YYYY */
static void PrintDate(Date *date)
{
    if (date->day < 10)
    {
        printf("0");
    }
    printf("%d/", date->day);

    if (date->month < 10)
    {
        printf("0");
    }
    printf("%d/%d", date->month, date->year + YEAR_OFFSET);
}

/*printf xx:xx AM/PM */
static void PrintTime(Time *time)
{
    if (time->hours < 10)
    {
        printf("0");
    }
    printf("%d:", time->hours);

    if (time->seconds < 10)
    {
        printf("0");
    }
    printf("%d ", time->seconds);

    if (time->hours < 12)
    {
        printf("AM");
    }
    else
    {
        printf("PM");
    }
}