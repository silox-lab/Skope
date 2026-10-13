#include "../include/errors.h"
#include "../include/utils.h"
#include "../include/schema.h"
#include <stdlib.h>
#include <cjson/cJSON.h>

char *nodata_result_to_json(Result r) {
    
    char *status = result_enum_to_str(r);
    char *message;

    if (r.type == EMPTY) message = NULL;
    message = r.value;

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "type", status);
    if (message == NULL) cJSON_AddNullToObject(root, "value");
    cJSON_AddStringToObject(root, "value", message);

    return cJSON_PrintUnformatted(root);
}

char *wiki_arr_to_json(Result wiki_arr_result) {

    char *status = result_enum_to_str(wiki_arr_result);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "status", status);
    cJSON *value = cJSON_CreateObject();
    cJSON *wikis_arr = cJSON_AddArrayToObject(value, "wikis");

    for (int i = 0; ((Wiki **)(wiki_arr_result.value))[i] != NULL; i++) {

        Wiki *wiki = ((Wiki **)(wiki_arr_result.value))[i];

        cJSON *wiki_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(wiki_obj, "id", wiki->id);
        cJSON_AddNumberToObject(wiki_obj, "created_on", wiki->created_on);
        cJSON_AddStringToObject(wiki_obj, "name", wiki->name);
        cJSON_AddStringToObject(wiki_obj, "description", wiki->description);

        cJSON_AddItemToArray(wikis_arr, wiki_obj);

        free(wiki->name);
        free(wiki->description);
        free(wiki);
    }

    free(wiki_arr_result.value);

    cJSON_AddItemToObject(root, "value", value);

    return cJSON_PrintUnformatted(root);
}

char *scope_arr_to_json(Result scope_arr_result) {

    char *status = result_enum_to_str(scope_arr_result);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "status", status);
    cJSON *value = cJSON_CreateObject();
    cJSON *wikis_arr = cJSON_AddArrayToObject(value, "scopes");

    for (int i = 0; ((Scope **)(scope_arr_result.value))[i] != NULL; i++) {

        Scope *scope = ((Scope **)(scope_arr_result.value))[i];

        cJSON *scope_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(scope_obj, "id", scope->id);
        cJSON_AddNumberToObject(scope_obj, "created_on", scope->created_on);
        cJSON_AddStringToObject(scope_obj, "name", scope->name);
        cJSON_AddNumberToObject(scope_obj, "wiki_id", scope->wiki_id);
        cJSON_AddNumberToObject(scope_obj, "parent_scope_id", scope->parent_scope_id);

        cJSON_AddItemToArray(wikis_arr, scope_obj);

        free(scope->name);
        free(scope);
    }

    free(scope_arr_result.value);

    cJSON_AddItemToObject(root, "value", value);

    return cJSON_PrintUnformatted(root);
}

char *scopefile_arr_to_json(Result scopefile_arr_result) {

    char *status = result_enum_to_str(scopefile_arr_result);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "status", status);
    cJSON *value = cJSON_CreateObject();
    cJSON *scopefile_arr = cJSON_AddArrayToObject(value, "scopefiles");

    for (int i = 0; ((ScopeFile **)(scopefile_arr_result.value))[i] != NULL; i++) {

        ScopeFile *scopefile = ((ScopeFile **)(scopefile_arr_result.value))[i];

        cJSON *scopefile_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(scopefile_obj, "id", scopefile->id);
        cJSON_AddNumberToObject(scopefile_obj, "created_on", scopefile->created_on);
        cJSON_AddStringToObject(scopefile_obj, "name", scopefile->name);
        cJSON_AddNumberToObject(scopefile_obj, "scope_id", scopefile->scope_id);
        cJSON_AddStringToObject(scopefile_obj, "extension", scopefile->name);

        cJSON_AddItemToArray(scopefile_arr, scopefile_obj);

        free(scopefile->name);
        free(scopefile->extension);
    }

    free(scopefile_arr_result.value);

    cJSON_AddItemToObject(root, "value", value);

    return cJSON_PrintUnformatted(root);
}

char *globalentity_arr_to_json(Result globalentity_arr_result) {

    char *status = result_enum_to_str(globalentity_arr_result);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "status", status);
    cJSON *value = cJSON_CreateObject();
    cJSON *globalentity_arr = cJSON_AddArrayToObject(value, "wikis");

    for (int i = 0; ((Wiki **)(globalentity_arr_result.value))[i] != NULL; i++) {

        GlobalEntity *globalentity = ((GlobalEntity **)(globalentity_arr_result.value))[i];

        cJSON *globalentity_obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(globalentity_obj, "id", globalentity->id);
        cJSON_AddNumberToObject(globalentity_obj, "created_on", globalentity->created_on);
        cJSON_AddStringToObject(globalentity_obj, "type", globalentity->type);

        cJSON_AddItemToArray(globalentity_arr, globalentity_obj);

        free(globalentity->type);
        free(globalentity);
    }

    free(globalentity_arr_result.value);

    cJSON_AddItemToObject(root, "value", value);

    return cJSON_PrintUnformatted(root);
}