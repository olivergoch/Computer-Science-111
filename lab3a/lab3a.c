#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include "lab3a_SafeLib.h"
#include "ext2_fs.h"

/*/////////////////////////////////////////////////////////
// Constants
/////////////////////////////////////////////////////////*/
#define TRUE 1
#define FALSE 0
#define STD_IN 0
#define STD_OUT 1
#define STD_ERR 2
#define ZERO_BYTE '\0'
#define BUFF_SIZE 1024
#define SUPER_OFFSET 1024
#define SUPER_BLOCK_OFFSET 1024
#define BITS_IN_A_BYTE 8
// Spec Constants
#define BLOCK_ADDRESSING_THRESHOLD 60
#define EXIT_SUCCESSFUL_ANALYSIS 0
#define EXIT_BAD_ARGUMENTS 1
#define EXIT_OTHER_FAIL 2
#define INODE_DIRECTORY 'd'
#define INODE_REG_FILE 'f'
#define INODE_SYM_LINK 's'
#define INODE_OTHER '?'
// EXT2 Constants NOT INCLUDED IN CS111's ext2_fs.h
#define EXT2_S_IFREG 0x8000
#define EXT2_S_IFDIR 0x4000
#define EXT2_S_IFLNK 0xA000
#define NO_BLOCKS_PER_INDIR 256 // defined in 3.5.13.i_block
#define NON_PRESENT_ENTRY 0
#define SINDIR_OFFSET 12 // defined in 3.5.13.i_block
#define DINDIR_OFFSET (SINDIR_OFFSET + 256) // defined in 3.5.13.i_block
#define TINDIR_OFFSET (DINDIR_OFFSET + 65536) // defined in 3.5.13.i_block
// Function constants
#define GET_GMDT_SUCCESS 0
#define GET_GMDT_FAIL_GMTIM -1
#define GET_GMDT_FAIL_STRFT -2
#define GET_GMDT_FAIL_OTHER -3
#define GET_GMDT_TIMESTR_SZ 18 // becuase the string will be (example) "01/30/98 16:32:03", which is 18 characters, counting ZERO_BYTE

/*/////////////////////////////////////////////////////////
// Typedefs
/////////////////////////////////////////////////////////*/
typedef struct ext2_super_block super_block;
typedef struct ext2_group_desc group_desc;
typedef struct ext2_inode inode;
typedef struct ext2_dir_entry dir_entry;

/*/////////////////////////////////////////////////////////
// Structs
/////////////////////////////////////////////////////////*/
typedef struct superBlockSummary_
{
    uint32_t noBlocks;
    uint32_t noInodes;
    uint32_t blockSize;
    uint16_t inodeSize;
    uint32_t blocksPerGroup;
    uint32_t inodesPerGroup;
    uint32_t firstInode;
} superSummary;

typedef struct groupSummary_
{
    unsigned int groupNo;
    unsigned int noBlocks;
    unsigned int noInodes;
    uint16_t noFreeBlocks;
    uint16_t noFreeInodes;
    uint32_t blockBitMapNo;
    uint32_t inodeBitMapNo;
    uint32_t inodeTableID;
} groupSummary;

/*/////////////////////////////////////////////////////////
// GLOBAL VARIABLES
/////////////////////////////////////////////////////////*/
superSummary superData;
groupSummary* groupData = NULL;
unsigned int noGroups;
void* g_buff = NULL;
inode* g_inodeBuff = NULL;

/*/////////////////////////////////////////////////////////
// Function Definitions
/////////////////////////////////////////////////////////*/

/* 
 *  Frees any potentially-allocated memory.
 */
void cleanup();

/* 
 *  Checks for all valid options/arguments to the program, and handles them appropriately.
 *  The only valid arguments are:
 *      1. img_filename [required]
 *  If this argument is not specified, or it refers to an invalid file, an exit message will be printed
 *  and the program will exit. Otherewise, the file descriptor of the opened img_filename will be returned.
 *  @param argc: the argc variable passed to main.
 *  @param argv: the argv variable passed to main.
 *  @return: the file descriptor of img_filename.
 */
int parseAndExecuteArguments(int argc, char** argv);

/* 
 *  Calculates the block offset based on the given block number.
 *  @param blockNo: the number of the block we want the offset for.
 *  @return: the byte-based offset of the specified block.
 */
unsigned int blockOffset(unsigned int blockNo);

