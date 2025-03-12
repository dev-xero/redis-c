#include "../include/utils.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void msg(const char *msg) { fprintf(stderr, "%s\n", msg); }

void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}
