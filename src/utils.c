/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#define _XOPEN_SOURCE 500
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdlib.h>
#include <ftw.h>
#include "../include/errors.h"

char *get_user() {
    struct passwd *p = getpwuid(getuid());
    if (p == NULL) return NULL;
    return p->pw_name;
}

int remove_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    int rv = remove(fpath);
    
    if (rv) {
        return 1;
    }
    
    return 0;
}

int remove_directory_recursive(const char *path) {
    return nftw(path, remove_cb, 64, FTW_DEPTH | FTW_PHYS);
}

char *result_enum_to_str(Result r) {
    
    char *status;
    switch (r.type) {

        case SUCCESS:
        status = "SUCCESS";
        return status;
        
        case DB_ERR:
        status = "DB_ERR";
        return status;

        case SIMPLE_ERR:
        status = "SIMPLE_ERR";
        return status;

        case EMPTY:
        status = "EMPTY";
        return status;

        case FILE_DIR_NOTFOUND:
        status = "FILE_DIR_NOTFOUND";
        return status;
    }
}