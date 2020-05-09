
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "filesystem.h" // Headers for the core functionality
#include "auxiliary.h"  // Headers for auxiliary functions
#include "metadata.h"   // Type and structure declaration of the file system

Superbloque1 SB1;
Superbloque2 SB2;
INodoX Inodos[MAX_FILES];

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
    for(int i=0; i<MAX_FILES; i++) bitmap_setbit(SB1.mapaINodos, i, 0);
    for (int i=2; i < deviceSize/BLOCK_SIZE; i++) bitmap_setbit(SB1.mapaBloques, i, 0);
    SB1.numMagico = 100383438;
    
    for(int i = 0; i < MAX_FILES/2; i++) {
        memset(SB1.inodos[i].nombre, '\0', MAX_NAME_LENGHT+1);
        memset(SB2.inodos[i].nombre, '\0', MAX_NAME_LENGHT+1);
        for(int j = 0; j < 5; j++){
            SB1.inodos[i].bloque[j] = 0;
            SB2.inodos[i].bloque[j] = 0;
        }
        SB1.inodos[i].tipo = 0;
        SB2.inodos[i].tipo = 0;
        SB1.inodos[i].size = 0;
        SB2.inodos[i].size = 0;
        SB1.inodos[i].crc = 0;
        SB2.inodos[i].crc = 0;
        bitmap_setbit(SB1.mapaBloques, 0, 1);
        bitmap_setbit(SB1.mapaBloques, 1, 1);
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
    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

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

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (Inodos[i].estado == 1) {
            printf("Error unmountFS: file %d is still open", i);
            return -1;
        }
    }
    
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

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    if(strlen(fileName)>MAX_NAME_LENGHT) return -2;
    if(namei(fileName)!=-1){
        printf("File already exists\n");
        return -1;
    }else{
        int block;
        int i;

        //Buscamos un inodo libre
        for(i = 0; i < MAX_FILES; i++) {
            if(bitmap_getbit(SB1.mapaINodos, i)==0){
                bitmap_setbit(SB1.mapaINodos, i, 1);
                break;
            }
        }
        //Si no hay ninguno
        if(i==MAX_FILES){
            return -2;
        }

        //Buscamos un bloque libre
        for(int i = 2 ; i < SB1.diskSize/BLOCK_SIZE ; i++){
            if(bitmap_getbit(SB1.mapaBloques, i)==0){
                bitmap_setbit(SB1.mapaBloques, i, 1);
                block = i;  //Los dos bloques de SB
                break;
            }
        }

        //Asignacion en el inodo
        if(i<MAX_FILES/2){
            strcpy(SB1.inodos[i].nombre, fileName);
            SB1.inodos[i].bloque[0]=block;
            return 0;
        }else{
            strcpy(SB2.inodos[i].nombre, fileName);
            SB2.inodos[i].bloque[0]=block;
            return 0;
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

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    if(strlen(fileName)>MAX_NAME_LENGHT) return -2;

    int inode = namei(fileName);

    if(inode == -1){
        printf("File does not exist\n");
        return -1;
    }

    char bloque[BLOCK_SIZE];
    memset(bloque, '\0', BLOCK_SIZE);

    //Liberamos el inodo
    bitmap_setbit(SB1.mapaINodos, inode, 0);

    //Borramos el contenido del fichero y el inodo
    for (int j = 0; j < 5; j++)
    {
        if(inode<MAX_FILES/2){
            memset(SB1.inodos[inode].nombre, '\0', MAX_NAME_LENGHT+1);
            if(SB1.inodos[inode].bloque[j]!=0){
                if (bwrite(DEVICE_IMAGE, SB1.inodos[inode].bloque[j], bloque) == -1){
                    printf("Error bwrite\n");
                    return -2;
                }
                bitmap_setbit(SB1.mapaBloques, SB1.inodos[inode].bloque[j], 0);
                SB1.inodos[inode].bloque[j] = 0;
            }
            SB1.inodos[inode].size = 0;
            SB1.inodos[inode].crc = 0;
        }else{
            inode -= MAX_FILES/2;
            memset(SB2.inodos[inode].nombre, '\0', MAX_NAME_LENGHT+1);
            if(SB2.inodos[inode].bloque[j]!=0){
                if (bwrite(DEVICE_IMAGE, SB2.inodos[inode].bloque[j], bloque) == -1){
                    printf("Error bwrite\n");
                    return -2;
                }
                bitmap_setbit(SB1.mapaBloques, SB2.inodos[inode].bloque[j], 0);
                SB2.inodos[inode].bloque[j] = 0;
            }
            SB2.inodos[inode].size = 0;
            SB2.inodos[inode].crc = 0;
        }
    }
	return 0;
}

/*
 * @brief	Opens an existing file.
 * @return	The file descriptor if possible, -1 if file does not exist, -2 in case of error..
 */
int openFile(char *fileName)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    if(strlen(fileName) > MAX_NAME_LENGHT) return -2;

    int fd = namei(fileName);

    if(fd == -1){
        printf("File does not exist\n");
        return -1;
    }

    if(Inodos[fd].estado==1) return -2; //Ya esta abierto, cuidadito

    Inodos[fd].posPuntero=0;
    Inodos[fd].integridad=0;

    //Si es un blando hacemos la traduccion
    if(SB1.inodos[fd].tipo == 1 || SB2.inodos[fd-MAX_FILES/2].tipo == 1){
        Inodos[fd].estado=0;

        if(fd < MAX_FILES/2) fd = bi(SB1.inodos[fd].bloque[0]);
        else fd = bi(SB2.inodos[fd-MAX_FILES/2].bloque[0]);
        
        if(Inodos[fd].estado==1) return -2; //Ya esta abierto, cuidadito
    }

    //Lo volvemos a hacer por si fuera un blando
    Inodos[fd].estado=1;
    Inodos[fd].posPuntero=0;
    Inodos[fd].integridad=0;

    return fd;
}


