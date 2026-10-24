/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef SCOPE_DB
#define SCOPE_DB
#include "../../include/errors.h"
#include "../schema.h"
#include <sqlite3.h>
#include <stdint.h>

Result create_scope_db(Scope *new_scope);
Result get_scope_db(int64_t id, int64_t parent_scope_id, char *name);
Result delete_scope_db(int64_t id);
Result update_scope_db(int64_t id, Scope s);

#endif
