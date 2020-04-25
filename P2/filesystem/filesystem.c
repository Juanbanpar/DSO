
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	filesystem.c
 * @brief 	Implementation of the core file system funcionalities and auxiliary functions.
 * @date	Last revision 01/04/2020
 *
 */


#include "filesystem.h" // Headers for the core functionality
#include "auxiliary.h"  // Headers for auxiliary functions
#include "metadata.h"   // Type and structure declaration of the file system
#include <string.h>

Superbloque1 SB1, SB2;

/*
 * @brief 	Generates the proper file system structure in a storage device, as designed by the student.
 * @return 	0 if success, -1 otherwise.
 */
int mkFS(long deviceSize)
{

	if(deviceSize < 471040 || deviceSize > 614400){
		return -1;
	}

    //Inicializar los valores iniciales
    SB1.diskSize = deviceSize;
    for(int i=0; i<MAX_FILES; i++) SB1.mapaINodos[i] = "0"; //Seguro que hay una forma mejor
    SB1.mapaBloques[(deviceSize/BLOCK_SIZE)-2]; //Los bloques de datos son el número de bloques (tamaño de disco entre tamaño de bloque) menos los dos del superbloque
    for (int i = 0; i < 4; i++) SB1.mapaBloques[i] = "0";
    SB1.numMagico = 100383438;
    
    for(int i = 0; i < MAX_FILES/2; i++) {
        strcpy(SB1.inodos[i].nombre, "");
        strcpy(SB2.inodos[i].nombre, "");
        for(int j = 0; j < 5; j++){
            SB1.inodos[i].bloque[j] = 0;
            SB2.inodos[i].bloque[j] = 0;
        }
        SB1.inodos[i].size = 0;
        SB2.inodos[i].size = 0;
        //SB1.inodos[i].crc = NULL;
        //SB2.inodos[i].crc = NULL;
    }
    
    if (unmountFS() == -1){
        printf("Error unmountFS\n");
        return -1;
    }
    
    return 0;
}

/*
 * @brief 	Mounts a file system in the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int mountFS(void)
{
    if (bread(DEVICE_IMAGE, 0, ((char *)&(SB1))) == -1 || bread(DEVICE_IMAGE, 1, ((char *)&(SB2))) == -1){
        printf("Error bread\n");
        return -1;
    }

    return 0;
}

/*
 * @brief 	Unmounts the file system from the simulated device.
 * @return 	0 if success, -1 otherwise.
 */
int unmountFS(void)
{
    if (bwrite(DEVICE_IMAGE, 0, ((char *)&(SB1))) == -1 || bwrite(DEVICE_IMAGE, 1, ((char *)&(SB2))) == -1){
        printf("Error bwrite\n");
        return -1;
    }

    return 0;
}

/*
 * @brief	Creates a new file, provided it it doesn't exist in the file system.
 * @return	0 if success, -1 if the file already exists, -2 in case of error.
 */
int createFile(char *fileName)
{

    //Comprobar si ya existe -> namei?

    int block;
    for(int i = 0; i < MAX_FILES; i++) {
        if(bitmap_getbit(SB1.mapaINodos, i)==0){
            bitmap_setbit(SB1.mapaINodos, i, 1);
            for(int i = 0 ; i < sizeof(SB1.mapaBloques) ; i++){
                if(bitmap_getbit(SB1.mapaBloques, i)==0){
                    bitmap_setbit(SB1.mapaBloques, i, 1);
                    block = i + 2;  //Los dos bloques de SB
                    break;
                }
            }
            if(i<MAX_FILES/2){
                strcpy(SB1.inodos[i].nombre, fileName);
                SB1.inodos[i].bloque[0]=block;  //Sabemos que es el primer bloque porque estamos creando el fichero
                return 0;
            }else{
                strcpy(SB2.inodos[i].nombre, fileName);
                SB2.inodos[i].bloque[0]=block;
                return 0;
            }
        }
    }
	return -2;
}

/*
 * @brief	Deletes a file, provided it exists in the file system.
 * @return	0 if success, -1 if the file does not exist, -2 in case of error..
 */
int removeFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{
	return -2;
}

/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{
	return -1;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{
	return -1;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
	return -1;
}

/*
 * @brief	Checks the integrity of the file.
 * @return	0 if success, -1 if the file is corrupted, -2 in case of error.
 */

int checkFile (char * fileName)
{
    return -2;
}

/*
 * @brief	Include integrity on a file.
 * @return	0 if success, -1 if the file does not exists, -2 in case of error.
 */

int includeIntegrity (char * fileName)
{
    return -2;
}

/*
 * @brief	Opens an existing file and checks its integrity
 * @return	The file descriptor if possible, -1 if file does not exist, -2 if the file is corrupted, -3 in case of error
 */
int openFileIntegrity(char *fileName)
{

    return -2;
}

/*
 * @brief	Closes a file and updates its integrity.
 * @return	0 if success, -1 otherwise.
 */
int closeFileIntegrity(int fileDescriptor)
{
    return -1;
}

/*
 * @brief	Creates a symbolic link to an existing file in the file system.
 * @return	0 if success, -1 if file does not exist, -2 in case of error.
 */
int createLn(char *fileName, char *linkName)
{
    return -1;
}

/*
 * @brief 	Deletes an existing symbolic link
 * @return 	0 if the file is correct, -1 if the symbolic link does not exist, -2 in case of error.
 */
int removeLn(char *linkName)
{
    return -2;
}
