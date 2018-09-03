import sys, csv

#############################################################
# CONSTANTS
#############################################################
# Program Constants
EXIT_FAIL = 1
EXIT_INCONS = 2
EXIT_SUCCESS = 0
INCONS_FOUND = 2
NO_INCONS = 0
# Indexing Constants
SUPERBLOCK = "SUPERBLOCK"
INODE = "INODE"
GROUP = "GROUP"
IFREE = "IFREE"
BFREE = "BFREE"
DIRENT = "DIRENT"
INDIRECT = "INDIRECT"
INODE_NO = 1
INODE_TABLE_LOC = 8
TOT_BLOCKS = 1
BFREENO = 1
INDIRNO = 5
# EXT2 Constants
BLOCK_STORE_START = 12
BLOCK_INDIR_START = 12
SINDIR_OFFSET = 12
DINDIR_OFFSET = SINDIR_OFFSET + 256
TINDIR_OFFSET = DINDIR_OFFSET + 65536

#############################################################
# CLASSES
#############################################################
class SuperBlock:
    def __init__(self, data):
        self.noBlocks = int(data[1])
        self.noInodes = int(data[2])
        self.blockSize = int(data[3])
        self.inodeSize = int(data[4])
        self.blocksPerGroup = int(data[5])
        self.inodesPerGroup = int(data[6])
        self.firstInode = int(data[7])
        
#############################################################
# GLOBAL VARIABLES
#############################################################
fileSys = {}
superData = None # this is actually set in main

#############################################################
# MAIN
#############################################################
def main():

    f = parseArgs()

    # Stores the CSV into a multiple value per key dictionary
    # They key is the line identifier (e.g., INODE or SUPERBLOCK)
    # The value is a list of lists. Each list within this super list is the
    # values from an indivdual line broken down to a list via the csv parser.
    global fileSys
    csvFile = csv.reader(f, delimiter=',', quotechar='\'')
    for row in csvFile:
        fileSys.setdefault(row[0], []).append(row)

    global superData
    superData = SuperBlock(fileSys[SUPERBLOCK][0])

    # Each function either returns NO_ICONS (0) or INCONS_FOUND (2).
    # If we OR them together, we'll either end up with 0 or 2. Those are the only two values we
    # could possibly want to return with, as per the spec, and so we do this.
    exitStatus = NO_INCONS
    exitStatus |= auditBlockConsistency()
    exitStatus |= auditInodeAllocations()
    exitStatus |= auditDirectoryConsistency()

    sys.exit(exitStatus)

#############################################################
# FUNCTION DEFINITIONS
#############################################################
'''
 *  Parses out the appropriate command line-given file name. If it is invalid, or it is not given,
 *  prints error message and exits with EXIT_FAIL.
 *  @return: the opened file, which can be operated upon.
'''
def parseArgs():
    
    if( len(sys.argv) != 2 ):
        sys.stderr.write("ERROR: invalid command line arguments.\nUsage: lab3b CSV_FILE_NAME.\n")
        sys.exit(EXIT_FAIL)
    try:
        f = open(sys.argv[1], "r")
    except IOError as e:
        sys.stderr.write("ERROR: failure to open file \'" + sys.argv[1] + "\'.\nSystem Message: " + "{0}): {1}\n".format(e.errno, e.strerror))
        sys.exit(EXIT_FAIL)
    except:
        sys.stderr.write("ERROR: failure to open file " + sys.argv[1] + ".\n")
        sys.exit(EXIT_FAIL)
    else:
        return f

'''
 *  Creates the appropriate blockName given a level of indirection. Examples include:
 *  "BLOCK", "INDIRECT BLOCK", "DOUBLE INDIRECT BLOCK", and so on.
 *  @param levelOfIndirection: the calculated level of indirection for the block you want to name.
 *  @return: the proper block name for the given level of indirection.
'''
def blockName(levelOfIndirection):
    blockType = "INDIRECT " if (levelOfIndirection > 0) else ""
    indirSpec = "" if (levelOfIndirection < 2) else "DOUBLE " if (levelOfIndirection < 3) else "TRIPLE "
    return (indirSpec + blockType + "BLOCK")

'''
 *  Calculates the block number of the first non-reserved block.
 *  @return: the block number of the first non-reserved block.
'''
def calcFirstNonReservedBlock():
    inodeTableStart = int(fileSys[GROUP][0][INODE_TABLE_LOC])
    inodeTableSize = superData.noInodes / (superData.blockSize / superData.inodeSize)
    return inodeTableStart + inodeTableSize

