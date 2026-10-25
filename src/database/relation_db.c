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

Result get_relation_db(int64_t source_id, int64_t target_id, char *type,
                       int64_t id) {
  sqlite3_stmt *get_relation_stmt;
  sqlite3 *db = GET_W_DATABASE();

  if (source_id == 0 && target_id == 0 && type == NULL && id == 0) {
    return (Result){
        .type = SIMPLE_ERR,
        .value =
            "get_relation_db: at least one relation filter must be provided"};
  }

  int arr_length = 0;
  int arr_capacity = 10;
  Relation **relation_arr = malloc(sizeof(Relation *) * arr_capacity);

  if (relation_arr == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "get_relation_db: memory allocation failed"};
  }

  char sql[512] = "SELECT * FROM Relation WHERE 1=1 ";
  char where_clause[512] = "";

  int bind_index = 1;

  if (source_id != 0) {
    strcat(where_clause, " AND source_id = ?");
  }
  if (target_id != 0) {
    strcat(where_clause, " AND target_id = ?");
  }
  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }
  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }

  strcat(where_clause, ";");
  strcat(sql, where_clause);

  int prepare_stat = sqlite3_prepare_v2(db, sql, -1, &get_relation_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    free(relation_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  if (source_id != 0) {
    sqlite3_bind_int64(get_relation_stmt, bind_index++, source_id);
  }
  if (target_id != 0) {
    sqlite3_bind_int64(get_relation_stmt, bind_index++, target_id);
  }
  if (type != NULL) {
    sqlite3_bind_text(get_relation_stmt, bind_index++, type, -1, SQLITE_STATIC);
  }
  if (id != 0) {
    sqlite3_bind_int64(get_relation_stmt, bind_index++, id);
  }

  int step_result;

  while ((step_result = sqlite3_step(get_relation_stmt)) == SQLITE_ROW) {
    Relation *relation = malloc(sizeof(Relation));

    if (relation == NULL) {
      sqlite3_finalize(get_relation_stmt);

      for (int i = 0; i < arr_length; i++) {
        free(relation_arr[i]->type);
        free(relation_arr[i]);
      }

      free(relation_arr);
      return (Result){.type = SIMPLE_ERR,
                      .value = "get_relation_db: memory allocation failed"};
    }

    if (arr_length >= arr_capacity) {
      arr_capacity *= 2;
      Relation **temp =
          realloc(relation_arr, sizeof(Relation *) * arr_capacity);

      if (temp == NULL) {
        free(relation);
        sqlite3_finalize(get_relation_stmt);

        for (int i = 0; i < arr_length; i++) {
          free(relation_arr[i]->type);
          free(relation_arr[i]);
        }

        free(relation_arr);
        return (Result){.type = SIMPLE_ERR,
                        .value = "get_relation_db: memory reallocation failed"};
      }

      relation_arr = temp;
    }

    relation->id = sqlite3_column_int64(get_relation_stmt, 0);
    relation->source_id = sqlite3_column_int64(get_relation_stmt, 1);
    relation->target_id = sqlite3_column_int64(get_relation_stmt, 2);

    const char *type_text =
        (const char *)sqlite3_column_text(get_relation_stmt, 3);

    relation->type = type_text ? strdup(type_text) : NULL;
    relation->created_on = sqlite3_column_int64(get_relation_stmt, 4);

    relation_arr[arr_length] = relation;
    arr_length++;
  }

  if (arr_length == 0) {
    sqlite3_finalize(get_relation_stmt);
    free(relation_arr);

    if (step_result != SQLITE_DONE) {
      return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
    }

    return (Result){.type = EMPTY, .value = NULL};
  }

  relation_arr[arr_length] = NULL;
  arr_length++;

  if (step_result != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    sqlite3_finalize(get_relation_stmt);

    for (int i = 0; i < arr_length - 1; i++) {
      free(relation_arr[i]->type);
      free(relation_arr[i]);
    }

    free(relation_arr);
    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(get_relation_stmt);

  if (finalize_stat != SQLITE_OK) {
    for (int i = 0; i < arr_length - 1; i++) {
      free(relation_arr[i]->type);
      free(relation_arr[i]);
    }

    free(relation_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = relation_arr};
}

Result create_relation_db(Relation *relation) {
  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *create_stmt;

  const char *sql =
      "INSERT INTO Relation(source_id, target_id, type, created_on) "
      "VALUES (?, ?, ?, ?);";

  Result relation_exist = get_relation_db(
      relation->source_id, relation->target_id, relation->type, 0);

  if (relation_exist.type != SUCCESS && relation_exist.type != EMPTY) {
    return (Result){.type = relation_exist.type, .value = relation_exist.value};
  }

  Relation **relation_exist_arr = relation_exist.value;

  if (relation_exist.type != EMPTY) {
    for (int i = 0; relation_exist_arr[i] != NULL; i++) {
      free(relation_exist_arr[i]->type);
      free(relation_exist_arr[i]);
    }

    free(relation_exist_arr);

    return (Result){.type = SIMPLE_ERR,
                    .value = "create_relation_db: relation with same source, "
                             "target, and type exists"};
  }

  int prepare_stat = sqlite3_prepare_v2(db, sql, -1, &create_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    free(relation_exist_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  sqlite3_bind_int64(create_stmt, 1, relation->source_id);
  sqlite3_bind_int64(create_stmt, 2, relation->target_id);
  sqlite3_bind_text(create_stmt, 3, relation->type, -1, SQLITE_STATIC);
  sqlite3_bind_int64(create_stmt, 4, (int64_t)time(NULL));

  int step_stat = sqlite3_step(create_stmt);

  if (step_stat != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    sqlite3_finalize(create_stmt);
    free(relation_exist_arr);

    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(create_stmt);

  if (finalize_stat != SQLITE_OK) {
    free(relation_exist_arr);
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  free(relation_exist_arr);

  return (Result){.type = SUCCESS, .value = ""};
}

Result delete_relation_db(int64_t source_id, int64_t target_id, char *type,
                          int64_t id) {
  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *delete_relation_stmt;

  char sql[512] = "DELETE FROM Relation WHERE 1=1 ";
  char where_clause[512] = "";

  int bind_index = 1;

  if (source_id != 0) {
    strcat(where_clause, " AND source_id = ?");
  }
  if (target_id != 0) {
    strcat(where_clause, " AND target_id = ?");
  }
  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }
  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }

  if (source_id == 0 && target_id == 0 && type == NULL && id == 0) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "delete_relation_db: at least one relation filter "
                             "must be provided"};
  }

  strcat(where_clause, ";");
  strcat(sql, where_clause);

  int prepare_stat =
      sqlite3_prepare_v2(db, sql, -1, &delete_relation_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  if (source_id != 0) {
    sqlite3_bind_int64(delete_relation_stmt, bind_index++, source_id);
  }
  if (target_id != 0) {
    sqlite3_bind_int64(delete_relation_stmt, bind_index++, target_id);
  }
  if (type != NULL) {
    sqlite3_bind_text(delete_relation_stmt, bind_index++, type, -1,
                      SQLITE_STATIC);
  }
  if (id != 0) {
    sqlite3_bind_int64(delete_relation_stmt, bind_index++, id);
  }

  int step_stat = sqlite3_step(delete_relation_stmt);

  if (step_stat != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    sqlite3_finalize(delete_relation_stmt);
    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(delete_relation_stmt);

  if (finalize_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = ""};
}

Result update_relation_db(int64_t source_id, int64_t target_id, char *type,
                          int64_t id, Relation relation) {
  sqlite3 *db = GET_W_DATABASE();
  sqlite3_stmt *update_relation_stmt;

  char sql[512] = "UPDATE Relation SET";
  char update_clause[512] = "";
  char where_clause[256] = " WHERE 1=1";

  Result relation_exists = get_relation_db(source_id, target_id, type, id);

  if (relation_exists.type != SUCCESS && relation_exists.type != EMPTY) {
    return (Result){.type = relation_exists.type,
                    .value = relation_exists.value};
  }

  if (relation_exists.type == EMPTY) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "update_relation_db: Relation not found."};
  }

  Relation **relation_exists_arr = relation_exists.value;

  for (int i = 0; relation_exists_arr[i] != NULL; i++) {
    free(relation_exists_arr[i]->type);
    free(relation_exists_arr[i]);
  }

  free(relation_exists_arr);

  int to_update = 0;

  if (relation.source_id != 0)
    to_update++;

  if (relation.target_id != 0)
    to_update++;

  if (relation.type != NULL)
    to_update++;

  if (to_update == 0) {
    return (Result){.type = SIMPLE_ERR,
                    .value =
                        "update_relation_db: no relation fields to update"};
  }

  int added_to = 0;

  if (relation.source_id != 0) {
    added_to++;
    strcat(update_clause, " source_id = ?");
    if (added_to != to_update) {
      strcat(update_clause, ",");
    }
  }

  if (relation.target_id != 0) {
    added_to++;
    strcat(update_clause, " target_id = ?");
    if (added_to != to_update) {
      strcat(update_clause, ",");
    }
  }

  if (relation.type != NULL) {
    added_to++;
    strcat(update_clause, " type = ?");
    if (added_to != to_update) {
      strcat(update_clause, ",");
    }
  }

  if (source_id != 0) {
    strcat(where_clause, " AND source_id = ?");
  }
  if (target_id != 0) {
    strcat(where_clause, " AND target_id = ?");
  }
  if (type != NULL) {
    strcat(where_clause, " AND type = ?");
  }
  if (id != 0) {
    strcat(where_clause, " AND id = ?");
  }

  strcat(where_clause, ";");
  strcat(sql, update_clause);
  strcat(sql, where_clause);

  int prepare_stat =
      sqlite3_prepare_v2(db, sql, -1, &update_relation_stmt, NULL);

  if (prepare_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  int bind_index = 1;

  if (relation.source_id != 0) {
    sqlite3_bind_int64(update_relation_stmt, bind_index++, relation.source_id);
  }
  if (relation.target_id != 0) {
    sqlite3_bind_int64(update_relation_stmt, bind_index++, relation.target_id);
  }
  if (relation.type != NULL) {
    sqlite3_bind_text(update_relation_stmt, bind_index++, relation.type, -1,
                      SQLITE_STATIC);
  }

  if (source_id != 0) {
    sqlite3_bind_int64(update_relation_stmt, bind_index++, source_id);
  }
  if (target_id != 0) {
    sqlite3_bind_int64(update_relation_stmt, bind_index++, target_id);
  }
  if (type != NULL) {
    sqlite3_bind_text(update_relation_stmt, bind_index++, type, -1,
                      SQLITE_STATIC);
  }
  if (id != 0) {
    sqlite3_bind_int64(update_relation_stmt, bind_index++, id);
  }

  int step_stat = sqlite3_step(update_relation_stmt);

  if (step_stat != SQLITE_DONE) {
    char *err_msg = strdup(sqlite3_errmsg(db));
    sqlite3_finalize(update_relation_stmt);
    return (Result){.type = DB_ERR, .value = err_msg};
  }

  int finalize_stat = sqlite3_finalize(update_relation_stmt);

  if (finalize_stat != SQLITE_OK) {
    return (Result){.type = DB_ERR, .value = strdup(sqlite3_errmsg(db))};
  }

  return (Result){.type = SUCCESS, .value = ""};
}
