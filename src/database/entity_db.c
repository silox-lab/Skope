/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/errors.h"
#include "../../include/globals.h"
#include "../../include/schema.h"
#include <sqlite3.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

Result get_globalentity_db(char *type, int64_t local_id, int64_t id) {

  sqlite3_stmt *get_globalentity_stmt;
  sqlite3 *db = GET_W_DATABASE();
  if (type == NULL && local_id == 0) {
    return (Result){
        .type = SIMPLE_ERR,
        .value = "globalentity_db: both type and local id can not be empty"};
  }

  int arr_length = 0;
  int arr_capacity = 10;
  GlobalEntity **globalentity_arr =
      malloc(sizeof(GlobalEntity *) * arr_capacity);

  if (globalentity_arr == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "mglobalentity_db: emory allocation failed"};
  }

  char sql[512] = "SELECT * FROM GlobalEntity WHERE 1=1 ";
  char where_clause[512] = "";

  int bind_index = 1;
  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }
  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }
  if (local_id != 0) {
    strcat(where_clause, " AND local_id = ?");
  }

  strcat(where_clause, ";");
  strcat(sql, where_clause);

  int prepare_stat =
      sqlite3_prepare_v2(db, sql, -1, &get_globalentity_stmt, NULL);
  if (prepare_stat != SQLITE_OK) {
    free(globalentity_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  if (type != NULL) {
    sqlite3_bind_text(get_globalentity_stmt, bind_index++, type, -1,
                      SQLITE_STATIC);
  }
  if (id != 0) {
    sqlite3_bind_int(get_globalentity_stmt, bind_index++, id);
  }
  if (local_id != 0) {
    sqlite3_bind_int(get_globalentity_stmt, bind_index++, local_id);
  }

  int step_result;
  while ((step_result = sqlite3_step(get_globalentity_stmt)) == SQLITE_ROW) {
    GlobalEntity *globalentity = malloc(sizeof(GlobalEntity));

    if (globalentity == NULL) {
      int finalize_stat = sqlite3_finalize(get_globalentity_stmt);
      if (finalize_stat != SQLITE_OK) {
        for (int i = 0; i < arr_length; i++) {
          free(globalentity_arr[i]);
        }
        free(globalentity_arr);
        return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
      }
      for (int i = 0; i < arr_length; i++) {
        free(globalentity_arr[i]);
      }
      free(globalentity_arr);
      return (Result){.type = SIMPLE_ERR, .value = "memory allocation failed"};
    }

    if (arr_length >= arr_capacity) {
      arr_capacity *= 2;
      GlobalEntity **temp =
          realloc(globalentity_arr, sizeof(GlobalEntity *) * arr_capacity);
      if (temp == NULL) {
        free(globalentity);
        sqlite3_finalize(get_globalentity_stmt);
        for (int i = 0; i < arr_length; i++) {
          free(globalentity_arr[i]->type);
          free(globalentity_arr[i]);
        }
        free(globalentity_arr);
        return (Result){.type = SIMPLE_ERR,
                        .value = "memory reallocation failed"};
      }
      globalentity_arr = temp;
    }

    globalentity->id = sqlite3_column_int64(get_globalentity_stmt, 0);
    const char *type_text =
        (const char *)sqlite3_column_text(get_globalentity_stmt, 1);

    if (type_text)
      globalentity->type = strdup(type_text);

    globalentity->created_on = sqlite3_column_int(get_globalentity_stmt, 3);

    globalentity_arr[arr_length] = globalentity;
    arr_length++;
  }

  if (arr_length == 0) {
    sqlite3_finalize(get_globalentity_stmt);
    free(globalentity_arr);

    return (Result){.type = EMPTY, .value = NULL};
  }

  globalentity_arr[arr_length] = NULL;
  arr_length++;

  if (step_result != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    int finalize_stat = sqlite3_finalize(get_globalentity_stmt);
    if (finalize_stat != SQLITE_OK) {
      for (int i = 0; i != arr_length - 1; i++) {
        free(globalentity_arr[i]->type);
        free(globalentity_arr[i]);
      }
      free(globalentity_arr);
      return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
    }
    for (int i = 0; i != arr_length - 1; i++) {
      free(globalentity_arr[i]->type);
      free(globalentity_arr[i]);
    }
    free(globalentity_arr);
    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(get_globalentity_stmt);
  if (finalize_stat != SQLITE_OK) {
    for (int i = 0; i != arr_length - 1; i++) {
      free(globalentity_arr[i]->type);
      free(globalentity_arr[i]);
    }
    free(globalentity_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = globalentity_arr};
}

//[UNUSABLE] kept for future possible usecases
Result create_globalentity_db(GlobalEntity *globalentity) {
  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *create_stmt;
  const char *sql =
      "INSERT INTO GlobalEntity(type, local_id, created_on) VALUES (?, ?, ?);";

  Result globalentity_exist =
      get_globalentity_db(globalentity->type, globalentity->local_id, 0);

  if (globalentity_exist.type != SUCCESS && globalentity_exist.type != EMPTY) {
    return (Result){.type = globalentity_exist.type,
                    .value = globalentity_exist.value};
  }

  GlobalEntity **globalentity_exist_arr = globalentity_exist.value;

  if (globalentity_exist.type != EMPTY) {
    for (int i = 0; globalentity_exist_arr[i] != NULL; i++) {
      free(globalentity_exist_arr[i]->type);
      free(globalentity_exist_arr[i]);
    }
    free(globalentity_exist_arr);

    return (Result){.type = SIMPLE_ERR,
                    .value = "globalentity with same type and local id exist"};
  }

  int prepare_stat = sqlite3_prepare_v2(db, sql, -1, &create_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    for (int i = 0;
         globalentity_exist_arr != NULL && globalentity_exist_arr[i] != NULL;
         i++) {
      free(globalentity_exist_arr[i]->type);
      free(globalentity_exist_arr[i]);
    }
    free(globalentity_exist_arr);

    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  sqlite3_bind_text(create_stmt, 1, globalentity->type, -1, SQLITE_STATIC);
  sqlite3_bind_int64(create_stmt, 2, globalentity->local_id);
  sqlite3_bind_int64(create_stmt, 3, (int64_t)time(NULL));

  int step_stat = sqlite3_step(create_stmt);

  if (step_stat != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    sqlite3_finalize(create_stmt);

    for (int i = 0;
         globalentity_exist_arr != NULL && globalentity_exist_arr[i] != NULL;
         i++) {
      free(globalentity_exist_arr[i]->type);
      free(globalentity_exist_arr[i]);
    }
    free(globalentity_exist_arr);

    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(create_stmt);

  if (finalize_stat != SQLITE_OK) {
    for (int i = 0;
         globalentity_exist_arr != NULL && globalentity_exist_arr[i] != NULL;
         i++) {
      free(globalentity_exist_arr[i]->type);
      free(globalentity_exist_arr[i]);
    }
    free(globalentity_exist_arr);

    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  for (int i = 0;
       globalentity_exist_arr != NULL && globalentity_exist_arr[i] != NULL;
       i++) {
    free(globalentity_exist_arr[i]->type);
    free(globalentity_exist_arr[i]);
  }

  free(globalentity_exist_arr);

  return (Result){.type = SUCCESS, .value = ""};
}

//[UNUSABLE] kept for future possible usecases
Result delete_globalentity_db(char *type, int64_t local_id, int64_t id) {
  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *delete_globalentity_stmt;

  char sql[512] = "DELETE FROM GlobalEntity WHERE 1=1 ";
  char where_clause[512] = "";

  int bind_index = 1;

  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }

  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }

  if (local_id != 0) {
    strcat(where_clause, " AND local_id = ?");
  }

  strcat(where_clause, ";");
  strcat(sql, where_clause);

  int prepare_stat =
      sqlite3_prepare_v2(db, sql, -1, &delete_globalentity_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  if (type != NULL) {
    sqlite3_bind_text(delete_globalentity_stmt, bind_index++, type, -1,
                      SQLITE_STATIC);
  }

  if (id != 0) {
    sqlite3_bind_int64(delete_globalentity_stmt, bind_index++, id);
  }

  if (local_id != 0) {
    sqlite3_bind_int64(delete_globalentity_stmt, bind_index++, local_id);
  }

  int step_delete_globalentity = sqlite3_step(delete_globalentity_stmt);

  if (step_delete_globalentity != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));

    sqlite3_finalize(delete_globalentity_stmt);

    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(delete_globalentity_stmt);

  if (finalize_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = ""};
}

//[UNUSABLE] kept for future possible usecases
Result update_globalentity_db(char *type, int64_t local_id, int64_t id,
                              GlobalEntity globalentity) {

  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *update_globalentity_stmt;

  char sql[512] = "UPDATE GlobalEntity SET";
  char update_clause[512] = "";
  char where_clause[256] = " WHERE 1=1";

  Result globalentity_exists = get_globalentity_db(type, local_id, id);

  if (globalentity_exists.type != SUCCESS &&
      globalentity_exists.type != EMPTY) {
    return (Result){.type = globalentity_exists.type,
                    .value = globalentity_exists.value};
  }

  if (globalentity_exists.type == EMPTY) {
    return (Result){.type = SIMPLE_ERR, .value = "GlobalEntity not found."};
  }

  for (int i = 0; ((GlobalEntity **)globalentity_exists.value)[i] != NULL;
       i++) {
    free(((GlobalEntity **)globalentity_exists.value)[i]->type);
    free(((GlobalEntity **)globalentity_exists.value)[i]);
  }

  free(globalentity_exists.value);

  int to_update = 0;

  if (globalentity.type != NULL)
    to_update++;

  if (globalentity.local_id != 0)
    to_update++;

  int added_to = 0;

  if (globalentity.type != NULL) {
    added_to++;
    strcat(update_clause, " type = ?");

    if (added_to != to_update && to_update != 1) {
      strcat(update_clause, ",");
    }
  }

  if (globalentity.local_id != 0) {
    added_to++;
    strcat(update_clause, " local_id = ?");

    if (added_to != to_update && to_update != 1) {
      strcat(update_clause, ",");
    }
  }

  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }

  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }

  if (local_id != 0) {
    strcat(where_clause, " AND local_id = ?");
  }

  strcat(where_clause, ";");
  strcat(sql, update_clause);
  strcat(sql, where_clause);

  int prepare_stat =
      sqlite3_prepare_v2(db, sql, -1, &update_globalentity_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  int bind_index = 1;

  if (globalentity.type != NULL) {
    sqlite3_bind_text(update_globalentity_stmt, bind_index++, globalentity.type,
                      -1, SQLITE_STATIC);
  }

  if (globalentity.local_id != 0) {
    sqlite3_bind_int64(update_globalentity_stmt, bind_index++,
                       globalentity.local_id);
  }

  if (type != NULL) {
    sqlite3_bind_text(update_globalentity_stmt, bind_index++, type, -1,
                      SQLITE_STATIC);
  }

  if (id != 0) {
    sqlite3_bind_int64(update_globalentity_stmt, bind_index++, id);
  }

  if (local_id != 0) {
    sqlite3_bind_int64(update_globalentity_stmt, bind_index++, local_id);
  }

  int step_stat = sqlite3_step(update_globalentity_stmt);

  if (step_stat != SQLITE_DONE) {
    if (sqlite3_finalize(update_globalentity_stmt) != SQLITE_OK) {
      return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
    }

    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  int finalize_stat = sqlite3_finalize(update_globalentity_stmt);

  if (finalize_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = ""};
}
