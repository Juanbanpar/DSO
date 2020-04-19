
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

typedef struct {
    unsigned int numInodos; /* Número de inodos en el dispositivo */
    unsigned int numBloquesDatos; /* Número de bloques de datos en el disp. */
    unsigned int *primerBloqueDatos; /* Número de bloque del 1º bloque de datos */
    unsigned int tamDispositivo; /* Tamaño total del disp. (en bytes) */
    //char relleno[PADDING_SB]; /* Campo de relleno (para completar un bloque) */
} TipoSuperbloque;

typedef struct {
    char nombre[32]; /* Nombre del fichero/ directorio asociado */
    uint16_t tamanyo; /* Tamaño actual del fichero en bytes */
    //unsigned int bloqueIndirecto; /* Número del bloque indirecto */
    //char *relleno; /* Campo relleno para llenar un bloque con malloc a la resta de lo que le quede al fichero*/
    uint32_t crc;
} TipoInodoDisco;

