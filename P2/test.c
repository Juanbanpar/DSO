
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

#define N_BLOCKS 48 * 5		  // Number of blocks in the device
#define DEV_SIZE N_BLOCKS * BLOCK_SIZE // Device size, in bytes

int main()
{
	int ret, fd;
	// char *bufferIn;
	char bufferOut[MAX_FILE_SIZE];
	
    ///////ID3

	ret = mountFS();
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID3: mountFS before mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID3 mountFS before mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID5

	ret = unmountFS();
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID5 unmountFS before mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID5 unmountFS before mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    /////ID8

	ret = createFile("/test.txt");
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID8 createFile before mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID8 createFile before mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
	///////ID1
	ret = mkFS(DEV_SIZE);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID1 mkFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID1 mkFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////ID2

	ret = mountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID2 mountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID2 mountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
	/////ID6

	ret = createFile("/test.txt");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID6 createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID6 createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

    /////ID7

	ret = createFile("/test.txt");
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID7 createFile duplicated ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID7 createFile duplicated ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
	///////ID9

	fd = openFile("/test.txt");
	if (fd == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID9 openFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID9 openFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

    ///////ID10

	int fdFallo = openFile("/test.txt");
	if (fdFallo != -2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID10 openFile already open ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID10 openFile already open ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID11

	fdFallo = openFile("/testInexistente.txt");
	if (fdFallo != -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID11 openFile inexistent ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID11 openFile inexistent ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
	///////

	char *bufferIn = "0-abcdefghijklmnñopqrstuvwxyz-1-abcdefghijklmnñopqrstuvwxyz-2-abcdefghijklmnñopqrstuvwxyz-3-abcdefghijklmnñopqrstuvwxyz-4-abcdefghijklmnñopqrstuvwxyz-5-abcdefghijklmnñopqrstuvwxyz-6-abcdefghijklmnñopqrstuvwxyz\0";

	ret = writeFile(fd, bufferIn, strlen(bufferIn));
	if (ret == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = createFile("/second.txt");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST createFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	int fd2 = openFile("/second.txt");
	if (fd == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST openFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	bufferIn = "222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222\0";

	ret = writeFile(fd2, bufferIn, strlen(bufferIn));
	if (ret == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	bufferIn = "10-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-11-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-12-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-13-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-14abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-15-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-16-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-17-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-18-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-19-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-20-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-21-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-22-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-23-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-24-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-25-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-26-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-27-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-28-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-29-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-30-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-31-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-32-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-33-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-34-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-35-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-36-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-37-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-38-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-39-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-40-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-41-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-42-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-43-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-44-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-45-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-46-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-47-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-48-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz-50-abcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyzabcdefghijklmnñopqrstuvwxyz\0";

	ret = writeFile(fd, bufferIn, strlen(bufferIn));
	if (ret == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST writeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	ret = readFile(fd, bufferOut, MAX_FILE_SIZE);
	if (ret == -1)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST readFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////ID24

	ret = closeFile(fd);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID24 closeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID 24 closeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////ID24

	ret = closeFile(fd2);
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID24 closeFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID24 closeFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID25

    int fd3 = 10;
	ret = closeFile(fd3);
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID25 closeFile not open ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID25 closeFile not open ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID26

    fd3 = -10;
	ret = closeFile(fd3);
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID26 closeFile negative ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID26 closeFile negative ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID27

    fd3 = 500;
	ret = closeFile(fd3);
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID27 closeFile too big ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID27 closeFile too big ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID28

	fd = openFileIntegrity("/test.txt");
	if (fd == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID28 openFileIntegrity no CRC ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID28 openFileIntegrity no CRC ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID29

    ret = createFile("/testIntegridad.txt");
    fd = openFile("/testIntegridad.txt");
    bufferIn = "hola";
    writeFile(fd, bufferIn, strlen(bufferIn));
	int integ = includeIntegrity("/testIntegridad.txt");
	if (integ != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID29 includeIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID29 includeIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    closeFile(fd);
    
    ///////ID30

	fd = openFileIntegrity("/testIntegridad.txt");
	if (fd != 2)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID30 openFileIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID30 openFileIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID31

	integ = checkFile("/testIntegridad.txt");
	if (integ != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID31 checkFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID31 checkFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID32

	integ = closeFileIntegrity(fd);
	if (integ != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID32 closeFileIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID32 closeFileIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID33

	fd = openFileIntegrity("/testIntegridad.txt");
    ret = closeFile(fd);
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID33 closeFile after openFileIntegrity ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID33 closeFile after openFileIntegrity ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    closeFileIntegrity(fd);
    
    ///////ID34

	fd = openFile("/test.txt");
    ret = closeFileIntegrity(fd);
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID34 closeFileIntegrity after openFile ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID34 closeFileIntegrity after openFile ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    closeFile(fd);
    
    ///////ID35

    ret = createLn("/test.txt", "/enlace");
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID35 createLn ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID35 createLn ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID36

    ret = createLn("/dso.txt", "/enlace");
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID36 createLn inexistent file ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID36 createLn inexistent file ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
    ///////ID37

    ret = createLn("/enlace", "/enlace2");
	if (ret == 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID37 createLn to link ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID37 createLn to link ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);
    
	///////ID4

	ret = unmountFS();
	if (ret != 0)
	{
		fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID4 unmountFS ", ANSI_COLOR_RED, "FAILED\n", ANSI_COLOR_RESET);
		return -1;
	}
	fprintf(stdout, "%s%s%s%s%s", ANSI_COLOR_BLUE, "TEST ID4 unmountFS ", ANSI_COLOR_GREEN, "SUCCESS\n", ANSI_COLOR_RESET);

	///////

	return 0;
}
