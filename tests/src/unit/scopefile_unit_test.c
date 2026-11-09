#include <criterion/criterion.h>
#include <criterion/internal/assert.h>
#include "../../include/sqlite_mock.h"
#include "../../../include/schema.h"
#include "../../../include/utils.h"
#include "../../../include/globals.h"
#include "../../../include/database/scope_db.h"
#include "../../../include/database/scopefile_db.h"

static void setup(void) {
    cr_expect(sqlite_mock_db().type == SUCCESS, "SQLITE failed to run.");
}

static void teardown() {
    char *path = GET_STORAGE_PATH();
    int remove_storage = remove_directory_recursive(path);
    cr_assert(remove_storage == 0, "remove_storage: failed");
}

TestSuite(scopefile_unit, .init = setup, .fini = teardown);

Test(scopefile_unit, scopefile_crud_test) {

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

  // READ

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

  
  Result get_main_sf_res = get_scopefile_db("main", created_scope->id);
  cr_assert(get_sf_res.type == SUCCESS, "get_main_sf_res: failed to get scopefile: %s \n", (char *)get_main_sf_res.value);

  ScopeFile *created_main_scopefile = ((ScopeFile **)get_main_sf_res.value)[0];

  cr_assert(
    created_main_scopefile->id == 1 &&
    strcmp(created_main_scopefile->name, "main") == 0 &&
    strcmp(created_main_scopefile->extension, "txt") == 0 &&
    created_main_scopefile->scope_id == created_scope->id,

    "created_main_scopefile: field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "extension: %s\n"
    "scope_id: %ld\n",
    created_main_scopefile->id,
    created_main_scopefile->name,
    created_main_scopefile->extension,
    created_main_scopefile->scope_id
  );

  // UPDATE

  ScopeFile update_sf_payload = {
    .name = "bar",
    .extension = "txt"
  };
  
  Result update_sf_res = update_scopefile_db("foo", created_scope->id, update_sf_payload);
  cr_assert(update_sf_res.type == SUCCESS, "update_sf_res: failed: %s\n", (char *)update_sf_res.value);

  Result get_updated_sf_res = get_scopefile_db("bar", created_scope->id);  
  cr_assert(get_updated_sf_res.type == SUCCESS, "get_updated_sf_res: failed to get scopefile: %s \n", (char *)get_updated_sf_res.value);

  ScopeFile *updated_scopefile = ((ScopeFile **)get_updated_sf_res.value)[0];

  cr_assert(
    updated_scopefile->id == 2 &&
    strcmp(updated_scopefile->name, "bar") == 0 &&
    strcmp(updated_scopefile->extension, "txt") == 0 &&
    updated_scopefile->scope_id == created_scope->id,

    "updated_scopefile: field mismatch:\n"
    "id: %ld\n"
    "name: %s\n"
    "extension: %s\n"
    "scope_id: %ld\n",
    updated_scopefile->id,
    updated_scopefile->name,
    updated_scopefile->extension,
    updated_scopefile->scope_id
  );

  // DELETE

  Result delete_scopefile = delete_scopefile_db(updated_scopefile->id);
  cr_assert(delete_scopefile.type == SUCCESS, "delete_scopefile: %s\n", (char *)delete_scopefile.value);


  for (int i=0; ((Scope **)get_s_res.value)[i] != NULL; i++) {
      free(((Scope **)get_s_res.value)[i]->name);
      free(((Scope **)get_s_res.value)[i]);
  }
  free((Scope **)get_s_res.value);

  Result results[] = { get_sf_res, get_main_sf_res, get_updated_sf_res };

  for (int r = 0; r < 3; r++) {
    ScopeFile **scopefiles = results[r].value;

    for (int i = 0; scopefiles[i] != NULL; i++) {
      free(scopefiles[i]->name);
      free(scopefiles[i]->extension);
      free(scopefiles[i]);
    }

    free(scopefiles);
  }

}

Test(scopefile_unit, scopefile_search_test) {

  Scope new_scope = {
    .name = "test_scope",
    .parent_scope_id = 0
  };

  Result scope_create_res = create_scope_db(&new_scope);
  cr_assert_eq(
    scope_create_res.type,
    SUCCESS,
    "create_scope_db: failed: %s",
    (char *)scope_create_res.value
  );

  Result scope_get_res = get_scope_db(0, 0, "test_scope");
  cr_assert_eq(
    scope_get_res.type,
    SUCCESS,
    "get_scope_db failed: %s",
    (char *)scope_get_res.value
  );

  Scope *created_scope = ((Scope **)scope_get_res.value)[0];
  int64_t scope_id = created_scope->id;

  ScopeFile new_scopefile = {
    .name = "foo",
    .extension = "txt",
    .scope_id = scope_id
  };

  Result create_res = create_scopefile_db(&new_scopefile);
  cr_assert_eq(
    create_res.type,
    SUCCESS,
    "create_scopefile_db failed: %s",
    (char *)create_res.value
  );

  Result get_res = get_scopefile_db("foo", scope_id);
  cr_assert_eq(
    get_res.type,
    SUCCESS,
    "get_scopefile_db failed: %s",
    (char *)get_res.value
  );

  ScopeFile *created_scopefile = ((ScopeFile **)get_res.value)[0];

  cr_expect(
    strcmp(created_scopefile->name, "foo") == 0 &&
    strcmp(created_scopefile->extension, "txt") == 0 &&
    created_scopefile->scope_id == scope_id
  );

  ScopeFile new_scopefile2 = {
    .name = "bar",
    .extension = "pdf",
    .scope_id = scope_id
  };

  Result create_res2 = create_scopefile_db(&new_scopefile2);
  cr_assert_eq(
    create_res2.type,
    SUCCESS,
    "create_scopefile_db failed: %s",
    (char *)create_res2.value
  );

  Result get_res2 = get_scopefile_db("bar", scope_id);
  cr_assert_eq(
    get_res2.type,
    SUCCESS,
    "get_scopefile_db failed: %s",
    (char *)get_res2.value
  );

  ScopeFile *created_scopefile2 = ((ScopeFile **)get_res2.value)[0];

  cr_expect(
    strcmp(created_scopefile2->name, "bar") == 0 &&
    strcmp(created_scopefile2->extension, "pdf") == 0 &&
    created_scopefile2->scope_id == scope_id
  );

  Result search_res = search_scopefile_db("foo");

  cr_assert_eq(
    search_res.type,
    SUCCESS,
    "search_scopefile_db failed: %s",
    (char *)search_res.value
  );

  ScopeFile *searched_scopefile = ((ScopeFile **)search_res.value)[0];

  cr_expect(
    strcmp(searched_scopefile->name, "foo") == 0 &&
    strcmp(searched_scopefile->extension, "txt") == 0 &&
    searched_scopefile->scope_id == scope_id
  );

  Result results[] = { get_res, get_res2, search_res };

  for (int r = 0; r < 3; r++) {
    ScopeFile **scopefiles = results[r].value;

    for (int i = 0; scopefiles[i] != NULL; i++) {
      free(scopefiles[i]->name);
      free(scopefiles[i]->extension);
      free(scopefiles[i]);
    }

    free(scopefiles);
  }

  Scope **scopes = scope_get_res.value;

  for (int i = 0; scopes[i] != NULL; i++) {
    free(scopes[i]->name);
    free(scopes[i]);
  }

  free(scopes);
}
