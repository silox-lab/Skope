/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../include/api/socket.h"
#include "../include/database/entity_db.h"
#include "../include/database/general.h"
#include "../include/database/relation_db.h"
#include "../include/database/scope_db.h"
#include "../include/database/scopefile_db.h"
#include "../include/errors.h"
#include "../include/globals.h"
#include "../include/schema.h"
#include "../include/triggers.h"
#include <ncurses.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

void run_event_loop(void) {

  INIT_DATABASE();
  INIT_STORAGE_PATH();

  sqlite3 *db = GET_W_DATABASE();

  if (db == NULL) {
    printf("ERROR: database is NULL\n");
    return;
  }

  printf("DB filename: %s\n", sqlite3_db_filename(db, "main"));

  Result r;

  r = run_sql_db(DB_SCOPE);

  if (r.type != SUCCESS) {
    printf("Scope table creation failed: %s\n", (char *)r.value);
    return;
  }

  r = run_sql_db(DB_SCOPE_FILE);

  if (r.type != SUCCESS) {
    printf("ScopeFile table creation failed: %s\n", (char *)r.value);
    return;
  }

  r = run_sql_db(DB_ENTITY);

  if (r.type != SUCCESS) {
    printf("GlobalEntity table creation failed: %s\n", (char *)r.value);
    return;
  }

  r = run_sql_db(DB_RELATION);

  if (r.type != SUCCESS) {
    printf("Relation table creation failed: %s\n", (char *)r.value);
    return;
  }

  Result trg1_s = run_sql_db(DB_SCOPE_TRG1);
  if (trg1_s.type != SUCCESS)
    printf("trg1_s: %s\n", (char *)trg1_s.value);

  Result trg2_s = run_sql_db(DB_SCOPE_TRG2);
  if (trg2_s.type != SUCCESS)
    printf("trg2_s: %s\n", (char *)trg2_s.value);

  Result trg1_sf = run_sql_db(DB_SCOPE_FILE_TRG1);
  if (trg1_sf.type != SUCCESS)
    printf("trg1_sf: %s\n", (char *)trg1_sf.value);

  Result trg2_sf = run_sql_db(DB_SCOPE_FILE_TRG2);
  if (trg2_sf.type != SUCCESS)
    printf("trg2_sf: %s\n", (char *)trg2_sf.value);

  Scope s = {
      .name = "lithium ion",
      .parent_scope_id = 0,
  };

  Result make_s = create_scope_db(&s);

  if (make_s.type != SUCCESS) {

    printf("create_scope_db failed: %s\n", (char *)make_s.value);

    return;
  }

  printf("Top-level scope created successfully.\n");

  Result get_s = get_scope_db(0, 0, s.name);

  if (get_s.type != SUCCESS) {

    printf("get_scope_db failed: %s\n", (char *)get_s.value);

    return;
  }

  if (get_s.value == NULL) {

    printf("ERROR: Scope was created but could not be found.\n");

    return;
  }

  Scope **scope_arr = (Scope **)get_s.value;

  if (scope_arr[0] == NULL) {

    printf("ERROR: Scope result contains no Scope.\n");

    return;
  }

  Scope *scope1 = scope_arr[0];

  Scope s2 = {
      .name = "ion3",
      .parent_scope_id = scope1->id,
  };

  Result make_s2 = create_scope_db(&s2);

  if (make_s2.type != SUCCESS) {

    printf("create_scope_db (child) failed: %s\n", (char *)make_s2.value);

    return;
  }

  printf("Child scope created successfully.\n");

  Result get_s2 = get_scope_db(0, scope1->id, s2.name);

  if (get_s2.type != SUCCESS) {

    printf("get_scope_db (child) failed: %s\n", (char *)get_s2.value);

    return;
  }

  if (get_s2.value == NULL) {

    printf("ERROR: Child scope was created but not found.\n");

    return;
  }

  Scope **scope2_arr = (Scope **)get_s2.value;

  if (scope2_arr[0] == NULL) {

    printf("ERROR: Child scope result contains no Scope.\n");

    return;
  }

  Scope *scope2 = scope2_arr[0];

  ScopeFile sf = {
      .name = "hello_world", .scope_id = scope2->id, .extension = "txt"};

  Result make_sf = create_scopefile_db(&sf);

  if (make_sf.type != SUCCESS) {

    printf("create_scopefile_db failed: %s\n", (char *)make_sf.value);

    return;
  }

  printf("ScopeFile created successfully.\n");

  Result get_sf = get_scopefile_db(sf.name, sf.scope_id);

  if (get_sf.type != SUCCESS) {

    printf("get_scopefile_db failed: %s\n", (char *)get_sf.value);

    return;
  }

  if (get_sf.value == NULL) {

    printf("ERROR: ScopeFile was created but could not be found.\n");

    return;
  }

  ScopeFile *sf_from_db = ((ScopeFile **)get_sf.value)[0];

  GlobalEntity ge = {
      .type = "Scope",
  };

  Result get_ge = get_globalentity_db(ge.type, ge.local_id, 0);

  if (get_ge.type != SUCCESS) {
    printf("get_globalentity_db failed: %s\n", (char *)get_ge.value);
    return;
  }

  if (get_ge.value == NULL) {
    printf("ERROR: GlobalEntity was created but could not be found.\n");
    return;
  }

  GlobalEntity **globalentity_arr = (GlobalEntity **)get_ge.value;

  if (globalentity_arr[0] == NULL) {
    printf("ERROR: GlobalEntity result contains no GlobalEntity.\n");
    return;
  }

  GlobalEntity *globalentity = globalentity_arr[0];

  printf("GlobalEntity retrieved successfully. ID: %lld\n",
         (long long)globalentity->id);

  sqlite3_close(GET_W_DATABASE());

  Relation rel = {.source_id = globalentity->id,
                  .target_id = globalentity->id,
                  .type = "references"};

  Result make_rel = create_relation_db(&rel);

  if (make_rel.type != SUCCESS) {
    printf("create_relation_db failed: %s\n", (char *)make_rel.value);
    return;
  }

  printf("Relation created successfully.\n");

  Result get_rel = get_relation_db(rel.source_id, rel.target_id, rel.type, 0);

  if (get_rel.type != SUCCESS) {
    printf("get_relation_db failed: %s\n", (char *)get_rel.value);
    return;
  }

  if (get_rel.value == NULL) {
    printf("ERROR: Relation was created but could not be found.\n");
    return;
  }

  Relation **relation_arr = (Relation **)get_rel.value;

  if (relation_arr[0] == NULL) {
    printf("ERROR: Relation result contains no Relation.\n");
    return;
  }

  Relation *relation = relation_arr[0];

  printf("Relation retrieved successfully. ID: %lld\n",
         (long long)relation->id);

  Result socket_errors = setup_socket();
  if (socket_errors.type != SUCCESS) {
    printf("socket error: %s\n", (char *)socket_errors.value);
  }
  sqlite3_close(GET_W_DATABASE());
}
