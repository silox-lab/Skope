/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/errors.h"
#include "../../include/api/dispatcher.h"
#include "../../include/api/serializer.h"
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>

Result setup_socket() {

  char *get_dir = getenv("XDG_RUNTIME_DIR");
  char runtime_dir[256];

  if (get_dir == NULL) {
    strcpy(runtime_dir, "/tmp/wikit.sock");
  } else {
    strcpy(runtime_dir, get_dir);
    strcat(runtime_dir, "/wikit.sock");
  }

  unlink(runtime_dir);
  
  int sfd;
  sfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sfd == -1) {
    return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
  }

  struct sockaddr_un addr;

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, runtime_dir);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
    return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
  }

  int64_t numread;
  char *buf = malloc(2 * 1024 * 1024);
  int cfd;
  
  if (listen(sfd, 5) == -1) {
      return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
  }
  
  for (;;) {

    memset(buf, 0, 2 * 1024 * 1024); 

    cfd = accept(sfd, NULL, NULL);

    if (cfd == -1) {
      return (Result) {
        .type = SIMPLE_ERR,
        .value = strerror(errno)
      };
    }

    numread = read(cfd, buf, 2 * 1024 * 1024);
    strcat(buf, "\0");
    
    if (numread == -1) {
      if (close(cfd) == -1) {
	return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
      }
      return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
    }
    
    Result router = route_request(buf);
    if (router.type == EMPTY) {
      Result response_result = {.type = EMPTY,
                         .value =
                             "requsted method did not matched any endpoint"};
      char *response_json = nodata_result_to_json(response_result);

      if (write(cfd, response_json, strlen(response_json + 1)) == -1) {
	return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
      }

      if (close(cfd) == -1) {
	return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
      }
    }
    
    strcat(router.value, "\n");
 
    if (write(cfd, router.value, (strlen(router.value) + 1)) == -1) {
      return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
    }
  }

  if (close(cfd) == -1) {
    return (Result) { .type = SIMPLE_ERR, .value = strerror(errno) };
  }
  return (Result) { .type = SUCCESS, .value = "" };
}
