/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/errors.h"
#include "../../include/globals.h"
#include <stdlib.h>
#include <sqlite3.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

Result open_database() {
  char *db_path = GET_DB_PATH();
  
  printf("OPENING DATABASE: %s\n", db_path);
  sqlite3 *db;
  int open_stat = sqlite3_open(db_path, &db);
  if (open_stat != SQLITE_OK) {
    return (Result){
        .type = SIMPLE_ERR,
        .value = "open_database: failed to open or create the database."};
  }

  int fk_enable =
      sqlite3_exec(db, "PRAGMA foreign_keys = ON;", NULL, NULL, NULL);
  if (fk_enable != SQLITE_OK) {
    return (Result){
        .type = SIMPLE_ERR,
        .value = "open_database: failed to open or create the database."};
  }
  free(db_path);
  return (Result){.type = SUCCESS, .value = db};
}

Result run_sql_db(const char *table) {

  sqlite3 *db = GET_W_DATABASE();

  char *run_err = NULL;
  int run_stat = sqlite3_exec(db, table, NULL, NULL, &run_err);
  if (run_stat != SQLITE_OK) {
    char *err_msg = strdup(run_err);
    sqlite3_free(run_err);
    return (Result){.type = DB_ERR, .value = err_msg};
  } else {
    return (Result){.type = SUCCESS, .value = ""};
  }
}