/* 
 *  Determines the inode type based on the imode, and returns the type as a single-character identifier.
 *  @param imode: the imode field of the inode you're interested in.
 *  @return: the single-character identifier of the type. As to conform to the spec, these identifiers are:
 *      1. 'f' for file
 *      2. 'd' for directory
 *      3. 's' for symbolic link
 *      4. '?' for anything else
 */
char parseInodeType(uint16_t imode);

/* 
 *  Turns the time parameter (which ought to be seconds since the epoch) and returns a formatted string
 *  (formatted like MM/DD/YY HH:MM:SS).
 *  @param inodeTimeVal: the seconds since the epoch for the time we wish to convert.
 *  @param timeString: if no errors, the formatted time string. It is assumed that any buffer passed here will be at least of size GET_GMDT_TIMESTR_SZ.
 *                     otherwise, NULL. Please be aware that this will "repoint" timeString, so don't have it pointing to anything meaningful when passed.
 *  @return: 0 (GET_GMDT_SUCCESS) on successful completion.
 *           < 0 on failure. Specifically:
 *              GET_GMDT_FAIL_GMTIM on a failed call to gmtime().
 *              GET_GMDT_FAIL_STRFT on a failed call to strfttime().
 *              GET_GMDT_FAIL_OTHER for any other failure.
 */
int getGMTDateTime(time_t inodeTimeVal, char* timeString);

/*  Summarizes the superblock, printing eight comma-separated fields (with no white-space) to stdout.
 *  These fields are:
 *      1. SUPERBLOCK
 *      2. total number of blocks (decimal)
 *      3. total number of i-nodes (decimal)
 *      4. block size (in bytes, decimal)
 *      5. i-node size (in bytes, decimal)
 *      6. blocks per group (decimal)
 *      7. i-nodes per group (decimal)
 *      8. first non-reserved i-node (decimal)
 *  @param fdImg: the image where we read the super block from.
 */
void summarizeSuperBlock(int fdImg);

/*  Summarizes each group in fdImg, printing a new-line terminated line for each group, with each line comprised 
 *  of nine comma-separated fields (with no white space).
 *  These fields are:
 *      1. GROUP
 *      2. group number (decimal, starting from zero)
 *      3. total number of blocks in this group (decimal)
 *      4. total number of i-nodes in this group (decimal)
 *      5. number of free blocks (decimal)
 *      6. number of free i-nodes (decimal)
 *      7. block number of free block bitmap for this group (decimal)
 *      8. block number of free i-node bitmap for this group (decimal)
 *      9. block number of first block of i-nodes in this group (decimal)
 *  @param fdImg: the image where we get the groups from.
 */
void summarizeGroups(int fdImg);

/*  Scans the provided bitmap, treating it as a data block bitmap if isDataBitMap == TRUE, as an inode bitmap otherwise.
 *  @param fdImg: the image where we get the data from.
 *  @param isDataBitMap: If isDataBitMap is TRUE, bitMap is treated as a data block bitmap. Otherwise, inode bitmap.
 *  Thus, if isDataBitMap == TRUE, print a new-line terminated line, with two comma-separated fields (no white space):
 *      1. BFREE
 *      2. number of the free blocks (decimal)
 *  Else:
 *      If the bit is marked as free, print the following two comma-separated fields (no white space):
 *          1. IFREE
 *          2. number of free inodes (decimal)
 *      Else, call summarizeInode().
 */
void operateOnBitMap(int fdImg, int isDataBitMap);

/*  If the inode has a non-zero link count, produce a new-line terminated line, 
 *  with up to 27 comma-separated fields (with no white space).
 *  These fields are:
 *      1. INODE
 *      2. inode number (decimal)
 *      3. file type ('f' for file, 'd' for directory, 's' for symbolic link, '?" for anything else)
 *      4. mode (low order 12-bits, octal ... suggested format "%o")
 *      5. owner (decimal)
 *      6. group (decimal)
 *      7. link count (decimal)
 *      8. time of last I-node change (mm/dd/yy hh:mm:ss, GMT)
 *      9. modification time (mm/dd/yy hh:mm:ss, GMT)
 *      10. time of last access (mm/dd/yy hh:mm:ss, GMT)
 *      11. file size (decimal)
 *      12. number of (512 byte) blocks of disk space (decimal) taken up by this file
 *      13-24. 12 direct block addresses
 *      25. Indirect Block Address
 *      26. Double Indirect Block Address
 *      27. Triple Indirect Block Address 
 *  If it is a symbolic link, and its length is than 60 bytes, fields 13-27 are not printed.
 *  @param fdImg: the image where we get the groups from.
 *  @param iNodeNo: the number of the inode we wish to summarize.
 *  @param groupNo: the group that this inode belongs to.
 */
