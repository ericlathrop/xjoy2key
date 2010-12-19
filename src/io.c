/*
Copyright 2010 Eric Lathrop

This file is part of xjoy2key.

xjoy2key is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

xjoy2key is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with xjoy2key.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "io.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

ssize_t read_timeout(int fd, void * buf, size_t count, struct timeval * timeout)
{
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    int ret = select(fd + 1, &fds, NULL, NULL, timeout);
    if (ret == 1)
    {
        return read(fd, buf, count);
    }
    return ret;
}

ssize_t read_timeout_usec(int fd, void * buf, size_t count, long usec_timeout)
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = usec_timeout;
    return read_timeout(fd, buf, count, &timeout);
}

int file_exists(const char * path)
{
    struct stat buf;
    return stat(path, &buf) == 0;
}
