#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include <criterion/internal/test.h>
#include <string.h>
#include "../../include/sqlite_mock.h"
#include "../../../include/globals.h"
#include "../../../include/utils.h"
#include "../../../include/schema.h"
#include "../../../include/database/scope_db.h"

static void setup(void) {
    cr_expect(sqlite_mock_db().type == SUCCESS, "SQLITE failed to run.");
}

static void teardown() {
    char *path = GET_STORAGE_PATH();
    int remove_storage = remove_directory_recursive(path);
    cr_assert(remove_storage == 0, "remove_storage: failed");
}

TestSuite(scope_unit, .init = setup, .fini = teardown);

Test(scope_unit, scope_crud_test) {

  sqlite3 *db = GET_W_DATABASE();

  Scope new_scope = {
    .name = "foo",
    .parent_scope_id = 0
  };

  // CREATE
  Result create_res = create_scope_db(&new_scope);
  cr_expect(create_res.type == SUCCESS, "create_scope_db failed: %s\n", (char *)create_res.value);

  // READ
  Result get_res = get_scope_db(0, 0, "foo");
  cr_expect(get_res.type == SUCCESS, "get_scope_db failed: %s\n", (char *)get_res.value);

  Scope *created_scope = ((Scope **)get_res.value)[0];
  cr_expect(
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

  // UPDATE
  Scope update_payload = {
    .name = "bar",
  };

  Result update_res = update_scope_db(created_scope->id, update_payload);
  cr_expect(update_res.type == SUCCESS, "update_scope_db failed: %s\n", (char *)update_res.value);

  Result get_updated_res = get_scope_db(0, 0, "bar");
  cr_expect(get_updated_res.type == SUCCESS, "get_scope_db (updated) failed: %s\n", (char *)get_updated_res.value);

  Scope *updated_scope = ((Scope **)get_updated_res.value)[0];
  cr_expect(
    updated_scope->id == 1 &&
    strcmp(updated_scope->name, "bar") == 0 &&
    updated_scope->parent_scope_id == 0,

    "updated_scope field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "parent_scope_id: %ld\n",
    updated_scope->id,
    updated_scope->name,
    updated_scope->parent_scope_id
  );

  // DELETE
  Result delete_res = delete_scope_db(created_scope->id);
  cr_expect(delete_res.type == SUCCESS, "delete_scope_db failed.");

  for (int i=0; ((Scope **)get_res.value)[i] != NULL; i++) {
      free(((Scope **)get_res.value)[i]->name);
      free(((Scope **)get_res.value)[i]);
  }
  free((Scope **)get_res.value);

  for (int i=0; ((Scope **)get_updated_res.value)[i] != NULL; i++) {
      free(((Scope **)get_updated_res.value)[i]->name);
      free(((Scope **)get_updated_res.value)[i]);
  }
  free((Scope **)get_updated_res.value);
}
