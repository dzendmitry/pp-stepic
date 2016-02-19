#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int set_nonblock(int fd)
{
	int flags;
#if defined(O_NONBLOCK)
	if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
		flags = 0;
	}
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	flags = 1;
	return ioctl(fd, FIOBIO, &flags);
#endif
}

void set_reusable(int fd) {
	int optval = 1;
	if (-1 == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval))) {
		handle_error("set_reusable()");
	}
}

std::string sockaddr_to_ip_port(struct sockaddr_in sa) {
	return std::string(inet_ntoa(sa.sin_addr)) + ":" + std::to_string(ntohs(sa.sin_port));	
}

std::string socket_to_ip_port(int sockfd) {
    struct sockaddr_in sa;
    socklen_t len = sizeof(struct sockaddr_in);
    getsockname(sockfd, (struct sockaddr *)&sa, &len);
    return sockaddr_to_ip_port(sa);
}