
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	test.c
 * @brief 	Implementation of the client test routines.
 * @date	01/03/2017
 *
 */


#include <stdio.h>
#include <string.h>
#include "filesystem/filesystem.h"


// Color definitions for asserts
#define ANSI_COLOR_RESET "\x1b[0m"
#define ANSI_COLOR_RED "\x1b[31m"
#define ANSI_COLOR_GREEN "\x1b[32m"
#define ANSI_COLOR_BLUE "\x1b[34m"

#define N_BLOCKS 250					  // Number of blocks in the device
#define DEV_SIZE N_BLOCKS *BLOCK_SIZE // Device size, in bytes

int main()
{
	int ret;
	
	///////
	ret = mkFS(DEV_SIZE);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = mountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = createFile("/test.txt");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	ret = unmountFS();
	ret = mountFS();
	
	int patata = openFile("/test.txt");
	//int namelo=namei("/test.txt");
	//printf("%d", namelo);
	printf("Descriptor del fichero %d\n", patata);
	char buffer1[2048]="patata";
	char buffer[2048];
	int escrito= writeFile(patata, buffer1, 10);
	printf("Numero de bytes escritos %d \n", escrito);
	int nepe=closeFile(patata);
	printf("Valor del close file: %d\n", nepe);
	patata = openFile("/test.txt");
	int leeido = readFile(patata, buffer, 6);
	printf("Cadena leida: %s\n", buffer);
	printf("Numero de bytes leidos %d\n", leeido);
	nepe=closeFile(patata);
	printf("Valor del close file: %d\n", nepe);
    
    //nepe=removeFile("/test.txt");
	//printf("Valor del remove file: %d\n", nepe);
	
    //patata = openFile("/test.txt");
    //printf("Valor del open file: %d\n", patata);
    
    nepe=includeIntegrity("/test.txt");
    printf("Valor del Integrity file: %d\n", nepe);
    nepe=checkFile("/test.txt");
    printf("Valor del checkFile file: %d\n", nepe);
    nepe=closeFileIntegrity(patata);
	printf("Valor del close file: %d\n", nepe);
    patata=openFileIntegrity("/test.txt");
    printf("Valor del openFileIntegrity file: %d\n", patata);
	///////

	ret = unmountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	return 0;
}
