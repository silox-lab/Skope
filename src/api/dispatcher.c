/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#include "../../include/errors.h"
#include "../../include/api/wiki_handler.h"
#include "../../include/api/scope_handler.h"
#include "../../include/api/scopefile_handler.h"
#include "../../include/api/relation_handler.h"
#include <cjson/cJSON.h>
#include <string.h>
#include <stdbool.h>

Result dispatch_wiki_request(const char *json) {
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

    return (Result) { .type = EMPTY, .value = NULL };
}

Result dispatch_scope_request(const char *json) {
    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in deserialization." };
    }

    cJSON *method =
        cJSON_GetObjectItemCaseSensitive(root, "method");

    cJSON *value =
        cJSON_GetObjectItemCaseSensitive(root, "value");

    if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
        cJSON_Delete(root);
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    Result result;

    if (strcmp(method->valuestring, "get_scope_db") == 0) {
        result = handle_get_scope_db(value);
    }
    else if (strcmp(method->valuestring, "create_scope_db") == 0) {
        result = handle_create_scope_db(value);
    }
    else if (strcmp(method->valuestring, "delete_scope_db") == 0) {
        result = handle_delete_scope_db(value);
    }
    else if (strcmp(method->valuestring, "update_scope_db") == 0) {
        result = handle_update_scope_db(value);
    }
    else {
        result = (Result) { .type = EMPTY, .value = NULL };
    }

    cJSON_Delete(root);
    return result;
}

Result dispatch_scopefile_request(const char *json) {

    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in deserialization."
        };
    }

    cJSON *method =
        cJSON_GetObjectItemCaseSensitive(root, "method");

    cJSON *value =
        cJSON_GetObjectItemCaseSensitive(root, "value");

    if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
        cJSON_Delete(root);
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling."
        };
    }

    Result result;

    if (strcmp(method->valuestring, "get_scopefile_db") == 0) {
        result = handle_get_scopefile_db(value);
    }
    else if (strcmp(method->valuestring, "create_scopefile_db") == 0) {
        result = handle_create_scopefile_db(value);
    }
    else if (strcmp(method->valuestring, "delete_scopefile_db") == 0) {
        result = handle_delete_scopefile_db(value);
    }
    else if (strcmp(method->valuestring, "update_scopefile_db") == 0) {
        result = handle_update_scopefile_db(value);
    }
    else {
        result = (Result) {
            .type = EMPTY,
            .value = NULL
        };
    }

    cJSON_Delete(root);
    return result;
}


Result dispatch_relation_request(const char *json) {

    cJSON *root = cJSON_Parse(json);

    if (root == NULL) {
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in deserialization"
        };
    }

    cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");
    cJSON *value = cJSON_GetObjectItemCaseSensitive(root, "value");

    if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
        cJSON_Delete(root);
        return (Result) {
            .type = SIMPLE_ERR,
            .value = "failure in handling"
        };
    }

    Result result;

    if (strcmp(method->valuestring, "get_relation_db") == 0) {
        result = handle_get_relation_db(value);
    } else if (strcmp(method->valuestring, "create_relation_db") == 0) {
        result = handle_create_relation_db(value);
    } else if (strcmp(method->valuestring, "delete_relation_db") == 0) {
        result = handle_delete_relation_db(value);
    } else if (strcmp(method->valuestring, "update_relation_db") == 0) {
        result = handle_update_relation_db(value);
    } else {
        result = (Result) {
            .type = EMPTY,
            .value = NULL
        };
    }

    cJSON_Delete(root);
    return result;
}