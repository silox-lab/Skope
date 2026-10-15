/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef RELATION_HANDLER
#define RELATION_HANDLER
#include "../errors.h"
#include <cjson/cJSON.h>

Result handle_get_relation_db(cJSON *value);
Result handle_create_relation_db(cJSON *value);
Result handle_delete_relation_db(cJSON *value);
Result handle_update_relation_db(cJSON *value);

#endif