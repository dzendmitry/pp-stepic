#include <iostream>
#include <vector>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "split.h"

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

void cmd_params(const std::string& cmd, std::string& bin, std::string& params) {
	std::size_t found = cmd.find_first_of(' ');
	if (found == std::string::npos) {
		bin = cmd;
		return;
	}
	bin = cmd.substr(0, found);
	params = cmd.substr(found + 1, cmd.size() - found);
}

void exec_cmd(const std::string& token) {
	std::string cmd, params;
	cmd_params(token, cmd, params);
	if (params.empty()) {
		if (execlp(cmd.c_str(), params.c_str(), (char*)NULL) == -1) {
			handle_error("shell execlp");
		}
	} else {
		if (execlp(cmd.c_str(), cmd.c_str(), params.c_str(), (char*)NULL) == -1) {
			handle_error("shell execlp");
		}
	}
}

int main(int argc, char* argv[]) {
	std::vector<std::string> tokens;
	std::string cmd;
	std::getline(std::cin, cmd);
	split(tokens, cmd, '|', s_split::no_empties);

	close(STDOUT_FILENO);
	int fd = open("/home/dzendmitry/result.out", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU);
	if (fd == -1) {
		handle_error("open()");
	}

	unsigned int tokens_count = tokens.size();
	if (!tokens_count) {
		return 0;
	} else if (tokens_count == 1) {
		exec_cmd(tokens[0]);
	} else {
		int forks = tokens_count - 1;
		for (;;) {
			int pipefd[2];
			if (pipe(pipefd) == -1) {
				handle_error("pipe()");
			}
			if(dup2(pipefd[0], STDIN_FILENO) == -1) {
				handle_error("dup2() parent");
			}
			close(pipefd[0]);
			size_t pid = fork();
			if (pid > 0) { // parent
				close(pipefd[1]);
				if (forks == tokens_count - 1) {
					exec_cmd(tokens[forks]);
				} else {
					exec_cmd(tokens[forks]);
				}
			} else if (!pid) { // children
				if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
					handle_error("dup2() children");
				}
				close(pipefd[1]);
				--forks;
				if (!forks) {
					exec_cmd(tokens[forks]);
				}
			} else {
				handle_error("fork()");
			}
		}
	}

	return 0;
}