void summarizeInode(int fdImg, unsigned int inodeNo, unsigned int groupNo);

/*  For each valid (non-zero I-node number) directory entry, print a new-line terminated line, 
 *  with seven comma-separated fields (no white space).
 *  These fields are:
 *      1. DIRENT
 *      2. parent inode number (decimal) ... the I-node number of the directory that contains this entry
 *      3. logical byte offset (decimal) of this entry within the directory
 *      4. inode number of the referenced file (decimal)
 *      5. entry length (decimal)
 *      6. name length (decimal)
 *      7. name (string, surrounded by single-quotes). 
 *  Note: for field 7, this function doesn't handle escaping characters. This is because the spec promises:
 *  "Don't worry about escaping, we promise there will be no single-quotes or commas in any of the file names."
 *  @param fdImg: the image where we get the inode from.
 *  @param inodeBuff: the buffer holding the directory inode.
 *  @param iNodeNo: the number of the inode passed.
 *  @return: 0 if successful.
 *           If the inode specified by inodeBuff is NOT a directory entry, -1.
 *           Otherwise, -2.
 */
int summarizeDirectoryInode(int fdImg, const inode* inodeBuff, unsigned int inodeNo);

/*  For each non-zero indirect block pointer found in this inode, produce a new-line terminated line 
 *  with six comma-separated fields (no white space).
 *  These fields are:
 *      1. INDIRECT
 *      2. I-node number of the owning file (decimal)
 *      3. (decimal) level of indirection for the block being scanned ... 1 for single indirect, 2 for double indirect, 3 for triple
 *      4. logical block offset (decimal) represented by the referenced block. 
 *         If the referenced block is a data block, this is the logical block offset of that block within the file. 
 *         If the referenced block is a single- or double-indirect block, this is the same as the logical offset of the first data block to which it refers.
 *      5. block number of the (1, 2, 3) indirect block being scanned (decimal) . . . not the highest level block (in the recursive scan), 
 *         but the lower level block that contains the block reference reported by this entry.
 *      6. block number of the referenced block (decimal)
 *  @param fdImg: the image where we get the inode from.
 *  @param inodeBuff: the buffer holding the directory inode.
 *  @param iNodeNo: the number of the inode passed.
 *  @param blockNo: the number of the indirect reference block we want to look at.
 *  @param level: the depth of the block we're looking at. Meaning, 3 if it's a triple-indirect, 2 if it's a double-indirect, 1 if a single.
 *  @param offset: the logical offset, as per the spec.
 */
void summarizeIndirectRefs(int fdImg, uint32_t inodeNo, uint32_t blockNo, uint32_t level, uint32_t offset);

/*/////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////*/

int main(int argc, char** argv)
{
    s_atexit(cleanup);

    int fdImg = parseAndExecuteArguments(argc, argv);

    summarizeSuperBlock(fdImg);

    g_buff = s_malloc(superData.blockSize); // Init a global buffer to pass around from function to function, to save on malloc costs
    g_inodeBuff = s_malloc(sizeof(inode));

    summarizeGroups(fdImg);

    // Summarize data bitmap and inodes
    operateOnBitMap(fdImg, TRUE); // data bitmap
    operateOnBitMap(fdImg, FALSE); // inode

    exit(EXIT_SUCCESSFUL_ANALYSIS);
}

/*/////////////////////////////////////////////////////////
// Function Implementations
/////////////////////////////////////////////////////////*/

void cleanup()
{
    free(g_inodeBuff);
    free(g_buff);
    free(groupData);
}

int parseAndExecuteArguments(int argc, char** argv)
{
    if( argc != 2 )
    {
        fprintf(stderr, "ERROR: improper number of arguments.\nUsage: ./lab3a IMG_FILENAME.\n");
        exit(EXIT_BAD_ARGUMENTS);
    }

    int fdImg = open(argv[1], O_RDONLY);
    if( fdImg < 0 )
    {
        int err = errno;
        fprintf(stderr, "ERROR: failed to open file \"%s\". %s.\n", argv[1], strerror(err));
        exit(EXIT_BAD_ARGUMENTS);
    }
    return fdImg;
}

