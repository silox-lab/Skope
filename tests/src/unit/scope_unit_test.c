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

  Result results[] = { get_res, get_updated_res };

  for (int r = 0; r < 2; r++) {
    Scope **scopes = results[r].value;

    for (int i = 0; scopes[i] != NULL; i++) {
      free(scopes[i]->name);
      free(scopes[i]);
    }

    free(scopes);
  }
}

Test(scope_unit, scope_search_test) {

  sqlite3 *db = GET_W_DATABASE();

  Scope new_scope = {
    .name = "foo",
    .parent_scope_id = 0
  };

  Result create_res = create_scope_db(&new_scope);
  cr_expect(create_res.type == SUCCESS, "create_scope_db failed: %s\n", (char *)create_res.value);

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

  Scope new_scope2 = {
    .name = "bar",
    .parent_scope_id = 0
  };

  Result create_res2 = create_scope_db(&new_scope2);
  cr_expect(create_res2.type == SUCCESS, "create_scope_db failed: %s\n", (char *)create_res2.value);

  Result get_res2 = get_scope_db(0, 0, "bar");
  cr_expect(get_res2.type == SUCCESS, "get_scope_db failed: %s\n", (char *)get_res2.value);

  Scope *created_scope2 = ((Scope **)get_res2.value)[0];
  cr_expect(
    created_scope2->id == 2 &&
    strcmp(created_scope2->name, "bar") == 0 &&
    created_scope2->parent_scope_id == 0,

    "created_scope2 field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "parent_scope_id: %ld\n",
    created_scope2->id,
    created_scope2->name,
    created_scope2->parent_scope_id
  );

  Result search_res = search_scope_db("f");
  cr_assert(search_res.type == SUCCESS, "search_res: failed: %s\n", (char *)search_res.value);

  Scope *searched_scope = ((Scope **)search_res.value)[0];
  cr_expect(
    strcmp(searched_scope->name, "foo") == 0 &&
    searched_scope->parent_scope_id == 0,

    "searched_scope: field mismatch:\n"
    "name: %s\n"
    "parent_scope_id: %ld\n",
    searched_scope->name,
    searched_scope->parent_scope_id
  );

  Result results[] = { get_res, get_res2, search_res };

  for (int r = 0; r < 3; r++) {
    Scope **scopes = results[r].value;

    for (int i = 0; scopes[i] != NULL; i++) {
      free(scopes[i]->name);
      free(scopes[i]);
    }

    free(scopes);
  }
}
