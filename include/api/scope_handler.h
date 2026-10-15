/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef SCOPE_HANDLER
#define SCOPE_HANDLER
#include "../errors.h"
#include <cjson/cJSON.h>

Result handle_get_scope_db(cJSON *value);
Result handle_create_scope_db(cJSON *value);
Result handle_delete_scope_db(cJSON *value);
Result handle_update_scope_db(cJSON *value);

#endif
