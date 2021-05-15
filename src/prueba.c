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

#define FILE_SIZE 25
#define FILE_PATH "blocks.txt"

char* mapeado;
char buffer[FILE_SIZE + 1];

void handler(int signum) {
	char* time = temporal_get_string_time("%H:%M:%S:%MS");
	printf("\n\n================================================================================\n"
			"Signal received at: %s\n\n"
			, time);
	free(time);
}

char* read_file() {
	FILE *pipe;

	if((pipe = popen("cat " FILE_PATH, "r")) == NULL) {
		perror("error al intentar abrir el archivo");
	}

	for(int i = 0; i < FILE_SIZE; i++) {
		buffer[i] = fgetc(pipe);
	}
	buffer[FILE_SIZE] = '\0';

	pclose(pipe);
	return buffer;
}

void print_map_and_file(char* str) {
	printf("Map %s:\n", str);
	mem_hexdump(mapeado, FILE_SIZE);
	printf("\nFile %s:\n", str);
	mem_hexdump(read_file(), FILE_SIZE);
	printf("--------------------------------------------------------------------------------\n");
}

int main(void) {
	printf("\n\n================================================================================\n"
			"My PID: %d\n\n"
			, getpid());

	int fd = open(FILE_PATH, O_RDWR | O_CREAT, (mode_t) 0777);
	if(fd == -1){
		perror("error al intentar abrir el archivo");
		exit(-1);
	}
	fallocate(fd, 0, 0, FILE_SIZE);

	mapeado = mmap(NULL, 25, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
	memset(mapeado, 'A' + rand() % 25, FILE_SIZE);
	msync(mapeado, FILE_SIZE, 0);

	print_map_and_file("at start");

	signal(SIGUSR1, handler);

	while(1) {
		sleep(1000000000);

		print_map_and_file("before msync()");
	
		msync(mapeado, FILE_SIZE, 0);

		print_map_and_file("after msync(), before memset()");

		memset(mapeado, 'A' + rand() % 25, FILE_SIZE);

		print_map_and_file("after memset(), before msync()");

		msync(mapeado, FILE_SIZE, 0);

		print_map_and_file("after msync()");
	}


	return EXIT_SUCCESS;
}