/*
 * @brief	Closes a file.
 * @return	0 if success, -1 otherwise.
 */
int closeFile(int fileDescriptor)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

    if(fileDescriptor < 0 || fileDescriptor > MAX_FILES || Inodos[fileDescriptor].estado==0) return -1;
    
    if (Inodos[fileDescriptor].integridad == 1) {
        printf("NF11: You can't close a file opened using openFileIntegrity using closeFile");
        return -1;
    }

    Inodos[fileDescriptor].estado=0;
    Inodos[fileDescriptor].posPuntero=0;

	return 0;
}

/*
 * @brief	Reads a number of bytes from a file and stores them in a buffer.
 * @return	Number of bytes properly read, -1 in case of error.
 */
int readFile(int fileDescriptor, void *buffer, int numBytes)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

    //Comprobacion de parametros
    if(fileDescriptor < 0 || fileDescriptor > MAX_FILES-1 || numBytes < 0) return -1;

    //se comprueba que el fichero este abierto
    if(Inodos[fileDescriptor].estado == 1){
        
        char buff[MAX_FILE_SIZE];
        int b_id;

        //Numero de bloques a leer
        int bq = 0;
        int preBq = 0;
        int postBq = 0;
        int index = 0;

        if(fileDescriptor < MAX_FILES/2){

            //comprobamos que no se pase y le quede por leer
            if(Inodos[fileDescriptor].posPuntero + numBytes > SB1.inodos[fileDescriptor].size){

                numBytes = SB1.inodos[fileDescriptor].size - Inodos[fileDescriptor].posPuntero;
                if(numBytes <= 0) return 0;
            }

            //Si el puntero no está en el borde de un bloque hay que leer un poco antes de empezar a leer bloques enteros
            if(Inodos[fileDescriptor].posPuntero != 0 || Inodos[fileDescriptor].posPuntero % (BLOCK_SIZE-1) != 0){

                //Bloque en el que estamos
                int bqPointer = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero) - 2;
                preBq = BLOCK_SIZE*(bqPointer+1)-1 - Inodos[fileDescriptor].posPuntero; //Lo que queda hasta el final de este bloque

                if(preBq > numBytes) preBq=numBytes;
            }
            
            bq = (numBytes-preBq)/BLOCK_SIZE;
            if((numBytes-preBq) % BLOCK_SIZE > 0) postBq = numBytes - bq - preBq;

            if(preBq > 0){
                
                //Tenemos que hacer la lectura a mano
                customRead(fileDescriptor, &buff[index], preBq);

                Inodos[fileDescriptor].posPuntero += preBq;
                index += preBq;
            }
            
            if(bq > 0){

                //En que bloque estamos
                b_id = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);

                //Hallamos el indice de bloque del inodo en el que se encuentra el bloque actual
                for(int i=0; i<5;i++){
                    if(SB1.inodos[fileDescriptor].bloque[i] == b_id){
                        b_id = i;
                        break;
                    }
                }
                
                for(int i=b_id; i<bq;i++){

                    //Leemos el bloque
                    bread(DEVICE_IMAGE, SB1.inodos[fileDescriptor].bloque[i], &buff[index]);

                    //Actualizamos las varibales
                    Inodos[fileDescriptor].posPuntero += BLOCK_SIZE;
                    index += BLOCK_SIZE;
                }
            }

            if(postBq > 0){
                //Tenemos que hacer la lectura a mano
                customRead(fileDescriptor, &buff[index], postBq);
                    
                Inodos[fileDescriptor].posPuntero += postBq;
                index += postBq;
            }
            
            memmove(buffer, (void *) buff, numBytes);
            return numBytes;

        }else{

            //comprobamos que no se pase y le quede por leer
            if(Inodos[fileDescriptor].posPuntero+numBytes > SB2.inodos[fileDescriptor-MAX_FILES/2].size){
                numBytes = SB2.inodos[fileDescriptor-MAX_FILES/2].size - Inodos[fileDescriptor].posPuntero;
                if(numBytes <= 0) return 0;
            }

            //Si el puntero no está en el borde de un bloque hay que leer un poco antes de empezar a leer bloques enteros
            if(Inodos[fileDescriptor].posPuntero != 0 || Inodos[fileDescriptor].posPuntero % (BLOCK_SIZE-1) != 0){

                //Bloque en el que estamos
                int bqPointer = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);
                preBq = BLOCK_SIZE*(bqPointer+1)-1 - Inodos[fileDescriptor].posPuntero; //Lo que queda hasta el final de este bloque

                if(preBq > numBytes) preBq=numBytes;
            }
            
            bq = (numBytes-preBq)/BLOCK_SIZE;
            if((numBytes-preBq) % BLOCK_SIZE > 0) postBq = numBytes - bq - preBq;

            if(preBq > 0){
                
                //Tenemos que hacer la lectura a mano
                customRead(fileDescriptor, &buff[index], preBq);

                Inodos[fileDescriptor].posPuntero += preBq;
                index += preBq;
            }
            
            if(bq > 0){

                //En que bloque estamos
                b_id = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);

                //Hallamos el indice de bloque del inodo en el que se encuentra el bloque actual
                for(int i=0; i<5;i++){
                    if(SB2.inodos[fileDescriptor-MAX_FILES/2].bloque[i] == b_id){
                        b_id = i;
                        break;
                    }
                }
                
                for(int i=b_id; i<bq;i++){

                    //Leemos el bloque
                    bread(DEVICE_IMAGE, SB2.inodos[fileDescriptor-MAX_FILES/2].bloque[i], &buff[index]);

                    //Actualizamos las varibales
                    Inodos[fileDescriptor].posPuntero += BLOCK_SIZE;
                    index += BLOCK_SIZE;
                }
            }

            if(postBq > 0){
                //Tenemos que hacer la lectura a mano
                customRead(fileDescriptor, &buff[index], postBq);
                    
                Inodos[fileDescriptor].posPuntero += postBq;
                index += postBq;
            }
            
            memmove(buffer, (void *) buff, numBytes);
            return numBytes;
        } 
    }
	return -1;
}

