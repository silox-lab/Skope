/*
 * SPDX-FileCopyrightText: 2026 silox-lab
 *
 * SPDX-License-Identifier: MIT
 */
#include "../../include/api/dispatcher.h"
#include "../../include/api/serializer.h"
#include "../../include/errors.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

Result setup_socket() {

  char *get_dir = getenv("XDG_RUNTIME_DIR");
  char runtime_dir[sizeof(((struct sockaddr_un *)0)->sun_path)];

  int path_len;

  if (get_dir == NULL) {
    path_len = snprintf(runtime_dir, sizeof(runtime_dir), "/tmp/skope.sock");
  } else {
    path_len =
        snprintf(runtime_dir, sizeof(runtime_dir), "%s/skope.sock", get_dir);
  }

  if (path_len < 0 || (size_t)path_len >= sizeof(runtime_dir)) {
    return (Result){.type = SIMPLE_ERR, .value = "Socket path too long"};
  }

  if (unlink(runtime_dir) == -1 && errno != ENOENT) {
    return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
  }

  int sfd = socket(AF_UNIX, SOCK_STREAM, 0);

  if (sfd == -1) {
    return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
  }

  struct sockaddr_un addr = {0};

  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, runtime_dir);

  if (bind(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
    close(sfd);
    return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
  }

  if (listen(sfd, 5) == -1) {
    close(sfd);
    unlink(runtime_dir);
    return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
  }

  char *buf = malloc(2 * 1024 * 1024);

  if (buf == NULL) {
    close(sfd);
    unlink(runtime_dir);
    return (Result){.type = SIMPLE_ERR, .value = "Failed to allocate buffer"};
  }

  for (;;) {

    int cfd = accept(sfd, NULL, NULL);

    if (cfd == -1) {
      if (errno == EINTR) {
        continue;
      }

      free(buf);
      close(sfd);
      unlink(runtime_dir);
      return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
    }

    size_t total_read = 0;
    int request_complete = 0;

    while (total_read < (2 * 1024 * 1024) - 1) {
      ssize_t numread =
          read(cfd, buf + total_read, (2 * 1024 * 1024) - 1 - total_read);

      if (numread == -1) {
        if (errno == EINTR) {
          continue;
        }
        break;
      }

      if (numread == 0) {
        break;
      }

      size_t i;
      for (i = 0; i < (size_t)numread; i++) {
        if (buf[total_read + i] == '\n') {
          total_read += i;
          request_complete = 1;
          break;
        }
      }

      if (request_complete) {
        break;
      }

      total_read += (size_t)numread;
    }

    buf[total_read] = '\0';

    if (request_complete) {
      Result router = route_request(buf);

      if (router.type == EMPTY) {
        Result response_result = {
            .type = EMPTY,
            .value = "requested method did not match any endpoint"};
        char *response_json = nodata_result_to_json(response_result);

        if (response_json != NULL) {
          size_t len = strlen(response_json);
          response_json[len] = '\0';

          size_t sent = 0;
          while (sent < len) {
            ssize_t n = write(cfd, response_json + sent, len - sent);
            if (n == -1) {
              if (errno == EINTR) {
                continue;
              }
              break;
            }
            if (n == 0) {
              break;
            }
            sent += (size_t)n;
          }

          if (sent == len) {
            (void)write(cfd, "\n", 1);
          }

          free(response_json);
        }
      } else if (router.value != NULL) {
        size_t len = strlen(router.value);
        size_t sent = 0;

        while (sent < len) {
          ssize_t n = write(cfd, router.value + sent, len - sent);
          if (n == -1) {
            if (errno == EINTR) {
              continue;
            }
            break;
          }
          if (n == 0) {
            break;
          }
          sent += (size_t)n;
        }

        if (sent == len) {
          (void)write(cfd, "\n", 1);
        }
      }
    }

    if (close(cfd) == -1) {
      free(buf);
      close(sfd);
      unlink(runtime_dir);
      return (Result){.type = SIMPLE_ERR, .value = strerror(errno)};
    }
  }

  free(buf);
  close(sfd);
  unlink(runtime_dir);

  return (Result){.type = SUCCESS, .value = ""};
}
