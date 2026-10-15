/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include <cjson/cJSON.h>
#include <stdint.h>
#include "../../include/errors.h"
#include "../../include/database/relation_db.h"

Result handle_get_relation_db(cJSON *value) {
    cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
    cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    int64_t source = 0;
    int64_t target = 0;
    int64_t relation_id = 0;

    if (source == 0 && target == 0 &&
        type == NULL && relation_id == 0) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "at least one relation filter must be provided"
        };
    }

    return get_relation_db(
        source,
        target,
        type != NULL ? type->valuestring : NULL,
        relation_id
    );
}

Result handle_create_relation_db(cJSON *value) {
    cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
    cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");

    int64_t source = 0;
    int64_t target = 0;

    Relation relation = {
        .source_id = source,
        .target_id = target,
        .type = type->valuestring
    };

    return create_relation_db(&relation);
}

Result handle_delete_relation_db(cJSON *value) {
    cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
    cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    int64_t source = 0;
    int64_t target = 0;
    int64_t relation_id = 0;

    if (source == 0 && target == 0 &&
        type == NULL && relation_id == 0) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "at least one relation filter must be provided"
        };
    }

    return delete_relation_db(
        source,
        target,
        type != NULL ? type->valuestring : NULL,
        relation_id
    );
}

Result handle_update_relation_db(cJSON *value) {
    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
    cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
    cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
    cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");

    int64_t relation_id = 0;
    int64_t source = 0;
    int64_t target = 0;

    if (source == 0 && target == 0 && type == NULL) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "no relation fields to update"
        };
    }

    Relation relation = {
        .source_id = source,
        .target_id = target,
        .type = type != NULL ? type->valuestring : NULL
    };

    return update_relation_db(
        0,
        0,
        NULL,
        relation_id,
        relation
    );
}