/*
 * @brief	Writes a number of bytes from a buffer and into a file.
 * @return	Number of bytes properly written, -1 in case of error.
 */
int writeFile(int fileDescriptor, void *buffer, int numBytes)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

    //Comprobacion de parametros
    if(fileDescriptor < 0 || fileDescriptor > MAX_FILES-1 || numBytes < 0) return -1;

    //se comprueba que el fichero este abierto
    if(Inodos[fileDescriptor].estado == 1){
        
        char buff[MAX_FILE_SIZE];
        int b_id;

        //Numero de bloques a leer
        int bq = 0;
        int preBq = 0;
        int postBq = 0;
        int index = 0;

        //comprobamos que no se pase y le quede por escribir
        if(Inodos[fileDescriptor].posPuntero + numBytes > MAX_FILE_SIZE){
            return 0;
        }

        //Si el puntero no está en el borde de un bloque hay que leer un poco antes de empezar a leer bloques enteros
        if(Inodos[fileDescriptor].posPuntero != 0 || Inodos[fileDescriptor].posPuntero % (BLOCK_SIZE-1) != 0){

            //Bloque en el que estamos
            int bqPointer = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero) - 2;
            preBq = BLOCK_SIZE*(bqPointer+1)-1 - Inodos[fileDescriptor].posPuntero; //Lo que queda hasta el final de este bloque

            if(preBq > numBytes) preBq=numBytes;
        }
            
        bq = (numBytes-preBq)/BLOCK_SIZE;
        if((numBytes-preBq) % BLOCK_SIZE > 0) postBq = numBytes - bq * BLOCK_SIZE - preBq;

        if(preBq > 0){

            b_id = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);

            //leemos lo que hay en el resto del bloque, unimos con lo nuestro y escribimos el bloque
            bread(DEVICE_IMAGE, b_id, buff);
            memmove(buff + Inodos[fileDescriptor].posPuntero%BLOCK_SIZE, (void *) buffer + index, preBq);
            bwrite(DEVICE_IMAGE, b_id, buff);

            Inodos[fileDescriptor].posPuntero += preBq;
            SB1.inodos[fileDescriptor].size += preBq;
            index += preBq;
        }

        if(fileDescriptor < MAX_FILES/2){

            if(bq > 0){
                for(int i=0; i<bq;i++){

                    memmove(buff + index, (void *) buffer + index, BLOCK_SIZE);
                    bwrite(DEVICE_IMAGE, bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero), buff + index);

                    //Actualizamos las varibales
                    Inodos[fileDescriptor].posPuntero += BLOCK_SIZE;
                    SB1.inodos[fileDescriptor].size += BLOCK_SIZE;
                    index += BLOCK_SIZE;
                }
            }

            if(postBq > 0){

                b_id = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);

                //leemos lo que hay en el resto del bloque, unimos con lo nuestro y escribimos el bloque
                memmove(buff + index, (void *) buffer + index, postBq);
                bwrite(DEVICE_IMAGE, b_id, buff + index);
                    
                Inodos[fileDescriptor].posPuntero += postBq;
                SB1.inodos[fileDescriptor].size += postBq;
                index += postBq;
            }
            return numBytes;

        }else{
            if(bq > 0){
                for(int i=0; i<bq;i++){

                    memmove(buff + index, (void *) buffer + index, BLOCK_SIZE);
                    bwrite(DEVICE_IMAGE, bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero), buff + index);

                    //Actualizamos las varibales
                    Inodos[fileDescriptor].posPuntero += BLOCK_SIZE;
                    SB2.inodos[fileDescriptor-MAX_FILES].size += BLOCK_SIZE;
                    index += BLOCK_SIZE;
                }
            }

            if(postBq > 0){

                b_id = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero);

                //leemos lo que hay en el resto del bloque, unimos con lo nuestro y escribimos el bloque
                memmove(buff + index, (void *) buffer + index, postBq);
                bwrite(DEVICE_IMAGE, b_id, buff + index);
                    
                Inodos[fileDescriptor].posPuntero += postBq;
                SB2.inodos[fileDescriptor-MAX_FILES/2].size += postBq;
                index += postBq;
            }
            return numBytes;            
        } 
    }
	return -1;
}