'''
 *  Calculates the offset based on the level of indirection.
 *  @param levelOfIndirection: the level of indirection of the block we want the offset for.
 *  @return: the proper offset.
'''
def calcOffset(levelOfIndirection):
    return 0 + SINDIR_OFFSET * (levelOfIndirection == 1) + DINDIR_OFFSET * (levelOfIndirection == 2) + TINDIR_OFFSET * (levelOfIndirection == 3)

'''
 *  Examines every single block pointer in every single I-node, direct block, indirect block, double-indirect block, 
 *  and triple indirect block to ascertain whether it is valid or not. Validity is defined as:
 *      1. An INVALID block is one whose number is less than zero or greater than the highest block in the file system.
 *      2. A RESERVED block is one that could not legally be allocated to any file because it should be reserved for file 
 *         system metadata (e.g., superblock, cylinder group summary, free block list, ...).
 *         tl;dr: this is any block with a blockNo < the val returned by calcFirstNonReservedBlock (and with a blockNo > 0).
 *      3. An ALLOCATED block is one that has been marked as being allocated somewhere, but is ALSO ref'd on the free list. That's a no no.
 *      4. An UNREFERENCED block is one between firstNonReservedBlock and totalNoBlocks that is not ref'd ANYWHERe.
 *      5. A DUPLICATE block is one that is ref'd multiple times by different INODES.
 *  Prints out inconsistent values, as defined in the spec.
 *  @return: NO_INCONS if no inconsistencies are found/reported. INCONS_FOUND if any inconsistencies are found.
'''
def auditBlockConsistency():
    ret = NO_INCONS
    firstNonReservedBlock = calcFirstNonReservedBlock()
    inodeEntries = fileSys[INODE]
    dataBlocksRefd = list(int(fileSys[INDIRECT][k][INDIRNO]) for k in range(len(fileSys[INDIRECT]))) # Build a list of all data blocks that have been referenced as free
    refTracker = {} # tracks how many references are made to any given blockNo. Keys are blockNo, vals are ref counts.
    # Look at all invalid/reserved inodes
    for row in inodeEntries:
        for i, val in enumerate(row[BLOCK_STORE_START:], start = 1):
            if int(val) not in dataBlocksRefd: dataBlocksRefd.append(int(val)) # notes a block that was referenced by an inode (and is not already marked as referenced)
            if int(val) in dataBlocksRefd and int(val) > 0: refTracker[val] = refTracker.setdefault(val, 0) + 1 # if it exists, increment it, otherwise, make it1
            levelOfIndirection = (i - BLOCK_INDIR_START) if ((i - BLOCK_INDIR_START) > 0) else 0;
            offset = calcOffset(levelOfIndirection)
            if( int(val) > superData.noBlocks or int(val) < 0 ):
                print  "INVALID " + blockName(levelOfIndirection) + " " + val + " IN INODE " + row[INODE_NO] + " AT OFFSET " + str(offset)
                ret = INCONS_FOUND
            elif( int(val) < firstNonReservedBlock and int(val) > 0 ):
                print "RESERVED " + blockName(levelOfIndirection) + " " + val + " IN INODE " + row[INODE_NO] + " AT OFFSET " + str(offset)
                ret = INCONS_FOUND
    
    # Check to see if we have alloc'd blocks ref'd on the free list
    freeBlocks = list(int(fileSys[BFREE][k][BFREENO]) for k in range(len(fileSys[BFREE]))) # build a list of all free block numbers

    for block in dataBlocksRefd:
        if block in freeBlocks:
            print "ALLOCATED BLOCK " + str(block) + " ON FREELIST"
            ret = INCONS_FOUND

    # Check to see if we have totally unreferenced blocks
    allBlocksRefd = list(set(dataBlocksRefd).union(set(freeBlocks)))
    for block in range(firstNonReservedBlock, superData.noBlocks):
        if block not in allBlocksRefd:
            print "UNREFERENCED BLOCK " + str(block)
            ret = INCONS_FOUND
    
    # There's surely a more concise way to check duplicates, but this works and is fairly clear in what it wants to do
    # Basically, we made a ref tracker earlier for each blockNo ref'd by an inode
    # Now we just reiterate through all inodes, and if a ref tracker is logging > 1 for the given block number, print the duplicate log
    for row in inodeEntries:
        for i, val in enumerate(row[BLOCK_STORE_START:], start = 1):
            levelOfIndirection = (i - BLOCK_INDIR_START) if ((i - BLOCK_INDIR_START) > 0) else 0;
            offset = calcOffset(levelOfIndirection)
            if val in refTracker and refTracker[val] > 1:
                print "DUPLICATE " + blockName(levelOfIndirection) + " " + val + " IN INODE " + row[INODE_NO] + " AT OFFSET " + str(offset)
                ret = INCONS_FOUND

    return ret

