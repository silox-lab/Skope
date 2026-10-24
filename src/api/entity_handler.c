/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/api/serializer.h"
#include "../../include/database/entity_db.h"
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <stdint.h>

Result handle_get_globalentity_db(cJSON *value) {

  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
  cJSON *local_id = cJSON_GetObjectItemCaseSensitive(value, "local_id");
  cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");

  if ((id != NULL && !cJSON_IsNumber(id)) ||
      (local_id != NULL && !cJSON_IsNumber(local_id)) ||
      (type != NULL && !cJSON_IsString(type))) {
    return (Result){.type = SIMPLE_ERR, .value = "failure in handling."};
  }

  if (id == NULL && local_id == NULL && type == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "at least one parameter must be provided"};
  }

  int64_t entity_id = id ? (int64_t)id->valuedouble : 0;
  int64_t entity_local_id = local_id ? (int64_t)local_id->valuedouble : 0;
  char *entity_type = type ? type->valuestring : NULL;

  return (Result){.type = SUCCESS,
                  .value = globalentity_arr_to_json(get_globalentity_db(
                      entity_type, entity_local_id, entity_id))};
}