/*
 * @brief	Modifies the position of the seek pointer of a file.
 * @return	0 if succes, -1 otherwise.
 */
int lseekFile(int fileDescriptor, long offset, int whence)
{
    //Comprobar que el fichero existe
    if(fileDescriptor < 0 || fileDescriptor > MAX_FILES-1){
        printf("File does not exists\n");
        return -1;
    }
    if(fileDescriptor < MAX_FILES/2 && strcmp(SB1.inodos[fileDescriptor].nombre,"")==0){
        printf("File does not exist\n");
        return -1;
    }else if(fileDescriptor > MAX_FILES/2 && strcmp(SB2.inodos[fileDescriptor-MAX_FILES/2].nombre,"")==0){
        printf("File does not exist\n");
        return -1;
    }

    //Comprobar que este abierto el fichero
    if(Inodos[fileDescriptor].estado == 0) return -1;

    //Ver si se quedaria fuera de los limites del fichero
    if (whence == FS_SEEK_CUR && (Inodos[fileDescriptor].posPuntero + offset > MAX_FILE_SIZE || Inodos[fileDescriptor].posPuntero + offset < 0)){
		printf("Pointer out of range\n");
		return -1;
	}
 
    //Actualizamos el puntero
    if(whence == FS_SEEK_CUR) Inodos[fileDescriptor].posPuntero += offset;
    else if(whence == FS_SEEK_BEGIN) Inodos[fileDescriptor].posPuntero = 0;
    else if(whence == FS_SEEK_END) Inodos[fileDescriptor].posPuntero = MAX_FILE_SIZE;

	return 0;
}

