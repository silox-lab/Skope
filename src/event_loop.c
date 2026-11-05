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
    sqlite3 *db = NULL;

    Scope **scope_arr = NULL;
    Scope **scope2_arr = NULL;
    ScopeFile **scopefile_arr = NULL;
    GlobalEntity **entity_arr = NULL;
    Relation **relation_arr = NULL;

    int failed = 0;

    INIT_DATABASE();
    INIT_STORAGE_PATH();

    db = GET_W_DATABASE();

    if (db == NULL) {
        printf("ERROR: database is NULL\n");
        return;
    }

    printf("DB filename: %s\n", sqlite3_db_filename(db, "main"));

    Result r = run_sql_db(DB_SCOPE);
    if (r.type != SUCCESS) {
        printf("Scope table creation failed: %s\n", (char *)r.value);
        failed = 1;
    }

    if (!failed) {
        r = run_sql_db(DB_SCOPE_FILE);
        if (r.type != SUCCESS) {
            printf("ScopeFile table creation failed: %s\n", (char *)r.value);
            failed = 1;
        }
    }

    if (!failed) {
        r = run_sql_db(DB_ENTITY);
        if (r.type != SUCCESS) {
            printf("GlobalEntity table creation failed: %s\n", (char *)r.value);
            failed = 1;
        }
    }

    if (!failed) {
        r = run_sql_db(DB_RELATION);
        if (r.type != SUCCESS) {
            printf("Relation table creation failed: %s\n", (char *)r.value);
            failed = 1;
        }
    }

    if (!failed) {
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
    }

    Scope s = {
        .name = "lithium ion",
        .parent_scope_id = 0,
    };

    if (!failed) {
        r = create_scope_db(&s);
        if (r.type != SUCCESS) {
            printf("create_scope_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            printf("Top-level scope created successfully.\n");
        }
    }

    if (!failed) {
        r = get_scope_db(0, 0, s.name);
        if (r.type != SUCCESS || r.value == NULL) {
            printf("get_scope_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            scope_arr = r.value;
        }
    }

    if (!failed && scope_arr[0] == NULL) {
        printf("ERROR: Scope result contains no Scope.\n");
        failed = 1;
    }

    Scope s2 = {
        .name = "ion3",
        .parent_scope_id = 0,
    };

    if (!failed) {
        s2.parent_scope_id = scope_arr[0]->id;

        r = create_scope_db(&s2);
        if (r.type != SUCCESS) {
            printf("create_scope_db (child) failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            printf("Child scope created successfully.\n");
        }
    }

    if (!failed) {
        r = get_scope_db(0, s2.parent_scope_id, s2.name);
        if (r.type != SUCCESS || r.value == NULL) {
            printf("get_scope_db (child) failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            scope2_arr = r.value;
        }
    }

    if (!failed && scope2_arr[0] == NULL) {
        printf("ERROR: Child scope result contains no Scope.\n");
        failed = 1;
    }

    ScopeFile sf = {
        .name = "hello_world",
        .scope_id = 0,
        .extension = "txt",
    };

    if (!failed) {
        sf.scope_id = scope2_arr[0]->id;

        r = create_scopefile_db(&sf);
        if (r.type != SUCCESS) {
            printf("create_scopefile_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            printf("ScopeFile created successfully.\n");
        }
    }

    if (!failed) {
        r = get_scopefile_db(sf.name, sf.scope_id);
        if (r.type != SUCCESS || r.value == NULL) {
            printf("get_scopefile_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            scopefile_arr = r.value;
        }
    }

    GlobalEntity ge = {
        .type = "Scope",
    };

    if (!failed) {
        r = get_globalentity_db(ge.type, ge.local_id, 0);
        if (r.type != SUCCESS || r.value == NULL) {
            printf("get_globalentity_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            entity_arr = r.value;
        }
    }

    if (!failed && entity_arr[0] == NULL) {
        printf("ERROR: GlobalEntity result contains no GlobalEntity.\n");
        failed = 1;
    }

    if (!failed) {
        printf("GlobalEntity retrieved successfully. ID: %lld\n",
               (long long)entity_arr[0]->id);
    }

    Relation rel = {
        .source_id = 0,
        .target_id = 0,
        .type = "references",
    };

    if (!failed) {
        rel.source_id = entity_arr[0]->id;
        rel.target_id = entity_arr[0]->id;

        r = create_relation_db(&rel);
        if (r.type != SUCCESS) {
            printf("create_relation_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            printf("Relation created successfully.\n");
        }
    }

    if (!failed) {
        r = get_relation_db(rel.source_id, rel.target_id, rel.type, 0);
        if (r.type != SUCCESS || r.value == NULL) {
            printf("get_relation_db failed: %s\n", (char *)r.value);
            failed = 1;
        } else {
            relation_arr = r.value;
        }
    }

    if (!failed && relation_arr[0] == NULL) {
        printf("ERROR: Relation result contains no Relation.\n");
        failed = 1;
    }

    if (!failed) {
        printf("Relation retrieved successfully. ID: %lld\n",
               (long long)relation_arr[0]->id);

        Result socket_errors = setup_socket();
        if (socket_errors.type != SUCCESS)
            printf("socket error: %s\n", (char *)socket_errors.value);
    }

    /* Free all query results, including on error paths. */
    if (scope_arr != NULL) {
        for (int i = 0; scope_arr[i] != NULL; i++) {
            free(scope_arr[i]->name);
            free(scope_arr[i]);
        }
        free(scope_arr);
    }

    if (scope2_arr != NULL) {
        for (int i = 0; scope2_arr[i] != NULL; i++) {
            free(scope2_arr[i]->name);
            free(scope2_arr[i]);
        }
        free(scope2_arr);
    }

    if (scopefile_arr != NULL) {
        for (int i = 0; scopefile_arr[i] != NULL; i++) {
            free(scopefile_arr[i]->name);
            free(scopefile_arr[i]->extension);
            free(scopefile_arr[i]);
        }
        free(scopefile_arr);
    }

    if (entity_arr != NULL) {
        for (int i = 0; entity_arr[i] != NULL; i++) {
            free(entity_arr[i]->type);
            free(entity_arr[i]);
        }
        free(entity_arr);
    }

    if (relation_arr != NULL) {
        for (int i = 0; relation_arr[i] != NULL; i++) {
            free(relation_arr[i]->type);
            free(relation_arr[i]);
        }
        free(relation_arr);
    }

    sqlite3_close(db);
}
