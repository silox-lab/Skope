/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef ENTITY_HANDLER
#define ENTITY_HANDLER
#include <cjson/cJSON.h>
#include "../errors.h"

Result handle_get_globalentity_db(cJSON *value);

#endif