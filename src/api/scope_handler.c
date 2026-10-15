/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../include/database/scope_db.h"

Result handle_get_scope_db(cJSON *value) {

    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
    cJSON *wiki_id = cJSON_GetObjectItemCaseSensitive(value, "wiki_id");
    cJSON *parent_scope_id =
        cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");

    if ((id != NULL && !cJSON_IsNumber(id)) ||
        (wiki_id != NULL && !cJSON_IsNumber(wiki_id)) ||
        (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id)) ||
        (name != NULL && !cJSON_IsString(name))) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    if (id == NULL && wiki_id == NULL &&
        parent_scope_id == NULL && name == NULL) {
        return (Result) { .type = SIMPLE_ERR, .value = "at least one parameter must be provided" };
    }

    int64_t scope_id = id ? (int64_t)id->valuedouble : 0;
    int64_t scope_wiki_id = wiki_id ? (int64_t)wiki_id->valuedouble : 0;
    int64_t scope_parent_id =
        parent_scope_id ? (int64_t)parent_scope_id->valuedouble : 0;
    char *scope_name = name ? name->valuestring : NULL;

    return get_scope_db(scope_id, scope_wiki_id, scope_parent_id, scope_name);
}

Result handle_create_scope_db(cJSON *value) {

    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *wiki_id = cJSON_GetObjectItemCaseSensitive(value, "wiki_id");
    cJSON *parent_scope_id =
        cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");

    if (!cJSON_IsString(name) ||
        !cJSON_IsNumber(wiki_id) ||
        (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id))) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    Wiki wiki = {
        .id = (int64_t)wiki_id->valuedouble
    };

    Scope scope = {
        .name = name->valuestring,
        .wiki_id = (int64_t)wiki_id->valuedouble,
        .parent_scope_id = parent_scope_id
            ? (int64_t)parent_scope_id->valuedouble
            : 0
    };

    return create_scope_db(&scope, &wiki);
}

Result handle_delete_scope_db(cJSON *value) {

    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    if (!cJSON_IsNumber(id)) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    return delete_scope_db((int64_t)id->valuedouble);
}

Result handle_update_scope_db(cJSON *value) {

    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *parent_scope_id =
        cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");
    cJSON *wiki_id = cJSON_GetObjectItemCaseSensitive(value, "wiki_id");

    if (!cJSON_IsNumber(id) ||
        (name != NULL && !cJSON_IsString(name)) ||
        (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id)) ||
        (wiki_id != NULL && !cJSON_IsNumber(wiki_id))) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    if (name == NULL && parent_scope_id == NULL && wiki_id == NULL) {
        return (Result) { .type = SIMPLE_ERR, .value = "at least one field to update must be provided" };
    }

    Scope scope = {
        .name = name ? name->valuestring : NULL,
        .parent_scope_id = parent_scope_id
            ? (int64_t)parent_scope_id->valuedouble
            : 0,
        .wiki_id = wiki_id ? (int64_t)wiki_id->valuedouble : 0
    };

    return update_scope_db((int64_t)id->valuedouble, scope);
}
