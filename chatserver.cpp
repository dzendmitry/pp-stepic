#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <map>

#include "general.h"

#define PORT 12345
#define LISTEN_BACKLOG 128
#define MAX_EVENTS 10000
#define BUFSZ 1024

#define DISCONNECT \
	if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL)) \
		handle_error("epoll_ctl(EPOLL_CTL_DEL, events[i].data.fd)"); \
	shutdown(events[i].data.fd, SHUT_RDWR); \
	close(events[i].data.fd); \
	fd_map.erase(events[i].data.fd); \

int main(int argc, char *argv[]) {

	std::map<int, struct sockaddr_in> fd_map;
	char buffer[BUFSZ];

	int master_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (master_socket == -1) {
		handle_error("socket(master_socket)");
	}

	set_nonblock(master_socket);
	set_reusable(master_socket);

	struct sockaddr_in sa;
	socklen_t addrlen;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(master_socket, (struct sockaddr *) &sa, sizeof(struct sockaddr_in))) {
		handle_error("bind(master_socket)");
	}

	if (-1 == listen(master_socket, LISTEN_BACKLOG)) {
		handle_error("listen(master_socket)");
	}

	int epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		handle_error("epoll_create1(epoll_fd)");
	}

	struct epoll_event event, events[MAX_EVENTS];
	event.data.fd = master_socket;
	event.events = EPOLLIN;
	if(-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, master_socket, &event)) {
		handle_error("epoll_ctl(EPOLL_CTL_ADD, master_socket)");
	}

	for (;;) {
		int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		if (nfds == -1) {
			handle_error("epoll_wait()");
		}

		for (int i = 0; i < nfds; ++i) {
			if (events[i].data.fd == master_socket) {
				int conn_sock = accept(master_socket, (struct sockaddr *) &sa, &addrlen);
				if (conn_sock == -1) {
					perror("accept(conn_sock)");
					continue;
				}
				set_nonblock(conn_sock);
				event.events = EPOLLIN;
				event.data.fd = conn_sock;
				if (-1 == epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_sock, &event)) {
					handle_error("epoll_ctl(EPOLL_CTL_ADD, conn_sock)");
				}
				fd_map.insert(std::pair<int, struct sockaddr_in>(conn_sock, sa));
			} else if (events[i].events & EPOLLERR || events[i].events & EPOLLHUP) {
				DISCONNECT
			} else {
				ssize_t rbytes = recv(events[i].data.fd, buffer, BUFSZ * sizeof(char), MSG_NOSIGNAL);
				if (rbytes == -1) {
					handle_error("recv()");
				}
				if (!rbytes) {
					DISCONNECT
					continue;
				}
				std::string msg = sockaddr_to_ip_port(fd_map[events[i].data.fd]) + " : " + std::string(buffer);
				for ( auto it = fd_map.begin(); it != fd_map.end(); ++it ) {
					ssize_t wbytes = send(it->first, msg.c_str(), msg.length(), MSG_NOSIGNAL);
					if (wbytes == -1) {
						handle_error("send()");
					}
				}
			}
		}
	}

	return 0;
}