/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../include/globals.h"
#include "../include/database/general.h"
#include "../include/utils.h"
#include <errno.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

static char *W_STORAGE_PATH;
static sqlite3 *W_DATABASE;

char *GET_DB_PATH() {
  char *user = get_user();
  if (user == NULL)
    return NULL;

  char *path = malloc(sizeof(char) * 256);

  snprintf(path, 256, "/home/%s/skope.db", user);
  return path;
}

char *GET_STORAGE_PATH() {
  char *user = get_user();
  if (user == NULL)
    return NULL;

  char *path = malloc(sizeof(char) * 256);

  snprintf(path, 256, "/home/%s/skope", user);
  return path;
}

void INIT_DATABASE() {
  Result db = open_database();

  if (db.type != SUCCESS) {
    W_DATABASE = NULL;
  } else {
    W_DATABASE = (sqlite3 *)db.value;
  }
}
sqlite3 *GET_W_DATABASE() { return W_DATABASE; }

void INIT_STORAGE_PATH() {
  char *user = get_user();
  if (user == NULL)
    return;

  char *path = malloc(sizeof(char) * 256);

  snprintf(path, 256, "/home/%s/skope", user);
  if (mkdir(path, S_IRWXU) == -1 && errno != EEXIST) {
    free(path);
    return;
  }

  return;
}
