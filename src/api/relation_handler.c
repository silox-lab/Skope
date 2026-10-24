/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/api/serializer.h"
#include "../../include/database/relation_db.h"
#include "../../include/errors.h"
#include <cjson/cJSON.h>
#include <stdint.h>

Result handle_get_relation_db(cJSON *value) {
  cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
  cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
  cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");
  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

  int64_t source = source_id ? source_id->valuedouble : 0;
  int64_t target = target_id ? target_id->valuedouble : 0;
  int64_t relation_id = id ? id->valuedouble : 0;

  if (!cJSON_IsNumber(id) || (type != NULL && !cJSON_IsString(type)) ||
      (source_id != NULL && !cJSON_IsNumber(source_id)) ||
      (target_id != NULL && !cJSON_IsNumber(target_id)) ||
      (id != NULL && !cJSON_IsNumber(id))) {
    return (Result){.type = SIMPLE_ERR, .value = "failure in handling."};
  }

  if (source == 0 && target == 0 && type == NULL && relation_id == 0) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "at least one relation filter must be provided"};
  }

  return (Result){.type = SUCCESS,
                  .value = relation_arr_to_json(get_relation_db(
                      source, target, type != NULL ? type->valuestring : NULL,
                      relation_id))};
}

Result handle_create_relation_db(cJSON *value) {
  cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
  cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
  cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");

  int64_t source = source_id ? source_id->valuedouble : 0;
  int64_t target = target_id ? target_id->valuedouble : 0;

  if (!cJSON_IsNumber(source_id) || !cJSON_IsNumber(target_id) ||
      !cJSON_IsString(type) ||
      (source_id != NULL && !cJSON_IsNumber(source_id)) ||
      (target_id != NULL && !cJSON_IsNumber(target_id))) {
    return (Result){.type = SIMPLE_ERR, .value = "failure in handling."};
  }

  if (source == 0 && target == 0 && type == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "at least one relation filter must be provided"};
  }

  Relation relation = {
      .source_id = source, .target_id = target, .type = type->valuestring};

  return (Result){.type = SUCCESS,
                  .value =
                      nodata_result_to_json(create_relation_db(&relation))};
}

Result handle_delete_relation_db(cJSON *value) {
  cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
  cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
  cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");
  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");

  int64_t source = source_id ? source_id->valuedouble : 0;
  int64_t target = target_id ? target_id->valuedouble : 0;
  int64_t relation_id = id ? id->valuedouble : 0;

  if (source == 0 && target == 0 && type == NULL && relation_id == 0) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "at least one relation filter must be provided"};
  }

  return (Result){.type = SUCCESS,
                  .value = nodata_result_to_json(delete_relation_db(
                      source, target, type != NULL ? type->valuestring : NULL,
                      relation_id))};
}

Result handle_update_relation_db(cJSON *value) {
  cJSON *id = cJSON_GetObjectItemCaseSensitive(value, "id");
  cJSON *source_id = cJSON_GetObjectItemCaseSensitive(value, "source_id");
  cJSON *target_id = cJSON_GetObjectItemCaseSensitive(value, "target_id");
  cJSON *type = cJSON_GetObjectItemCaseSensitive(value, "type");

  int64_t source = source_id ? source_id->valuedouble : 0;
  int64_t target = target_id ? target_id->valuedouble : 0;
  int64_t relation_id = id ? id->valuedouble : 0;

  if (source == 0 && target == 0 && type == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "no relation fields to update"};
  }

  Relation relation = {.source_id = source,
                       .target_id = target,
                       .type = type != NULL ? type->valuestring : NULL};

  return (Result){.type = SUCCESS,
                  .value = nodata_result_to_json(
                      update_relation_db(0, 0, NULL, relation_id, relation))};
}
