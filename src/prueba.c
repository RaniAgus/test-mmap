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
#include <stdbool.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include <commons/memory.h>
#include <commons/temporal.h>
#include <commons/string.h>

#define FILE_SIZE 16
#define FILE_PATH "blocks.txt"
#define ASCII_START 65
#define ASCII_COUNT 26

int fd;
char* mapeado;
char buffer[FILE_SIZE + 1];
char* time;

void sigusr1_handler(int signum);
bool file_exists();
void create_file();
void map_file();
char* cat_file();
void dump_map_and_cat(char* format);

int main(void) {
	if(!file_exists()) {
		create_file();
	}

	map_file();
	int8_t next = ((int8_t)(*mapeado)) - ASCII_START + 1;

	signal(SIGUSR1, sigusr1_handler);
	printf("\n\n================================================================================\n"
			"My PID: %d\n"
			"Next: %c (%x)\n\n"
			, getpid()
			, ASCII_START + next
			, ASCII_START + next);

	dump_map_and_cat("at start");

	while(1) {
		sleep(1000000000);

		dump_map_and_cat("before msync()");

		msync(mapeado, FILE_SIZE, 0);

		dump_map_and_cat("after msync(), before memset()");

		memset(mapeado, ASCII_START + (next++) % ASCII_COUNT, FILE_SIZE);

		dump_map_and_cat("after memset(), before msync()");

		msync(mapeado, FILE_SIZE, 0);

		dump_map_and_cat("after msync()");
	}


	return EXIT_SUCCESS;
}

bool file_exists() {
    FILE *file;
    if((file = fopen(FILE_PATH, "r"))){
        fclose(file);
        return true;
    }
    return false;
}

void sigusr1_handler(int signum) {
	time = temporal_get_string_time("%H:%M:%S:%MS");
	printf("\n\n================================================================================\n"
			"Signal received at: %s\n\n"
			, time);
	free(time);
}


void create_file() {
	int filedes = open(FILE_PATH, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG);
	if(filedes == -1){
		perror("error al intentar abrir " FILE_PATH);
		exit(-1);
	}
	fallocate(filedes, 0, 0, FILE_SIZE);
	char* character = string_repeat(ASCII_START, FILE_SIZE);
	write(filedes, character, FILE_SIZE);
	free(character);
	close(filedes);
}

void map_file() {
	fd = open(FILE_PATH, O_RDWR, S_IRWXU | S_IRWXG);
	if (fd == -1) {
		perror("error al intentar abrir blocks.txt");
		exit(-1);
	}
	mapeado = mmap(NULL, FILE_SIZE, PROT_WRITE | PROT_READ, MAP_SHARED, fd, 0);
}

void dump_map_and_cat(char* format) {
	printf("Map %s:\n", format);
	mem_hexdump(mapeado, FILE_SIZE);
	printf("\nFile %s:\n", format);
	mem_hexdump(cat_file(), FILE_SIZE);
	printf("--------------------------------------------------------------------------------\n");
}

char* cat_file() {
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

