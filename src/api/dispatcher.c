/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#include "../../include/errors.h"
#include "../../include/api/handler.h"
#include <cjson/cJSON.h>
#include <string.h>
#include <stdbool.h>

Result deserialize_wiki_request(const char *json) {
    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in deserialization." };
    }

    cJSON *method =
        cJSON_GetObjectItemCaseSensitive(root, "method");

    cJSON *value =
        cJSON_GetObjectItemCaseSensitive(root, "value");

    if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    if (strcmp(method->valuestring, "get_wiki_db") == 0) {
        return handle_get_wiki_db(value);
    }
    else if (strcmp(method->valuestring, "create_wiki_db") == 0) {
        return handle_create_wiki_db(value);
    }
    else if (strcmp(method->valuestring, "get_all_wikis") == 0) {
        return handle_get_all_wikis(value);
    }
    else if (strcmp(method->valuestring, "delete_wiki_db") == 0) {
        return handle_delete_wiki_db(value);
    }
    else if (strcmp(method->valuestring, "update_wiki_db") == 0) {
        return handle_update_wiki_db(value);
    }

    return (Result) { .type = SIMPLE_ERR, .value = "failure in dispatching" };
}