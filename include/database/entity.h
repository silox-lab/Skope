/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef ENTITY
#define ENTITY
#include "../schema.h"
#include "../errors.h"

Result get_globalentity_db(char *type, int64_t local_id, int64_t id);
Result create_globalentity_db(GlobalEntity *globalentity);
Result delete_globalentity_db(char *type, int64_t local_id, int64_t id); 
Result update_globalentity_db(char *type, int64_t local_id, int64_t id, GlobalEntity globalentity);

#endif