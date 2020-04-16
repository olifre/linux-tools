#include <sys/mman.h>
#include <errno.h>
#include <sys/resource.h>
#include <stdio.h>
#include <signal.h>
#include <cstdlib>
#include <limits.h>
#include <unistd.h>
#include <malloc.h>

void sighandler(int);

int exitRequest=0;

int main(int argc, char** argv) {
	signal(SIGINT, sighandler);
	unsigned long bytes=1024;
	if (argc>1) {
		bytes=strtoul(argv[1], NULL, 0);
		if (bytes==ULONG_MAX) {
			printf("Value out of range: %s.\r\n", argv[1]);
			exit(1);
		} else if (bytes==0) {
			printf("Cannot convert to number: %s.\r\n", argv[1]);
			exit(1);
		}
	}

	printf("Allocating %lu bytes.\r\n", bytes);
	fflush(stdout);
	char* foo=new char[bytes];
	long i;
	for (i=0; i<bytes; i++) {
		foo[i]=7;
	}

	printf("Running mlockall...\r\n");
	fflush(stdout);
	struct rlimit limits;
	getrlimit(RLIMIT_MEMLOCK, &limits);
	limits.rlim_cur = limits.rlim_max;
	setrlimit(RLIMIT_MEMLOCK, &limits);

	if (mlockall(MCL_CURRENT) < 0) {
		perror("mlockall failed to lock memory pages!");
	} else {
		printf("mlockall completed successfully.\r\n");
	}
	fflush(stdout);

	printf("Sleeping until SIGINT...");
	fflush(stdout);
	while (exitRequest==0) {
		sleep(1);
	}
	printf(" freeing...");
	fflush(stdout);
	delete [] foo;
	printf(" bye!\r\n");
	fflush(stdout);

	return 0;
}

void sighandler(int signum) {
	exitRequest=1;
}
