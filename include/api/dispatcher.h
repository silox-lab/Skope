/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
*/
#ifndef DISPATCHER
#define DISPATCHER
#include "../errors.h"

Result dispatch_wiki_request(const char *json);
Result dispatch_scope_request(const char *json);
Result dispatch_scopefile_request(const char *json);
Result dispatch_relation_request(const char *json);
Result route_request(const char *json);

#endif
