
#ifndef __IO_H
#define __IO_H

#include <unistd.h>
#include <sys/select.h>

ssize_t read_timeout(int fd, void * buf, size_t count, struct timeval * timeout);
ssize_t read_timeout_usec(int fd, void * buf, size_t count, long usec_timeout);

int file_exists(const char * path);

#endif
