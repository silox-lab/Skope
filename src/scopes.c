/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../include/database/scope_db.h"
#include "../include/errors.h"
#include "../include/globals.h"
#include "../include/slugify.h"
#include "../include/utils.h"
#include <dirent.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

Result build_scope_path(Scope s)
{
    int dirs_length = 0;
    int dirs_capacity = 8;

    char **dirs = malloc(sizeof(*dirs) * dirs_capacity);
    char *storage_path = GET_STORAGE_PATH();
    char *parent_name = NULL;
    char *final_path = NULL;

    Result result = { .type = SUCCESS, .value = NULL };

    if (dirs == NULL || storage_path == NULL) {
        free(dirs);
        free(storage_path);

        return (Result){
            .type = SIMPLE_ERR,
            .value = "build_scope_path: memory allocation failed"
        };
    }

    Scope current_scope = s;

    while (1) {
        char *slug = malloc(256);

        if (slug == NULL) {
            result = (Result){
                .type = SIMPLE_ERR,
                .value = "build_scope_path: memory allocation failed"
            };
            break;
        }

        slugify_default(current_scope.name, slug, 256);

        if (dirs_length == dirs_capacity) {
            int new_capacity = dirs_capacity * 2;

            char **new_dirs =
                realloc(dirs, sizeof(*dirs) * new_capacity);

            if (new_dirs == NULL) {
                free(slug);

                result = (Result){
                    .type = SIMPLE_ERR,
                    .value = "build_scope_path: memory allocation failed"
                };
                break;
            }

            dirs = new_dirs;
            dirs_capacity = new_capacity;
        }

        dirs[dirs_length++] = slug;

        if (current_scope.parent_scope_id == 0) {
            break;
        }

        Result parent_result =
            get_scope_db(current_scope.parent_scope_id, 0, NULL);

        if (parent_result.type != SUCCESS) {
            result = parent_result;
            break;
        }

        Scope **parent_arr = parent_result.value;

        if (parent_arr == NULL || parent_arr[0] == NULL) {
            result = (Result){
                .type = SIMPLE_ERR,
                .value = "build_scope_path: parent scope not found"
            };
            break;
        }

        char *new_parent_name = strdup(parent_arr[0]->name);
        int64_t new_parent_id = parent_arr[0]->parent_scope_id;

        for (int i = 0; parent_arr[i] != NULL; i++) {
            free(parent_arr[i]->name);
            free(parent_arr[i]);
        }

        free(parent_arr);

        if (new_parent_name == NULL) {
            result = (Result){
                .type = SIMPLE_ERR,
                .value = "build_scope_path: memory allocation failed"
            };
            break;
        }

        free(parent_name);
        parent_name = new_parent_name;

        current_scope.name = parent_name;
        current_scope.parent_scope_id = new_parent_id;
    }

    if (result.type == SUCCESS) {
        size_t path_capacity = strlen(storage_path) + 1;

        for (int i = 0; i < dirs_length; i++) {
            path_capacity += strlen(dirs[i]) + 1;
        }

        final_path = malloc(path_capacity);

        if (final_path == NULL) {
            result = (Result){
                .type = SIMPLE_ERR,
                .value = "build_scope_path: memory allocation failed"
            };
        } else {
            size_t position =
                (size_t)snprintf(final_path, path_capacity, "%s",
                                 storage_path);

            for (int i = dirs_length - 1; i >= 0; i--) {
                position += (size_t)snprintf(
                    final_path + position,
                    path_capacity - position,
                    "/%s",
                    dirs[i]
                );
            }

            result.value = final_path;
        }
    }

    free(parent_name);
    free(storage_path);

    for (int i = 0; i < dirs_length; i++) {
        free(dirs[i]);
    }

    free(dirs);

    return result;
}

