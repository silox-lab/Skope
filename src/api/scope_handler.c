/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/api/serializer.h"
#include "../../include/database/scope_db.h"
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

Result handle_get_scope_db(cJSON *value) {

  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
  cJSON *parent_scope_id =
      cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");
  cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");

  if ((id != NULL && !cJSON_IsNumber(id)) ||
      (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id)) ||
      (name != NULL && !cJSON_IsString(name))) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "get_scope: failure in handling."};
  }

  if (id == NULL && parent_scope_id == NULL && name == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value =
                        "get_scope: at least one parameter must be provided"};
  }

  int64_t scope_id = id ? (int64_t)id->valuedouble : 0;
  int64_t scope_parent_id =
      parent_scope_id ? (int64_t)parent_scope_id->valuedouble : 0;
  char *scope_name = name ? name->valuestring : NULL;

  return (Result){.type = SUCCESS,
                  .value = scope_arr_to_json(
                      get_scope_db(scope_id, scope_parent_id, scope_name))};
}

Result handle_create_scope_db(cJSON *value) {

  cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
  cJSON *parent_scope_id =
      cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");

  if (!cJSON_IsString(name) ||
      (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id))) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "create_scope: failure in handling."};
  }

  Scope scope = {.name = name->valuestring,
                 .parent_scope_id = parent_scope_id
                                        ? (int64_t)parent_scope_id->valuedouble
                                        : 0};

  return (Result){.type = SUCCESS,
                  .value = nodata_result_to_json(create_scope_db(&scope))};
}

Result handle_delete_scope_db(cJSON *value) {

  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

  if (!cJSON_IsNumber(id)) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "delete_scope: failure in handling."};
  }

  return (Result){.type = SUCCESS,
                  .value = nodata_result_to_json(
                      delete_scope_db((int64_t)id->valuedouble))};
}

Result handle_update_scope_db(cJSON *value) {

  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
  cJSON *name = cJSON_GetObjectItemCaseSensitive(value, "name");
  cJSON *parent_scope_id =
      cJSON_GetObjectItemCaseSensitive(value, "parent_scope_id");

  if (!cJSON_IsNumber(id) || (name != NULL && !cJSON_IsString(name)) ||
      (parent_scope_id != NULL && !cJSON_IsNumber(parent_scope_id))) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "update_scope: failure in handling."};
  }

  if (name == NULL && parent_scope_id == NULL) {
    return (Result){
        .type = SIMPLE_ERR,
        .value = "update_scope: at least one field to update must be provided"};
  }

  Scope scope = {
      .name = name ? name->valuestring : NULL,
      .parent_scope_id =
          parent_scope_id ? (int64_t)parent_scope_id->valuedouble : 0,
  };

  return (Result){.type = SUCCESS,
                  .value = nodata_result_to_json(
                      update_scope_db((int64_t)id->valuedouble, scope))};
}