unsigned int blockOffset(unsigned int blockNo)
{
    return SUPER_OFFSET + ((blockNo - 1) * superData.blockSize);
}

char parseInodeType(uint16_t imode)
{
    if( imode & EXT2_S_IFREG )
        return INODE_REG_FILE;
    else if( imode & EXT2_S_IFDIR )
        return INODE_DIRECTORY;
    else if( imode & EXT2_S_IFLNK )
        return INODE_SYM_LINK;
    else
        return INODE_OTHER;
}

int getGMTDateTime(time_t inodeTimeVal, char* timeString)
{
    struct tm* utcTime;
    if( (utcTime = gmtime(&inodeTimeVal)) == NULL ) return GET_GMDT_FAIL_GMTIM;
    if( strftime(timeString, GET_GMDT_TIMESTR_SZ, "%m/%d/%y %H:%M:%S", utcTime) != (GET_GMDT_TIMESTR_SZ - 1) ) return GET_GMDT_FAIL_STRFT;
    return GET_GMDT_SUCCESS;
}

void summarizeSuperBlock(int fdImg)
{
    super_block* super = (super_block*) s_malloc(sizeof(super_block));
    s_pread(fdImg, super, EXT2_MIN_BLOCK_SIZE, SUPER_BLOCK_OFFSET);

    superData.noBlocks = super->s_blocks_count;
    superData.noInodes = super->s_inodes_count;
    superData.blockSize = EXT2_MIN_BLOCK_SIZE << super->s_log_block_size;
    superData.inodeSize = super->s_inode_size;
    superData.blocksPerGroup = super->s_blocks_per_group;
    superData.inodesPerGroup = super->s_inodes_per_group;
    superData.firstInode = super->s_first_ino;

    printf("SUPERBLOCK,%u,%u,%u,%u,%u,%u,%u\n", superData.noBlocks,     superData.noInodes,         superData.blockSize,
                                                superData.inodeSize,    superData.blocksPerGroup,   superData.inodesPerGroup,
                                                superData.firstInode);

    free(super);
}

void summarizeGroups(int fdImg)
{
    noGroups = 1 + ((superData.noBlocks - 1) / superData.blocksPerGroup);
    group_desc* groupDescr = (group_desc*) s_malloc(sizeof(group_desc) * noGroups);
    groupData = s_malloc(sizeof(groupSummary) * noGroups);
    s_pread(fdImg, groupDescr, sizeof(group_desc) * noGroups, SUPER_OFFSET + superData.blockSize);

    for( unsigned int i = 0; i < noGroups; i++ )
    {
        unsigned int noGroupBlocks = ((i+1) == noGroups) ? superData.noBlocks - (i * superData.blocksPerGroup) : superData.blocksPerGroup;
        unsigned int noGroupInodes = ((i+1) == noGroups) ? superData.noInodes - (i * superData.inodesPerGroup) : superData.inodesPerGroup;
        
        groupData[i].groupNo = i;
        groupData[i].noBlocks = noGroupBlocks;
        groupData[i].noInodes = noGroupInodes;
        groupData[i].noFreeBlocks = groupDescr->bg_free_blocks_count;
        groupData[i].noFreeInodes = groupDescr->bg_free_inodes_count;
        groupData[i].blockBitMapNo = groupDescr->bg_block_bitmap;
        groupData[i].inodeBitMapNo = groupDescr->bg_inode_bitmap;
        groupData[i].inodeTableID = groupDescr->bg_inode_table;

        printf("GROUP,%u,%u,%u,%u,%u,%u,%u,%u\n", groupData[i].groupNo,       groupData[i].noBlocks,     groupData[i].noInodes,
                                                  groupData[i].noFreeBlocks,  groupData[i].noFreeInodes, groupData[i].blockBitMapNo,
                                                  groupData[i].inodeBitMapNo, groupData[i].inodeTableID);
    }
    free(groupDescr);
}