Result get_scope(int64_t id) {
  char scope_fullpath[256];

  Result scope = get_scope_db(id, 0, NULL);
  if (scope.type == EMPTY) {
    return (Result){.type = FILE_DIR_NOTFOUND, .value = NULL};
  }

  if (scope.type != SUCCESS) {
    return (Result){.type = scope.type, .value = scope.value};
  }

  Result s_path = build_scope_path(*((Scope **)scope.value)[0]);

  DIR *scope_dir = opendir((char *)s_path.value);

  if (scope_dir == NULL) {
    
    free((char *)s_path.value);
    for (int i=0; ((Scope **)scope.value)[i] != NULL; i++) {
      free(((Scope **)scope.value)[i]->name);
      free(((Scope **)scope.value)[i]);
    }
    
    free(((Scope **)scope.value));
    
    return (Result){.type = FILE_DIR_NOTFOUND, .value = NULL};
  }
  free((char *)s_path.value);

  for (int i=0; ((Scope **)scope.value)[i] != NULL; i++) {
    free(((Scope **)scope.value)[i]->name);
    free(((Scope **)scope.value)[i]);
  }
    
  free(((Scope **)scope.value));
    
  return (Result){.type = SUCCESS, .value = ""};
}

Result make_scope(Scope s) {

  Result scope_exist = get_scope(s.id);
  if (scope_exist.type == FILE_DIR_NOTFOUND) {

    Result build_path = build_scope_path(s);
    if (build_path.type != SUCCESS) {
      return (Result){.type = build_path.type, .value = build_path.value};
    }

    int mkdir_e = mkdir(build_path.value, S_IRWXU);
    if (mkdir_e == -1) {
      return (Result){.type = SIMPLE_ERR,
                      .value = "make_scope: Error when creating Scope."};
    }
    free(build_path.value);
    return (Result){.type = SUCCESS, .value = ""};
  } else {
    return (Result){.type = SIMPLE_ERR,
                    .value = "make_scope: Scope already exist."};
  }
}

Result delete_scope(Scope s) {

  Result scope_exist = get_scope(s.id);

  if (scope_exist.type == FILE_DIR_NOTFOUND) {
    return (Result){.type = SIMPLE_ERR, .value = "Scope not found."};
  }

  Result build_path = build_scope_path(s);
  if (build_path.type != SUCCESS) {
    return (Result){.type = build_path.type, .value = build_path.value};
  }

  int del_dir = remove_directory_recursive(build_path.value);
  if (del_dir != 0) {
    free(build_path.value);
    return (Result){.type = SIMPLE_ERR, .value = "delete failed."};
  }
  free(build_path.value);
  return (Result){.type = SUCCESS, .value = ""};
}

Result update_scope(int64_t scope_id, char *new_name, int64_t new_parent_id) {

  Result scope_exist_db = get_scope_db(scope_id, 0, NULL);
  if (scope_exist_db.type != SUCCESS) {
    return (Result){.type = scope_exist_db.type, .value = scope_exist_db.value};
  }

  Scope **s_arr = scope_exist_db.value;
  if (scope_exist_db.type == EMPTY) {
    return (Result){.type = SIMPLE_ERR, .value = "scope not found"};
  }

  Result path = build_scope_path(*s_arr[0]);
  if (path.type != SUCCESS) {
    return (Result){.type = path.type, .value = path.value};
  }

  Result scope_exist = get_scope(scope_id);
  free(path.value);
  if (scope_exist.type != SUCCESS) {
    return (Result){.type = scope_exist.type, .value = scope_exist.value};
  }

  Scope new_s = {.id = s_arr[0]->id,
                 .parent_scope_id = new_parent_id == 0
                                        ? s_arr[0]->parent_scope_id
                                        : new_parent_id,
                 .created_on = s_arr[0]->created_on,
                 .name = new_name == NULL ? s_arr[0]->name : new_name};

  Result new_path = build_scope_path(new_s);
  if (new_path.type != SUCCESS) {
    free(path.value);
    return (Result){.type = new_path.type, .value = new_path.value};
  }

  Result validate_rename = get_scope(scope_id);
  if (validate_rename.type == SUCCESS) {
    free(path.value);
    free(new_path.value);
    return (Result){
        .type = SIMPLE_ERR,
        .value = "there are already scopes with the name you want to chose."};
  }

  int rename_s = rename(path.value, new_path.value);
  if (rename_s != 0) {
    free(path.value);
    free(new_path.value);
    return (Result){.type = SIMPLE_ERR, .value = "error in renaming scope."};
  }

  free(s_arr[0]->name);
  free(s_arr[0]);
  free(s_arr);

  free(path.value);
  free(new_path.value);

  return (Result){.type = SUCCESS, .value = ""};
}
