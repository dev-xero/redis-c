#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

void msg(const char *msg);
void die(const char *msg);
int32_t read_full(int fd, char *buf, size_t n);
int32_t write_all(int fd, const char *buf, size_t n);

#endif // UTILS_H
