/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef HANDLER
#define HANDLER
#include "../errors.h"
#include <cjson/cJSON.h>

Result handle_get_wiki_db(cJSON *value);
Result handle_create_wiki_db(cJSON *value);
Result handle_get_all_wikis(cJSON *value);
Result handle_delete_wiki_db(cJSON *value);
Result handle_update_wiki_db(cJSON *value);

#endif