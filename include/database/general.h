/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef GENERAL
#define GENERAL
#include <sqlite3.h>
#include "../../include/errors.h"

Result open_database();
Result run_sql_db(char *table);

#endif