void operateOnBitMap(int fdImg, int isDataBitMap)
{
    // This is a slightly messy function, mainly due to isDataBitMap, but if I tried to make it more abstract and clean, 
    // I ended up needing. 10 or so parameters. At the end of the day, I don't really NEED abstract or clean, because
    // we're definitely not going to deal with more than two bitmaps, and we're definitely not going to want to do more
    // than 2 or 3 operations on them.
    /* Bit map organized like:
                         ---------------
                 BYTE 0:| bit7 ... bit0 |
                 BYTE 1:| bit7 ... bit0 |
                        |               |
              BYTE 1023:| bit7 ... bit0 |
                         ---------------
        Thus, we want to access the bits of each byte, hence iterating byteNo up to actualSize (the total number of bytes),
        and then iterating bitNo up to BITS_IN_A_BYTE.
    */

    unsigned int itemNo = 1;
    uint8_t* readBitMap = (uint8_t*) g_buff;
	for( unsigned int i = 0; i < noGroups; i++ )
	{
        // Figure out which data points we're using based on the bitmap
        uint32_t bitMapID = isDataBitMap ? groupData[i].blockBitMapNo : groupData[i].inodeBitMapNo;
        uint32_t totalBytes = superData.blockSize;
        unsigned int totalItems = isDataBitMap ? groupData[i].noBlocks : groupData[i].noInodes;
		s_pread(fdImg, readBitMap, superData.blockSize, bitMapID * superData.blockSize);

		for( unsigned int byteNo = 0; byteNo < (totalBytes); byteNo++ )
		{
			unsigned int bitMask = 1; //we need to mask off the part we want
			for( unsigned int bitNo = 0; bitNo < BITS_IN_A_BYTE; bitNo++, itemNo++ )
			{
				//if we run over the actual number of alloc'd inodes, exit func
				if( itemNo > (totalItems) ) return;
				if( (bitMask & readBitMap[byteNo]) == 0 ) // Whether we're operating on an inode bitmap, or a data bit map, if the bit is marked as free, we always want to summarize it, and that's it
                {
					if( isDataBitMap ) printf("BFREE,%u\n", itemNo + i * superData.blocksPerGroup);
                    else printf("IFREE,%u\n", itemNo + i * superData.inodesPerGroup);
                }
                
                if( !isDataBitMap && (bitMask & readBitMap[byteNo]) ) // only if we're operating on an inode bitmap do we care about doing stuff with alloc'd entries
                {
                    summarizeInode(fdImg, itemNo, i);
                }
				bitMask = bitMask << 1; //shift it over to get next byte
			}
		}
	}
}

void summarizeInode(int fdImg, unsigned int inodeNo, unsigned int groupNo)
{
    s_pread(fdImg, g_inodeBuff, sizeof(inode), blockOffset(groupData[groupNo].inodeTableID) + (inodeNo-1) * sizeof(inode));
    uint16_t linkCount = g_inodeBuff->i_links_count;
    if( linkCount > 0 )
    {
        char inodeType = parseInodeType(g_inodeBuff->i_mode);
        char creTime[GET_GMDT_TIMESTR_SZ], modTime[GET_GMDT_TIMESTR_SZ], accTime[GET_GMDT_TIMESTR_SZ];
        int creTimeRet = 0, modTimeRet = 0, accTimeRet = 0;
        if( (creTimeRet = getGMTDateTime(g_inodeBuff->i_ctime, creTime)) < 0 || (modTimeRet = getGMTDateTime(g_inodeBuff->i_mtime, modTime)) < 0 || (accTimeRet = getGMTDateTime(g_inodeBuff->i_atime, accTime)) < 0 )
        {
            fprintf(stderr, "ERROR: failed to process inode epoch tracker into GMT.\n");
            if( creTimeRet < 0 ) fprintf(stderr, "Attempt to get creation time failed. Contents of creTime: %s.\nError code: %d\n", creTime, creTimeRet);
            if( modTimeRet < 0 ) fprintf(stderr, "Attempt to get modification time failed. Contents of modTime: %s.\nError code: %d\n", modTime, modTimeRet);
            if( accTimeRet < 0 ) fprintf(stderr, "Attempt to get access time failed. Contents of accTime: %s.\nError code: %d\n", accTime, accTimeRet);
            exit(EXIT_OTHER_FAIL);
        }

        printf("INODE,%u,%c,%o,%u,%u,%u,%s,%s,%s,%u,%u", inodeNo,             inodeType,             g_inodeBuff->i_mode & 0xFFF,
                                                         g_inodeBuff->i_uid,  g_inodeBuff->i_gid,    linkCount,
                                                         creTime,             modTime,               accTime,
                                                         g_inodeBuff->i_size, g_inodeBuff->i_blocks);
                                                         

        if( inodeType == INODE_SYM_LINK && g_inodeBuff->i_size < BLOCK_ADDRESSING_THRESHOLD ) printf("\n"); // if we're a symbolic link, and we're below the threshold, we're done printing
        else // No matter what, we're supposed to summarize these blocks within the summarize inode field
        {
            printf(",");
            for( int i = 0; i < EXT2_N_BLOCKS; i++ )
            {
                printf("%u", g_inodeBuff->i_block[i]); // quick note: here, we are printing the address of what this this stuff points to. We don't have it read into memory yet.
                if( (i+1) != EXT2_N_BLOCKS ) printf(","); // we don't want a comma on our final value, but we want it everywhere else
            }
            printf("\n");

            // Now that we've finished up the inode summary line, we need to turn our attention to other things
            // If it's a directory, we need to do a per-directory processing, as per the spec
            if( inodeType == INODE_DIRECTORY ) 
                if( summarizeDirectoryInode(fdImg, g_inodeBuff, inodeNo) < 0 ) // I built in a return value for summarize directory, but, if everything else is right, this should never fail
                {
                    fprintf(stderr, "ERROR: attempt to summarize a directory inode failed. This should not be happening.\n");
                    exit(EXIT_OTHER_FAIL);
                }

            if( inodeType == INODE_DIRECTORY || inodeType == INODE_REG_FILE )
            {
                summarizeIndirectRefs(fdImg, inodeNo, g_inodeBuff->i_block[EXT2_IND_BLOCK], 1, SINDIR_OFFSET);
                summarizeIndirectRefs(fdImg, inodeNo, g_inodeBuff->i_block[EXT2_DIND_BLOCK], 2, DINDIR_OFFSET);
                summarizeIndirectRefs(fdImg, inodeNo, g_inodeBuff->i_block[EXT2_TIND_BLOCK], 3, TINDIR_OFFSET);
            }
        }
    }
}