'''
 *  Examines inodes and for each discovered inconsistency in terms of allocation status, a message is sent to stdout.
 *  @return: NO_INCONS if no inconsistencies are found/reported. INCONS_FOUND if any inconsistencies are found.
'''
def auditInodeAllocations():
    ret = NO_INCONS
    firstInode = superData.firstInode
    totalInodes = superData.noInodes
    inodeEntries = fileSys[INODE]
    iFreeEntries = fileSys[IFREE]
    #Create dictionary of all known inodes and set them to none
    dictInode = {}
    for i in range(totalInodes - firstInode+1):
        dictInode.setdefault(i+firstInode, "NONE")
    #if there is an allocated inode with the number update dict
    for row in inodeEntries:
        dictInode[int(row[1])] = "ALLOCATED"
    #if there is a free inode with the number update dict
    #also check for conflicts between free and merge list
    for row in iFreeEntries:
        if(dictInode[int(row[1])] == "ALLOCATED"):
            print "ALLOCATED INODE " + row[1] + " ON FREELIST"
            ret = INCONS_FOUND
        else:
            dictInode[int(row[1])] = "FREE"
    #if there are any inodes not accounted for print it out
    for key, value in dictInode.iteritems():
        if(value == "NONE"):
            print "UNALLOCATED INODE " + str(key) + " NOT ON FREELIST"
            ret = INCONS_FOUND
    return ret

'''
 *  Audits directories to ensure all of their data is consistent.
 *  @return: NO_INCONS if no inconsistencies are found/reported. INCONS_FOUND if any inconsistencies are found.
'''
def auditDirectoryConsistency():
    ret = NO_INCONS
    direntEntries = fileSys[DIRENT]
    inodeEntries = fileSys[INODE]
    totalInodes = superData.noInodes
    iFreeEntries = fileSys[IFREE]
    #create dictionary for links
    linkDict = {}
    parentDict = {2:2}

    #iterate throught directories and increment number of links
    for row in direntEntries:
        isValid = True
        inodeNo = row[3]
        inodeParentNo = row[1]
        linkCount = row[6]
        if int(inodeNo) in linkDict:
            linkDict[int(inodeNo)] += 1
        elif int(inodeNo) < 0 or int(inodeNo) > totalInodes:
            print "DIRECTORY INODE " + inodeParentNo + " NAME '" + linkCount + "' INVALID INODE " + inodeNo
            ret = INCONS_FOUND
            isValid = False
        else:
            linkDict[int(inodeNo)] = 1

        if linkCount != ".." and linkCount != ".":
            parentDict[int(inodeNo)] = int(inodeParentNo)

        #also check for .
        if linkCount == "." and inodeParentNo != inodeNo:
            print "DIRECTORY INODE " + inodeParentNo + " NAME '.' LINK TO INODE " + inodeNo + " SHOULD BE " + inodeParentNo
            ret = INCONS_FOUND

        #also check for invalid inodes
        inodeNums = list(fileSys[INODE][k][INODE_NO] for k in range(len(fileSys[INODE])))
        if inodeNo not in inodeNums and isValid:
            print "DIRECTORY INODE " + inodeParentNo + " NAME '" + linkCount + "' UNALLOCATED INODE " + inodeNo
            ret = INCONS_FOUND
    #check ..
    for row in direntEntries:
        inodeNo = row[3]
        inodeParentNo = row[1]
        linkCount = row[6]
        if linkCount == ".." and parentDict[int(inodeParentNo)] != int(inodeNo):
            print "DIRECTORY INODE " + inodeParentNo + " NAME '..' LINK TO INODE " + inodeNo + " SHOULD BE " + str(parentDict[int(inodeParentNo)])
            ret = INCONS_FOUND

    #check all the inodes to see if link counts match up
    for row in inodeEntries:
        inodeNo = row[3]
        inodeParentNo = row[1]
        linkCount = row[6]
        if int(inodeParentNo) not in linkDict:
            print "INODE " + inodeParentNo + " HAS 0 LINKS BUT LINKCOUNT IS " + linkCount
            ret = INCONS_FOUND
        elif linkDict[int(inodeParentNo)] != int(linkCount):
            print "INODE " + inodeParentNo + " HAS " + str(linkDict[int(inodeParentNo)]) + " LINKS BUT LINKCOUNT IS " + linkCount
            ret = INCONS_FOUND
    return ret


if __name__ == "__main__":
    main()
