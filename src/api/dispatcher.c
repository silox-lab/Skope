/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/api/entity_handler.h"
#include "../../include/api/relation_handler.h"
#include "../../include/api/scope_handler.h"
#include "../../include/api/scopefile_handler.h"
#include "../../include/errors.h"
#include <cjson/cJSON.h>
#include <stdbool.h>
#include <string.h>

Result dispatch_scope_request(const char *json) {
  cJSON *root = cJSON_Parse(json);

  if (root == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "scope_request: failure in deserialization."};
  }

  cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");
  cJSON *value = cJSON_GetObjectItemCaseSensitive(root, "value");

  if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
    cJSON_Delete(root);
    return (Result){.type = SIMPLE_ERR,
                    .value = "scope_request: failure in handling."};
  }

  Result result;

  if (strcmp(method->valuestring, "get_scope_db") == 0) {
    result = handle_get_scope_db(value);
  } else if (strcmp(method->valuestring, "create_scope_db") == 0) {
    result = handle_create_scope_db(value);
  } else if (strcmp(method->valuestring, "delete_scope_db") == 0) {
    result = handle_delete_scope_db(value);
  } else if (strcmp(method->valuestring, "update_scope_db") == 0) {
    result = handle_update_scope_db(value);
  } else {
    result = (Result){.type = EMPTY, .value = NULL};
  }

  cJSON_Delete(root);
  return result;
}

Result dispatch_scopefile_request(const char *json) {

  cJSON *root = cJSON_Parse(json);

  if (root == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "scopefile_request: failure in deserialization."};
  }

  cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");

  cJSON *value = cJSON_GetObjectItemCaseSensitive(root, "value");

  if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
    cJSON_Delete(root);
    return (Result){.type = SIMPLE_ERR,
                    .value = "scopefile_request: failure in handling."};
  }

  Result result;

  if (strcmp(method->valuestring, "get_scopefile_db") == 0) {
    result = handle_get_scopefile_db(value);
  } else if (strcmp(method->valuestring, "create_scopefile_db") == 0) {
    result = handle_create_scopefile_db(value);
  } else if (strcmp(method->valuestring, "delete_scopefile_db") == 0) {
    result = handle_delete_scopefile_db(value);
  } else if (strcmp(method->valuestring, "update_scopefile_db") == 0) {
    result = handle_update_scopefile_db(value);
  } else {
    result = (Result){.type = EMPTY, .value = NULL};
  }

  cJSON_Delete(root);
  return result;
}

Result dispatch_entity_request(const char *json) {

  cJSON *root = cJSON_Parse(json);

  if (root == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value =
                        "globalentity_request: failure in deserialization"};
  }

  cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");
  cJSON *value = cJSON_GetObjectItemCaseSensitive(root, "value");

  if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
    cJSON_Delete(root);
    return (Result){.type = SIMPLE_ERR,
                    .value = "globalentity_request: failure in handling"};
  }

  Result result;

  if (strcmp(method->valuestring, "get_globalentity_db") == 0) {
    result = handle_get_globalentity_db(value);
  }

  cJSON_Delete(root);
  return result;
}

Result dispatch_relation_request(const char *json) {

  cJSON *root = cJSON_Parse(json);

  if (root == NULL) {
    return (Result){.type = SIMPLE_ERR,
                    .value = "relation_request: failure in deserialization"};
  }

  cJSON *method = cJSON_GetObjectItemCaseSensitive(root, "method");
  cJSON *value = cJSON_GetObjectItemCaseSensitive(root, "value");

  if (!cJSON_IsString(method) || !cJSON_IsObject(value)) {
    cJSON_Delete(root);
    return (Result){.type = SIMPLE_ERR,
                    .value = "relation_request: failure in handling"};
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
    result = (Result){.type = EMPTY, .value = NULL};
  }

  cJSON_Delete(root);
  return result;
}

Result route_request(const char *json) {

  Result route_scope = dispatch_scope_request(json);
  if (route_scope.type != EMPTY) {
    return route_scope;
  }

  Result route_scopefile = dispatch_scopefile_request(json);
  if (route_scopefile.type != EMPTY) {
    return route_scopefile;
  }

  Result route_entity = dispatch_entity_request(json);
  if (route_entity.type != EMPTY) {
    return route_entity;
  }

  Result route_relation = dispatch_relation_request(json);
  if (route_relation.type != EMPTY) {
    return route_relation;
  }

  return (Result){.type = EMPTY, .value = NULL};
}
