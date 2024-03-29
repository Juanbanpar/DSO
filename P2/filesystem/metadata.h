
/*
 *
 * Operating System Design / Diseño de Sistemas Operativos
 * (c) ARCOS.INF.UC3M.ES
 *
 * @file 	metadata.h
 * @brief 	Definition of the structures and data types of the file system.
 * @date	Last revision 01/04/2020
 *
 */

#define bitmap_getbit(bitmap_, i_) (bitmap_[i_ >> 3] & (1 << (i_ & 0x07)))
static inline void bitmap_setbit(char *bitmap_, int i_, int val_) {
  if (val_)
    bitmap_[(i_ >> 3)] |= (1 << (i_ & 0x07));
  else
    bitmap_[(i_ >> 3)] &= ~(1 << (i_ & 0x07));
}

#include <stdint.h>
#define MAX_FILES 48        //NF1
#define MAX_NAME_LENGHT 32  //NF2
#define BLOCK_SIZE 2048     //NF4

struct INodo;
struct INodoX;
struct Superbloque1;
struct Superbloque2;

typedef struct INodo {
    char nombre[MAX_NAME_LENGHT + 1];   //Nombre del fichero
    uint16_t bloque[5];                 //Bloques en los que está presente
    uint8_t tipo;                       //1 si es enlace
    uint16_t size;                      //Tamaño del fichero en bytes
    uint32_t crc;                       //CRC32
} INodo;

typedef struct INodoX {
    uint32_t posPuntero;    //Puntero, mirar requisitos.
    uint8_t estado;         //Estado del archivo
    uint8_t integridad;     //Para comprobar si se abrió con o sin integridad
} INodoX;

typedef struct Superbloque1 {
    uint32_t numMagico;                 //Número mágico que indentifica nuestro sistema de ficheros
    uint64_t diskSize;                  //El tamaño del disco
    char mapaINodos[MAX_FILES/8];       //Mapa de inodos
    char mapaBloques[(MAX_FILES*5)/8];  //Mapa de bloques
    struct INodo inodos[MAX_FILES / 2]; //Array de inodos
    char relleno[747];
} Superbloque1;

typedef struct Superbloque2 {
    struct INodo inodos[MAX_FILES / 2]; //Array de inodos
    char relleno[800];
} Superbloque2;
