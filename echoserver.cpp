#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define PORT 12345
#define LISTEN_BACKLOG 128
#define BUFFER_SIZE 256

int main(int argc, char *argv[]) {
	int sd = socket(AF_INET, SOCK_STREAM, 0);
	if (sd == -1) {
		handle_error("");
	}

	int optval = 1;
	if (-1 == setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval, sizeof(optval))) {
		handle_error("");
	}


	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	if (-1 == bind(sd, (struct sockaddr *) &sa, sizeof(struct sockaddr_in))) {
		handle_error("");
	}

	if (-1 == listen(sd, LISTEN_BACKLOG)) {
		handle_error("");
	}

	char buffer[BUFFER_SIZE];
	struct sockaddr_in sa_in;
	socklen_t addrlen;
	while (int _sd = accept(sd, (struct sockaddr *) &sa_in, &addrlen)) {
		if (_sd == -1) {
			handle_error("");
		}

		printf("client %s:%d start\n", inet_ntoa(sa_in.sin_addr), ntohs(sa_in.sin_port));

		do {
			ssize_t bytes = recv(_sd, buffer, BUFFER_SIZE * sizeof(char), 0);
			if (!bytes)
				break;
			if (bytes < 0)
				handle_error("");
			if (bytes > 0)
				buffer[bytes-2] = '\0';
			printf("%s\n", buffer);
			if (-1 == send(_sd, buffer, bytes, 0))
				handle_error("");
		} while (strcmp(buffer, "quit"));

		printf("client %s:%d finish\n", inet_ntoa(sa_in.sin_addr), ntohl(sa_in.sin_port));
	}

	return 0;
}