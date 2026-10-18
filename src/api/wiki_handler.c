/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#include <cjson/cJSON.h>
#include <stdint.h>
#include <stdbool.h>
#include "../../include/database/wiki_db.h"
#include "../../include/api/serializer.h"

Result handle_get_wiki_db(cJSON *value) {

    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    if ((name != NULL && !cJSON_IsString(name)) ||
        (id != NULL && !cJSON_IsNumber(id))) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    if (name == NULL && id == NULL) {
        return (Result) { .type = SIMPLE_ERR, .value = "both name and id can not be empty" };
    }

    char *wiki_name = name ? name->valuestring : NULL;
    int64_t wiki_id = id ? (int64_t)id->valuedouble : 0;

    Result wiki_arr = get_wiki_db(wiki_name, wiki_id);
    
    char *response = wiki_arr_to_json(wiki_arr);
    return (Result) { .type = SUCCESS, .value = response };
}

Result handle_create_wiki_db(cJSON *value) {

    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *description =
        cJSON_GetObjectItemCaseSensitive(value, "description");

    if (!cJSON_IsString(name) ||
        !cJSON_IsString(description)) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    Wiki wiki = {.name = name->valuestring,
                 .description = description->valuestring};

    char *response = nodata_result_to_json(create_wiki_db(&wiki));
    return (Result) { .type = SUCCESS, .value = response };
}

Result handle_get_all_wikis(Result *out) {

    char *response = wiki_arr_to_json(get_all_wikis());
    return (Result) { .type = SUCCESS, .value = response };
}

Result handle_delete_wiki_db(cJSON *value) {

    cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

    if (!cJSON_IsNumber(id)) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    char *response =
        nodata_result_to_json(delete_wiki_db((int64_t)id->valuedouble));
    return (Result) {
      .type = SUCCESS, .value = response };
}

Result handle_update_wiki_db(cJSON *value) {
    
    cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
    cJSON *description =
        cJSON_GetObjectItemCaseSensitive(value, "description");

    if (!cJSON_IsString(name) ||
        !cJSON_IsString(description)) {
        return (Result) { .type = SIMPLE_ERR, .value = "failure in handling." };
    }

    Wiki wiki = {
        .name = name->valuestring,
        .description = description->valuestring
    };

    char *response =
        nodata_result_to_json(update_wiki_db(name->valuestring, wiki));
    return (Result) { .type = SUCCESS, .value = response };
}
