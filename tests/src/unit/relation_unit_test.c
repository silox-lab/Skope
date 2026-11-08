#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include "../../include/sqlite_mock.h"
#include "../../../include/utils.h"
#include "../../../include/schema.h"
#include "../../../include/database/scope_db.h"
#include "../../../include/database/scopefile_db.h"
#include "../../../include/database/entity_db.h"
#include "../../../include/database/relation_db.h"
#include "../../../include/globals.h"

static void setup() {
    cr_assert(sqlite_mock_db().type == SUCCESS, "SQLITE failed to run");
}

static void teardown() {
    char *path = GET_STORAGE_PATH();
    int remove_storage = remove_directory_recursive(path);
    cr_assert(remove_storage == 0, "remove_storage: failed");
}

TestSuite(relation_unit, .init = setup, .fini = teardown);

Test(relation_unit, relation_crud_test) {

  // CREATE
  Scope new_scope = {
      .name = "foo",
      .parent_scope_id = 0
  };
  
  Result create_s_res = create_scope_db(&new_scope);
  cr_assert(create_s_res.type == SUCCESS, "create_scope_db failed: %s\n", (char *)create_s_res.value);

  Result get_s_res = get_scope_db(0, 0, "foo");
  cr_assert(get_s_res.type == SUCCESS, "get_scope_db failed: %s\n", (char *)get_s_res.value);

  Scope *created_scope = ((Scope **)get_s_res.value)[0];
  cr_assert(
    created_scope->id == 1 &&
    strcmp(created_scope->name, "foo") == 0 &&
    created_scope->parent_scope_id == 0,

    "created_scope field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "parent_scope_id: %ld\n",
    created_scope->id,
    created_scope->name,
    created_scope->parent_scope_id
  );

  ScopeFile new_scopefile = {
    .name = "foo",
    .extension = "md",
    .scope_id = created_scope->id
  };

  Result create_sf_res = create_scopefile_db(&new_scopefile);
  cr_assert(create_sf_res.type == SUCCESS, "create_sf_res: failed to create scopefile: %s \n", (char *)create_sf_res.value);

  Result get_sf_res = get_scopefile_db("foo", created_scope->id);
  cr_assert(get_sf_res.type == SUCCESS, "get_sf_res: failed to get scopefile: %s \n", (char *)get_sf_res.value);

  ScopeFile *created_scopefile = ((ScopeFile **)get_sf_res.value)[0];

  cr_assert(
    created_scopefile->id == 2 &&
    strcmp(created_scopefile->name, "foo") == 0 &&
    strcmp(created_scopefile->extension, "md") == 0 &&
    created_scopefile->scope_id == created_scope->id,

    "created_scopefile: field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "extension: %s\n"
    "scope_id: %ld\n",
    created_scopefile->id,
    created_scopefile->name,
    created_scopefile->extension,
    created_scopefile->scope_id
  );

  Result get_s_entity_res = get_globalentity_db("Scope", created_scope->id, 0);
  cr_assert(get_s_entity_res.type == SUCCESS, "get_s_entity_res: failed: %s\n", (char *)get_s_entity_res.value);

  GlobalEntity *created_s_entity = ((GlobalEntity **)get_s_entity_res.value)[0];
  cr_assert(
    strcmp(created_s_entity->type, "Scope") == 0 &&
    created_s_entity->local_id == created_scope->id,

    "created_s_entity: field mismatch:\n"
    "type: %s\n"
    "local_id: %ld\n",
    created_s_entity->type,
    created_s_entity->local_id
  );

  Result get_sf_entity_res = get_globalentity_db("ScopeFile", created_scopefile->id, 0);
  cr_assert(get_sf_entity_res.type == SUCCESS, "get_sf_entity_res: failed: %s\n", (char *)get_sf_entity_res.value);

  GlobalEntity *created_sf_entity = ((GlobalEntity **)get_sf_entity_res.value)[0];
  cr_assert(
    strcmp(created_sf_entity->type, "ScopeFile") == 0 &&
    created_sf_entity->local_id == created_scopefile->id,

    "created_sf_entity: field mismatch:\n"
    "type: %s\n"
    "local_id: %ld\n",
    created_sf_entity->type,
    created_sf_entity->local_id
  );

  Relation new_relation = {
      .type = "Depends",
      .source_id = created_sf_entity->id,
      .target_id = created_s_entity->id
  };

  Result create_relation_res = create_relation_db(&new_relation);
  cr_assert(create_relation_res.type == SUCCESS, "create_relation_db: failed: %s\n", (char *)create_relation_res.value);

  // READ
  
  Result get_created_rel_res = get_relation_db(0, 0, NULL, 1);
  cr_assert(get_created_rel_res.type == SUCCESS, "get_created_rel_res: failed: %s\n", (char *)get_created_rel_res.value);

  Relation *created_relation = ((Relation **)get_created_rel_res.value)[0];
  cr_assert(
    created_relation->id == 1 &&
    strcmp(created_relation->type, "Depends") == 0 &&
    created_relation->source_id == created_sf_entity->id &&
    created_relation->target_id == created_s_entity->id,

    "created_relation: field mismatch:\n"
    "id: %ld\n"
    "type: %s\n"
    "source_id: %ld\n"
    "target_id: %ld\n",
    created_relation->id,
    created_relation->type,
    created_relation->source_id,
    created_relation->target_id
  );


  // UPDATE
  
  Relation update_rel_payload = {
      .type = "Explains",
      .source_id = created_relation->target_id,
      .target_id = created_relation->source_id
  };

  Result update_rel_res = update_relation_db(0, 0, NULL, 1, update_rel_payload);
  cr_assert(update_rel_res.type == SUCCESS, "update_rel_res: failed: %s\n", (char *)update_rel_res.value);

  Result get_updated_rel_res = get_relation_db(0, 0, NULL, 1);
  cr_assert(get_updated_rel_res.type == SUCCESS, "get_updated_rel_res: failed: %s\n", (char *)get_updated_rel_res.value);

  Relation *updated_relation = ((Relation **)get_updated_rel_res.value)[0];
  cr_assert(
    updated_relation->id == 1 &&
    strcmp(updated_relation->type, "Explains") == 0 &&
    updated_relation->source_id == created_s_entity->id &&
    updated_relation->target_id == created_sf_entity->id,

    "updated_relation: field mismatch:\n"
    "id: %ld\n"
    "type: %s\n"
    "source_id: %ld\n"
    "target_id: %ld\n",
    updated_relation->id,
    updated_relation->type,
    updated_relation->source_id,
    updated_relation->target_id
  );

  // DELETE
  
  Result delete_rel_res = delete_relation_db(0, 0, NULL, 1);

  cr_assert(delete_rel_res.type == SUCCESS, "delete_rel_res: failed: %s\n", (char *)delete_rel_res.value);
}
