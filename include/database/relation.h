#ifndef RELATION
#define RELATION
#include "../errors.h"
#include "../schema.h"

Result get_relation_db(int64_t source_id, int64_t target_id, char *type,
                       int64_t id);
Result create_relation_db(Relation *relation);
Result delete_relation_db(int64_t source_id, int64_t target_id, char *type,
                          int64_t id);
Result update_relation_db(int64_t source_id, int64_t target_id, char *type,
                          int64_t id, Relation relation);
                          
#endif