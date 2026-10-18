/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../include/database/scopefile_db.h"
#include "../../include/api/serializer.h"

Result handle_get_scopefile_db(cJSON *value) {

    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *scope_id = cJSON_GetObjectItemCaseSensitive(value, "scope_id");

    if ((name != NULL && !cJSON_IsString(name)) ||
        (scope_id != NULL && !cJSON_IsNumber(scope_id))) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling."
        };
    }

    char *scopefile_name = name ? name->valuestring : NULL;
    int64_t scopefile_scope_id =
        scope_id ? (int64_t)scope_id->valuedouble : 0;

    return (Result) { .type = SUCCESS, .value = scopefile_arr_to_json(get_scopefile_db(scopefile_name, scopefile_scope_id)) };
}

Result handle_create_scopefile_db(cJSON *value) {

    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *extension =
        cJSON_GetObjectItemCaseSensitive(value, "extension");
    cJSON *scope_id = cJSON_GetObjectItemCaseSensitive(value, "scope_id");

    if (!cJSON_IsString(name) ||
        !cJSON_IsString(extension) ||
        !cJSON_IsNumber(scope_id)) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling."
        };
    }

    ScopeFile scopefile = {
        .name = name->valuestring,
        .extension = extension->valuestring,
        .scope_id = (int64_t)scope_id->valuedouble
    };

    return (Result) {.type = SUCCESS, .value = nodata_result_to_json(create_scopefile_db(&scopefile)) };
}

Result handle_delete_scopefile_db(cJSON *value) {

    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    if (!cJSON_IsNumber(id)) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling."
        };
    }

    return (Result) { .type = SUCCESS, .value = nodata_result_to_json(delete_scopefile_db((int64_t)id->valuedouble)) };
}

Result handle_update_scopefile_db(cJSON *value) {

    cJSON *current_name =
        cJSON_GetObjectItemCaseSensitive(value, "current_name");
    cJSON *scope_id =
        cJSON_GetObjectItemCaseSensitive(value, "scope_id");
    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *extension =
        cJSON_GetObjectItemCaseSensitive(value, "extension");
    cJSON *new_scope_id =
        cJSON_GetObjectItemCaseSensitive(value, "new_scope_id");

    if (!cJSON_IsString(current_name) ||
        !cJSON_IsNumber(scope_id) ||
        (name != NULL && !cJSON_IsString(name)) ||
        (extension != NULL && !cJSON_IsString(extension)) ||
        (new_scope_id != NULL && !cJSON_IsNumber(new_scope_id))) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling."
        };
    }

    if (name == NULL && extension == NULL && new_scope_id == NULL) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "at least one field to update must be provided"
        };
    }

    ScopeFile scopefile = {
        .name = name ? name->valuestring : NULL,
        .extension = extension ? extension->valuestring : NULL,
        .scope_id = new_scope_id
            ? (int64_t)new_scope_id->valuedouble
            : 0
    };

    return (Result) { .type = SUCCESS, .value = nodata_result_to_json(update_scopefile_db(
        current_name->valuestring,
        (int64_t)scope_id->valuedouble,
        scopefile
											  )) };
}
