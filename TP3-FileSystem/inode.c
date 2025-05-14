#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "inode.h"
#include "diskimg.h"

/**
 * Checks if an inode is of a specific type
 * Returns 1 on success, 0 otherwise
 */
static int inode_istype(struct inode *inp, uint16_t type) {
    return (inp->i_mode & IFMT) == type;
}

int inode_iget(struct unixfilesystem *fs, int inumber, struct inode *inp) {
    //Valido numero inode (comienzan en 1)
    if (inumber < 1) return -1;
    
    //Calculo ubicacion inode en el inode area
    int inodes_per_block = DISKIMG_SECTOR_SIZE / sizeof(struct inode);
    int inode_index = (inumber - 1) % inodes_per_block;
    int block_offset = (inumber - 1) / inodes_per_block;
    int sector = INODE_START_SECTOR + block_offset;
    
    struct inode inodes[inodes_per_block];
    int res = diskimg_readsector(fs->dfd, sector, inodes);
    if (res != DISKIMG_SECTOR_SIZE) return -1;
    
    *inp = inodes[inode_index];
    return 0;
}

int inode_indexlookup(struct unixfilesystem *fs, struct inode *inp,
    int blockNum) {  
    //Verifico si el inode esta asignado
    if ((inp->i_mode & IALLOC) == 0) return -1;
    
    // Check if file uses large addressing algorithm using bit 12 of i_mode
    int is_large = (inp->i_mode & ILARG) != 0;
    
    if (!is_large) {
        //Small file, 8 bloques directos
        if (blockNum < 0 || blockNum >= 8) return -1;
        return inp->i_addr[blockNum];
    } else {
        //Large files, 7 indirectos simples, 1 indirecto doble
        if (blockNum < 0) return -1;
        
        //Bloques singly indirect
        if (blockNum < 7 * 256) {
            int indirect_index = blockNum / 256;
            int indirect_offset = blockNum % 256;
            uint16_t indirect_block[256];
            int sector = inp->i_addr[indirect_index];
            if (sector == 0) return -1;
            int res = diskimg_readsector(fs->dfd, sector, indirect_block);
            if (res != DISKIMG_SECTOR_SIZE) return -1;
            return indirect_block[indirect_offset];
        } 
        //Bloques doubly indirect
        else if (blockNum < 7 * 256 + 256 * 256) {
            int double_index = (blockNum - 7 * 256) / 256;
            int double_offset = (blockNum - 7 * 256) % 256;
            uint16_t double_block[256];
            int sector = inp->i_addr[7];
            if (sector == 0) return -1;
            int res = diskimg_readsector(fs->dfd, sector, double_block);
            if (res != DISKIMG_SECTOR_SIZE) return -1;
            int indirect_sector = double_block[double_index];
            if (indirect_sector == 0) return -1;
            uint16_t indirect_block[256];
            res = diskimg_readsector(fs->dfd, indirect_sector, indirect_block);
            if (res != DISKIMG_SECTOR_SIZE) return -1;
            return indirect_block[double_offset];
        } else {
            return -1;
        }
    }
}

int inode_getsize(struct inode *inp) {
  return ((inp->i_size0 << 16) | inp->i_size1); 
}

int inode_isdir(struct inode *inp) {
    return inode_istype(inp, IFDIR);
}

int inode_isreg(struct inode *inp) {
    return inode_istype(inp, 0);
}
