#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


// buf to read in the line, max number of chars
// return length of line //
int read_line(char *buf, int max) {
	int n;
	n = read(0, buf, max-1);
	if(n == 0) return n;
	buf[n] = 0;
	return n;
}

// is destroying buf
char *read_token(char *buf, char *prev_token, unsigned int len) 
{
	char *p;
	if(prev_token == NULL) { // first time here
		for(p = buf; p < buf+len; p++) {
			if(*p==' ' || *p=='\t'||*p=='\r'||*p=='\n') {
				*p = 0;
			}
		}
		for(p = buf; *p==0 && p < buf+len; p++)
				;// find first non 0 char at the begining; 
		prev_token = p;
		return prev_token;
	}
	int found = 0;
	for(p = prev_token;p < buf + len; p++) {
		if(*p == 0) {
			found = 1;
			continue;
		}
		if(found) {
			prev_token = p;
			return p;
		}
	}
	return NULL;
}

#define MAX_ARGC 10
int main() {
	printf("cOSh - cOSiris shell\n");
	char buf[256];
	char *argv[MAX_ARGC];
	int argc;
	char *token;
	int len;
	pid_t pid;
	while(1) {
		write(1, "# ", 2);
		len = read_line(buf, sizeof(buf));
		if(len <= 0) {	// discard empty line //
			continue;
		}

		token = NULL;
		for(argc = 0; argc < MAX_ARGC; argc++) {
			token = read_token(buf, token, len);
			if(!token) {
				break;
			}
			argv[argc] = token;
		}
		argv[argc] = 0;

		pid = fork();
		if(pid == 0) {
			exit(execvp(argv[0], argv));
		} else if(pid < 0) {
			printf("error forking\n");
		} else {
			int status;
			pid = wait(&status);
			if(status!=0) {
				printf("cosh: error executing %s\n", argv[0]);
			}
		}
	}
	return 0;
}
