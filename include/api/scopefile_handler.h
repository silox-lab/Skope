/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef SCOPEFILE_HANDLER
#define SCOPEFILE_HANDLER
#include "../errors.h"
#include <cjson/cJSON.h>

Result handle_get_scopefile_db(cJSON *value);
Result handle_create_scopefile_db(cJSON *value);
Result handle_delete_scopefile_db(cJSON *value);
Result handle_update_scopefile_db(cJSON *value);

#endif
