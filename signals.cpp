#include <signal.h>                                                             
#include <unistd.h>                                                             
#include <sys/types.h>                                                          
#include <stdio.h>                                                              
#include <string.h>

void savepid(const char *path) {
	pid_t pid = getpid();
	char pid_s[10];
	sprintf(pid_s, "%d", pid);
	FILE *f = fopen(path, "w");
	fwrite(pid_s, sizeof(char) * strlen(pid_s), 1, f);
	fclose(f);
}

/* int main(int argc, char *argv[]) {                                              
	savepid("/home/dzendmitry/pid");
	signal(SIGTERM, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	pause();
	return 0;
} */

int main(int argc, char *argv[]) {

	savepid("/home/dzendmitry/pid_parent");

	pid_t pid = fork();
	if (pid) { // parent
		waitpid(pid, NULL, 0);
	} else { // child
		savepid("/home/dzendmitry/pid_child");
		pause();
	}

}