/*
 * @brief	Checks the integrity of the file.
 * @return	0 if success, -1 if the file is corrupted, -2 in case of error.
 */
int checkFile (char * fileName)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    int inodo = 0;
    inodo = namei(fileName);
    
    if(inodo == -1){
        printf("File does not exist\n");
        return -2;
    }

    if(Inodos[inodo].estado!=1){
        return -2;
    }
    int pos = Inodos[inodo].posPuntero;
    Inodos[inodo].posPuntero=0;
    
    if(inodo < MAX_FILES/2) {
        unsigned char buffer[SB1.inodos[inodo].size];
        readFile(inodo, buffer, SB1.inodos[inodo].size);
        uint32_t val = CRC32(buffer, SB1.inodos[inodo].size);
        
        if (SB1.inodos[inodo].crc == val) {
            Inodos[inodo].posPuntero=pos;
            return 0;
        } else {
            Inodos[inodo].posPuntero=pos;
            return -1;
        }
    } else {
        unsigned char buffer[SB2.inodos[inodo-MAX_FILES/2].size];
        readFile(inodo, buffer, SB2.inodos[inodo-MAX_FILES/2].size);
        uint32_t val = CRC32(buffer, SB2.inodos[inodo-MAX_FILES/2].size);
        
        if (SB2.inodos[inodo-MAX_FILES/2].crc == val) {
            Inodos[inodo].posPuntero=pos;
            return 0;
        } else {
            Inodos[inodo].posPuntero=pos;
            return -1;
        }
    }
    
    return -2;
}

/*
 * @brief	Include integrity on a file.
 * @return	0 if success, -1 if the file does not exists, -2 in case of error.
 */

int includeIntegrity (char * fileName)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    int inodo = 0;
    inodo = namei(fileName);
    
    if(inodo == -1){
        printf("File does not exist\n");
        return -1;
    }
    
    if(Inodos[inodo].estado!=1){
        return -2;
    }
    int pos = Inodos[inodo].posPuntero;
    Inodos[inodo].posPuntero=0;
    
    if(inodo < MAX_FILES/2) {
        unsigned char buffer[SB1.inodos[inodo].size];
        readFile(inodo, buffer, SB1.inodos[inodo].size);
        uint32_t val = CRC32(buffer, SB1.inodos[inodo].size);
        SB1.inodos[inodo].crc = val;
        
        Inodos[inodo].posPuntero=pos;
        return 0;
    } else {
        unsigned char buffer[SB2.inodos[inodo-MAX_FILES/2].size];
        readFile(inodo, buffer, SB2.inodos[inodo-MAX_FILES/2].size);
        uint32_t val = CRC32(buffer, SB2.inodos[inodo-MAX_FILES/2].size);
        SB2.inodos[inodo-MAX_FILES/2].crc = val;
        
        Inodos[inodo].posPuntero=pos;
        return 0;
    }
    
    return -2;
}

/*
 * @brief	Opens an existing file and checks its integrity
 * @return	The file descriptor if possible, -1 if file does not exist, -2 if the file is corrupted, -3 in case of error
 */
int openFileIntegrity(char *fileName)
{    

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -3;
    }

    int open = openFile(fileName);

    if (open == -1) return -1;
    else if (open == -2) return -3;

    if (open < MAX_FILES/2) {
        if (SB1.inodos[open].crc == 0) {
            printf("NF10: This file doesn't have a CRC");
            closeFile(open);
            return -3;
        } else {
            int check = checkFile(fileName);

            if (check == -2) return -1;
            else if (check == -1) return -2;
            else {
                printf("%d", open);
                Inodos[open].integridad=1;  //NF12
                return open;
            }
        }
    } else {
        int fd = open - MAX_FILES/2;
        if (SB2.inodos[fd].crc == 0) {
            printf("NF10: This file doesn't have a CRC");
            closeFile(fd);
            return -3;
        } else {
            int check = checkFile(fileName);

            if (check == -2) return -1;
            else if (check == -1) return -2;
            else {
                Inodos[open].integridad=1;  //NF12
                return open;
            }
        }
    }
}

