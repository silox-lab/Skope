#include "../../include/globals.h"
#include "../../include/errors.h"
#include "../../include/schema.h"
#include "../../include/triggers.h"
#include "../../include/database/general.h"
#include <stdio.h>
#include <stdlib.h>

Result sqlite_mock_db() {
  
  char *db_path = ":memory:";
  
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

  INIT_DATABASE(db);
  INIT_STORAGE_PATH();
  
  Result make_scope_table = run_sql_db(DB_SCOPE);
  if (make_scope_table.type != SUCCESS) return make_scope_table;
  
  Result make_scopefile_table = run_sql_db(DB_SCOPE_FILE);
  if (make_scopefile_table.type != SUCCESS) return make_scopefile_table;
  
  Result make_globalentity_table = run_sql_db(DB_ENTITY);
  if (make_globalentity_table.type != SUCCESS) return make_globalentity_table;
  
  Result make_relation_table = run_sql_db(DB_RELATION);
  if (make_relation_table.type != SUCCESS) return make_relation_table;

  
  Result make_scope_trigger1 = run_sql_db(DB_SCOPE_TRG1);
  if (make_scope_trigger1.type != SUCCESS) return make_scope_trigger1;
  
  Result make_scope_trigger2 = run_sql_db(DB_SCOPE_TRG2);
  if (make_scope_trigger2.type != SUCCESS) return make_scope_trigger2;  
  
  Result make_scopefile_trigger1 = run_sql_db(DB_SCOPE_FILE_TRG1);
  if (make_scopefile_trigger1.type != SUCCESS) return make_scopefile_trigger1;
  
  Result make_scopefile_trigger2 = run_sql_db(DB_SCOPE_FILE_TRG2);
  if (make_scopefile_trigger2.type != SUCCESS) return make_scopefile_trigger2;

  return (Result) { .type = SUCCESS, .value = "" };
}
