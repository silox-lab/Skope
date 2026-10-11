/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/

#include <dirent.h>
#include <stdio.h>
#include "../include/errors.h"
#include "../include/globals.h"
#include "../include/slugify.h"
#include "../include/database/wiki_db.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../include/utils.h"

Result get_wiki(char *name)
{
    char wiki_fullpath[256];

    snprintf(
        wiki_fullpath,
        sizeof(wiki_fullpath),
        "%s/%s",
        GET_STORAGE_PATH(),
        name
    );

    DIR *wiki_dir = opendir(wiki_fullpath);

    if (wiki_dir == NULL) {

        return (Result){
            .type = FILE_DIR_NOTFOUND,
            .value = NULL
        };
    }

    return (Result){
        .type = SUCCESS,
        .value = ""
    };
}

Result make_wiki(char *name) {

  char *wiki_name_slug = malloc(256 * sizeof(char));

  slugify_default(name, wiki_name_slug, sizeof(wiki_name_slug));

  size_t name_cap = strlen(wiki_name_slug) + 1;
  char *wiki_name = calloc(name_cap, 1);

  if (!wiki_name) {
    return (Result){ .type = SIMPLE_ERR, .value = "oom" };
  }

  strcpy(wiki_name, wiki_name_slug);

  char wiki_fullpath[256];
  snprintf(wiki_fullpath, sizeof(wiki_fullpath), "%s/%s", GET_STORAGE_PATH(), wiki_name);
  
  Result wiki_exist = get_wiki(wiki_fullpath);
  
  if (wiki_exist.type == FILE_DIR_NOTFOUND) {
    
    int mkdir_e = mkdir(wiki_fullpath, S_IRWXU);
    if (mkdir_e == -1) {
        return (Result) {
        .type = SIMPLE_ERR,
        .value = "Error when creating Wiki."
        };
    }
    return (Result){.type = SUCCESS, .value = "" };
  } else {
    return (Result){.type = SIMPLE_ERR, .value = "Wiki already exist." };
  }
}

Result delete_wiki(char *name) {

  char *wiki_name_slug = malloc(256 * sizeof(char));

  slugify_default(name, wiki_name_slug, sizeof(wiki_name_slug));

  size_t name_cap = strlen(wiki_name_slug) + 1;
  char *wiki_name = calloc(name_cap, 1);

  if (!wiki_name) {
    return (Result){ .type = SIMPLE_ERR, .value = "oom" };
  }

  strcpy(wiki_name, wiki_name_slug);

  char wiki_fullpath[256];
  snprintf(wiki_fullpath, sizeof(wiki_fullpath), "%s/%s", GET_STORAGE_PATH(), wiki_name);

  Result wiki_exist = get_wiki(wiki_fullpath);
  if (wiki_exist.type != SUCCESS) {
    return (Result){ .type = SIMPLE_ERR, .value = "Wiki not found." };
  }

  int del_dir = remove_directory_recursive(wiki_fullpath);
  if (del_dir != 0) {
    return (Result) { .type = SIMPLE_ERR, .value = "delete failed." };
  }

  return (Result) { .type = SUCCESS, .value = "" };
}

Result update_wiki(char *name, char *new_name) {  

  char *wiki_name_slug = malloc(256 * sizeof(char));

  slugify_default(name, wiki_name_slug, sizeof(wiki_name_slug));

  size_t name_cap = strlen(wiki_name_slug) + 1;
  char *wiki_name = calloc(name_cap, 1);

  if (!wiki_name) {
    return (Result){ .type = SIMPLE_ERR, .value = "oom" };
  }

  strcpy(wiki_name, wiki_name_slug);

  char wiki_fullpath[256];
  snprintf(wiki_fullpath, sizeof(wiki_fullpath), "%s/%s", GET_STORAGE_PATH(), wiki_name);
  
  char *wiki_newname_slug = malloc(256 * sizeof(char));

  slugify_default(name, wiki_newname_slug, sizeof(wiki_newname_slug));

  size_t newname_cap = strlen(wiki_newname_slug) + 1;
  char *wiki_newname = calloc(newname_cap, 1);

  if (!wiki_newname) {
    return (Result){ .type = SIMPLE_ERR, .value = "oom" };
  }

  strcpy(wiki_newname, wiki_newname_slug);

  char wiki_newfullpath[256];
  snprintf(wiki_newfullpath, sizeof(wiki_newfullpath), "%s/%s", GET_STORAGE_PATH(), wiki_newname);
  
  Result wiki_exist_db = get_wiki_db(name, 0);
  if (wiki_exist_db.type != SUCCESS) {
    return (Result) { .type = wiki_exist_db.type, .value = wiki_exist_db.value };
  }

  Result wiki_exist = get_wiki(wiki_name);
  if (wiki_exist.type != SUCCESS) {
    return (Result){ .type = SIMPLE_ERR, .value = "Wiki not found." };
  }

  Result validate_rename = get_wiki(wiki_newname);
  if (wiki_exist.type == SUCCESS) {
    return (Result){ .type = SIMPLE_ERR, .value = "there are already wikis with the name you want to chose." };
  }

  int rename_w = rename(wiki_fullpath, wiki_newfullpath);
  if (rename_w != 0) {
    return (Result) { .type = SIMPLE_ERR, .value = "error in renaming wiki." };
  }

  return (Result) { .type = SUCCESS, .value = "" };
}