/*
 * @brief	Closes a file and updates its integrity.
 * @return	0 if success, -1 otherwise.
 */
int closeFileIntegrity(int fileDescriptor)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -1;
    }

    if(fileDescriptor < 0 || fileDescriptor > MAX_FILES) return -1;
    
    if (Inodos[fileDescriptor].integridad == 0) {
        printf("NF12: You can't close a file opened using openFile using closeFileIntegrity");
        return -1;
    }

    if(fileDescriptor < MAX_FILES/2) {
        if(includeIntegrity(SB1.inodos[fileDescriptor].nombre) == 0){
            Inodos[fileDescriptor].estado=0;
            Inodos[fileDescriptor].posPuntero=0;

            return 0;
        }
    } else {
        if(includeIntegrity(SB2.inodos[fileDescriptor-MAX_FILES/2].nombre) == 0){
            Inodos[fileDescriptor].estado=0;
            Inodos[fileDescriptor].posPuntero=0;

            return 0;
        }
    }

    return -1;
}

/*
 * @brief	Creates a symbolic link to an existing file in the file system.
 * @return	0 if success, -1 if file does not exist, -2 in case of error.
 */
int createLn(char *fileName, char *linkName)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    if(strlen(fileName) > MAX_NAME_LENGHT || strlen(linkName) > MAX_NAME_LENGHT) return -2;

    int inodo;
    if(namei(linkName) != -1){
        printf("Symbolic link already exists\n");
        return -1;
    }
    if((inodo = namei(fileName))==-1){
        printf("File does not exist\n");
        return -1;
    }
    
    //Comprobamos si el archivo a enlazar es un enlace, en caso afirmativo no se deja crear para evitar ciclos
    if (inodo < MAX_FILES/2) {
        if(SB1.inodos[inodo].tipo==1) {
            return -2;
        }
    } else {
        if(SB2.inodos[inodo-MAX_FILES/2].tipo==1) {
            return -2;
        }
    }
    
    int i;

    //Buscamos un inodo libre
    for(i = 0; i < MAX_FILES; i++) {
        if(bitmap_getbit(SB1.mapaINodos, i)==0){
            bitmap_setbit(SB1.mapaINodos, i, 1);
            break;
        }
    }
    //Si no hay ninguno
    if(i==MAX_FILES) return -2;

    //Asignacion en el inodo
    if(i<MAX_FILES/2){
        SB1.inodos[i].size=0;
        SB1.inodos[i].tipo=1;   //Es simbolico
        strcpy(SB1.inodos[i].nombre, linkName);
        if(inodo<MAX_FILES/2) SB1.inodos[i].bloque[0] = SB1.inodos[inodo].bloque[0];
        else{
            inodo -= MAX_FILES/2;
            SB1.inodos[i].bloque[0] = SB2.inodos[inodo].bloque[0];
        }
        return 0;
    }else{
        i -= MAX_FILES/2;

        SB2.inodos[i].size=0;
        SB2.inodos[i].tipo=1;   //Es simbolico
        strcpy(SB2.inodos[i].nombre, linkName);
        if(inodo<MAX_FILES/2) SB2.inodos[i].bloque[0] = SB1.inodos[inodo].bloque[0];
        else{
            inodo -= MAX_FILES/2;
            SB2.inodos[i].bloque[0] = SB2.inodos[inodo].bloque[0];
        }
        return 0;
    }
	return -2;
}

/*
 * @brief 	Deletes an existing symbolic link
 * @return 	0 if the file is correct, -1 if the symbolic link does not exist, -2 in case of error.
 */
int removeLn(char *linkName)
{

    //Comprobamos que sea nuestro FS
    if(SB1.numMagico != 100383438) {
        printf("Este FS no es PotooooooooFS");
        return -2;
    }

    if(strlen(linkName) > MAX_NAME_LENGHT) return -2;

    //Cogemos el inodo
    int inodo = namei(linkName);
    
    if(inodo == -1){
        printf("Symbolic link does not exist\n");
        return -1;
    }
    
    //Liberamos el inodo
    bitmap_setbit(SB1.mapaINodos, inodo, 0);

    //Borramos la informacion del inodo
    if(inodo < MAX_FILES/2) {
        memset(SB1.inodos[inodo].nombre, '\0', MAX_NAME_LENGHT+1);
        bitmap_setbit(SB1.mapaBloques, SB2.inodos[inodo].bloque[0], 0);
        SB1.inodos[inodo].bloque[0] = 0;      
        return 0;
    } else {
        inodo -= MAX_FILES/2;
        memset(SB2.inodos[inodo].nombre, '\0', MAX_NAME_LENGHT+1);
        bitmap_setbit(SB1.mapaBloques, SB2.inodos[inodo].bloque[0], 0);
        SB2.inodos[inodo].bloque[0] = 0;      
        return 0;
    }    
    return -2;
}

