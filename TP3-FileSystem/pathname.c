#include "pathname.h"
#include "directory.h"
#include "inode.h"
#include "diskimg.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

int pathname_lookup(struct unixfilesystem *fs, const char *pathname) {
    if (!pathname || pathname[0] != '/') return -1;

    int inumber = ROOT_INUMBER;
    const char *p = pathname;
    while (*p == '/') p++;

    //Buffer
    char name[15];

    while (*p) {
        int i = 0;
        while (*p && *p != '/' && i < 14) name[i++] = *p++;
        name[i] = '\0';

        while (*p == '/') p++;

        if (i == 0) break;

        //Nombre del directorio actual
        struct direntv6 dirEnt;
        if (directory_findname(fs, name, inumber, &dirEnt) < 0) return -1;
        
        inumber = dirEnt.d_inumber;
    }
    return inumber;
}