int summarizeDirectoryInode(int fdImg, const inode* inodeBuff, unsigned int inodeNo)
{
    if( parseInodeType(inodeBuff->i_mode) != INODE_DIRECTORY )
        return -1;
    
    unsigned char* buff = s_malloc(superData.blockSize);
    s_pread(fdImg, buff, superData.blockSize, blockOffset(inodeBuff->i_block[0]));
    unsigned int size = 0; // the total size of what we've looked at so far
    unsigned int noBlocks = 1; // the number of blocks we've looked at
    char fileName[EXT2_NAME_LEN+1];
    dir_entry* entry = (dir_entry*) buff;
    while( size < inodeBuff->i_size && noBlocks < EXT2_NDIR_BLOCKS && entry->file_type ) // we don't want to surpass EXT2_NDIR_BLOCKS because we process indirect blocks elsewhere; ALSO, unknown files return 0 for file_typeh, and we don't want to fuck with unknown files
    {
        memcpy(fileName, entry->name, entry->name_len);
        fileName[entry->name_len] = ZERO_BYTE;

        if( entry->inode != 0 ) // The inode number must be nonzero for us to report
            printf("DIRENT,%u,%u,%u,%u,%u,\'%s\'\n", inodeNo,         size,               entry->inode,
                                                     entry->rec_len,  entry->name_len,    fileName);

        size += entry->rec_len;
        entry = (void*) entry + entry->rec_len;
    
        if( size < inodeBuff->i_size && size >= superData.blockSize * noBlocks )
        {
            s_realloc(buff, superData.blockSize * ++noBlocks);
            entry = (void*) buff + size; // reset entry to the new point it's supposed to look at, since realloc might've moved things
            s_pread(fdImg, entry, superData.blockSize, blockOffset(inodeBuff->i_block[noBlocks-1]));
        }
    }
    free(buff);
    return 0;
}

void summarizeIndirectRefs(int fdImg, uint32_t inodeNo, uint32_t blockNo, uint32_t level, uint32_t offset)
{
    uint32_t* buff = (uint32_t*) s_malloc(superData.blockSize);
    s_pread(fdImg, buff, superData.blockSize, blockOffset(blockNo));
    for( int i = 0; i < NO_BLOCKS_PER_INDIR; i++ )
    {
        if( buff[i] != NON_PRESENT_ENTRY )
        {
            printf("INDIRECT,%u,%u,%u,%u,%u\n", inodeNo, level, offset+i, blockNo, buff[i]);
            if( level > 1 ) summarizeIndirectRefs(fdImg, inodeNo, buff[i], level-1, offset+i);
        }
    }
    free(buff);
}