/*
 * @brief 	        Translates name of file into inode
 * @return 	        i>0 if the file exists, -1 if the file does not exist.
 * @superblocks 	i<MAX_FILES/2 if the inode in SB1, i>MAX_FILES/2 if the inode in SB2.
 */
int namei(char *file_name)
{

    if(strlen(file_name) > MAX_NAME_LENGHT) return -2;

    for (int i = 0; i < MAX_FILES/2; i++)
    {
        if(strcmp(SB1.inodos[i].nombre, file_name)==0) return i;
        else if(strcmp(SB2.inodos[i].nombre, file_name)==0) return i+MAX_FILES/2;
    }
    
    return -1;
}

/*
 * @brief 	        Translates a block into the original file
 * @return 	        i>0 if the file exists, -1 if the file does not exist or is SuperBlock.
 * @superblocks 	i<MAX_FILES/2 if the inode in SB1, i>MAX_FILES/2 if the inode in SB2.
 */
int bi(int block)
{
    if(block<2) return -1;  //Superbloques

    for (int i = 0; i < MAX_FILES/2; i++)
    {
        if(SB1.inodos[i].bloque[0]==block){
            if(SB1.inodos[i].tipo!=1) return i;
        }
        else if(SB2.inodos[i].bloque[0]==block){
            if(SB1.inodos[i].tipo!=1) return i+MAX_FILES/2;
        }
    }
    
    return -1;
}

/*
 * @brief 	        Translates a position into block, if the block does not exist it is created in the inode
 * @return 	        returns the actual block
 */
int bmap(int i, int pos){
    int bloquepuntero = 0;
    if(i < MAX_FILES/2){
        if((bloquepuntero = SB1.inodos[i].bloque[pos / (BLOCK_SIZE-1)])==0){
            //Buscamos un bloque libre
            for(int h = 2 ; h < SB1.diskSize/BLOCK_SIZE ; h++){ //Los dos bloques de SB
                if(bitmap_getbit(SB1.mapaBloques, h)==0){
                    bitmap_setbit(SB1.mapaBloques, h, 1);
                    bloquepuntero = h;
                    SB1.inodos[i].bloque[pos / BLOCK_SIZE-1]= bloquepuntero;
                    break;
                }
            }
        }
    }else{
        if((bloquepuntero = SB2.inodos[i-MAX_FILES/2].bloque[pos / (BLOCK_SIZE-1)])==0){
            //Buscamos un bloque libre
            for(int h = 2 ; h < SB1.diskSize/BLOCK_SIZE ; h++){ //Los dos bloques de SB
                if(bitmap_getbit(SB1.mapaBloques, h)==0){
                    bitmap_setbit(SB1.mapaBloques, h, 1);
                    bloquepuntero = h;
                    SB2.inodos[i-MAX_FILES/2].bloque[pos / BLOCK_SIZE-1]= bloquepuntero;
                    break;
                }
            }
        }
    }
    if(bloquepuntero==0) return -1; //el fichero no tiene nada
    return bloquepuntero;
}

/*
 * Reads the number oh bytes you wish
 * Returns 0 or -1 in case of error, including short
 * read.
 */
int customRead(int fileDescriptor, char *buffer, int size) {
	
    //Abrimos disk.dat
    int fd = open(DEVICE_IMAGE, O_RDONLY);
    if(fd < 0) return -1;

    //Movemos el puntero a donde estamos
    int offset = bmap(fileDescriptor, Inodos[fileDescriptor].posPuntero) * BLOCK_SIZE + Inodos[fileDescriptor].posPuntero % BLOCK_SIZE;
	lseek(fd, offset, SEEK_SET);

    //Leemos el tamano que queremos y lo dejamos en el buffer
	read(fd, buffer, size);

	close(fd);
	return 0;
}
