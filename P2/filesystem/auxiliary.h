
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	auxiliary.h
 * @brief 	Headers for the auxiliary functions required by filesystem.c.
 * @date	Last revision 01/04/2020
 *
 */

int bmap(int i, int pos);
int namei(char *file_name);
int bi(int block);
int customRead(int fileDescriptor, char *buffer, int size);
int customWrite(int fileDescriptor, char *buffer, int index, int size);