/*
 ============================================================================
 Name        : prueba.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <commons/memory.h>
#include <commons/temporal.h>

void handler(int signum) {
	char* time = temporal_get_string_time("%H:%M:%S:%MS");
	printf("\n\n================================================================================\n"
			"Signal received at: %s\n\n"
			, time);
	free(time);
}

int main(void) {
	printf("\n\n================================================================================\n"
			"My PID: %d\n\n"
			, getpid());

	int fd = open("blocks.txt", O_RDWR | O_CREAT, (mode_t) 0777);
	if(fd == -1){
		perror("error abriendo blocks.txt");
		exit(-1);
	}
	fallocate(fd, 0, 0, 25);

	char* mapeado = mmap(NULL, 25, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	memset(mapeado, 'A', 25);
	mem_hexdump(mapeado, 25);

	signal(SIGUSR1, handler);

	while(1) {
		sleep(1000000000);

		printf("\nBefore msync():\n\n");
		mem_hexdump(mapeado, 25);
		msync(mapeado, 25, 0);
		printf("\nAfter msync():\n\n");
		mem_hexdump(mapeado, 25);

		memset(mapeado, 'A', 25);
		msync(mapeado, 25, 0);
		printf("\nFile restarted\n\n");
	}


	return EXIT_SUCCESS;
}
