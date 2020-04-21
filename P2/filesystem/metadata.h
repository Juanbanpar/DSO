
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
struct Superbloque;

// typedef struct {
//     unsigned int numInodos; /* Número de inodos en el dispositivo */
//     unsigned int numBloquesDatos; /* Número de bloques de datos en el disp. */
//     unsigned int *primerBloqueDatos; /* Número de bloque del 1º bloque de datos */
//     unsigned int tamDispositivo; /* Tamaño total del disp. (en bytes) */
//     //char relleno[PADDING_SB]; /* Campo de relleno (para completar un bloque) */
// } TipoSuperbloque;
// 
// typedef struct {
//     char nombre[32]; /* Nombre del fichero/ directorio asociado */
//     uint16_t tamanyo; /* Tamaño actual del fichero en bytes */
//     //unsigned int bloqueIndirecto; /* Número del bloque indirecto */
//     //char *relleno; /* Campo relleno para llenar un bloque con malloc a la resta de lo que le quede al fichero*/
//     uint32_t crc;
// } TipoInodoDisco;

typedef struct INodo {
    char nombre[MAX_NAME_LENGHT];   //Nombre del fichero
    uint8_t bloque;                 //Número del bloque que al que pertenece
    uint16_t size;                  //Tamaño del fichero en bytes
    uint16_t puntero;               //Puntero del fichero
    uint8_t estado;                 //Estado del archivo
    uint32_t crc;                   //CRC32
} INodo;

typedef struct Superbloque {
    uint16_t numMagico;                     //Número mágico que indentifica nuestro sistema de ficheros
    long diskSize;                          //El tamaño del disco
    uint64_t mapaINodos;                    //Mapa de inodos
    uint64_t mapaBloques;                   //Mapa de bloques
    struct INodo inodos[MAX_FILES];         //Array de inodos
    //char *relleno; /* Campo relleno para llenar un bloque con malloc a la resta de lo que le quede al fichero*/
} Superbloque;
