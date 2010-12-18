
#include "io.h"


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

