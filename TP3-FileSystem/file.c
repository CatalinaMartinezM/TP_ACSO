#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "file.h"
#include "inode.h"
#include "diskimg.h"

int file_getblock(struct unixfilesystem *fs, int inumber, int blockNum, void *buf) {
    struct inode in;
    if (inode_iget(fs, inumber, &in) < 0) return -1;

    if ((in.i_mode & IALLOC) == 0) return -1;

    //Calculo tamaño archivo y cantidad de bloques
    int size = inode_getsize(&in);
    int numBlocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;

    if (blockNum < 0 || blockNum >= numBlocks) return -1;

    //Num sector el bloque
    int sector = inode_indexlookup(fs, &in, blockNum);
    if (sector <= 0) return -1;

    int res = diskimg_readsector(fs->dfd, sector, buf);
    if (res != DISKIMG_SECTOR_SIZE) return -1;

    //Ultimo bloque tiene menos bytes
    if (blockNum == numBlocks - 1 && (size % DISKIMG_SECTOR_SIZE) != 0)
        return size % DISKIMG_SECTOR_SIZE;
    
    return DISKIMG_SECTOR_SIZE;
}

