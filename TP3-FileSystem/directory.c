#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include "file.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int directory_findname(struct unixfilesystem *fs, const char *name,
		int dirinumber, struct direntv6 *dirEnt) {
    struct inode in;
    if (inode_iget(fs, dirinumber, &in) < 0) return -1;
    
    //Verifico inode esta asignado y es un directorio
    if ((in.i_mode & IALLOC) == 0 || !inode_isdir(&in)) return -1;
    
    int size = inode_getsize(&in);
    int numBlocks = (size + DISKIMG_SECTOR_SIZE - 1) / DISKIMG_SECTOR_SIZE;
    char buf[DISKIMG_SECTOR_SIZE];
    
    //Itero bloques del directorio
    for (int bno = 0; bno < numBlocks; bno++) {
        int bytes = file_getblock(fs, dirinumber, bno, buf);
        if (bytes < 0) return -1;
        
        int entries = bytes / sizeof(struct direntv6);
        struct direntv6 *dir = (struct direntv6 *)buf;
        
        //Verifico entradas del directorio en el bloque
        for (int i = 0; i < entries; i++) {
            if (strncmp(name, dir[i].d_name, 14) == 0) {
                *dirEnt = dir[i];
                return 0;
            }
        }
    }
    return -1;
}
