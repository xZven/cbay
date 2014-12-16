/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
#ifndef _HEADER_H_
#define _HEADER_H_
// CONSTANTES
#define DEBUG_ON 1
#define DEBUG_OFF 0
#define PORT_NUMBER 1234

#define TRUE 1
#define FALSE 0

#define LOGIN_FREE 1
#define LOGIN_BUSY 0

#define SUCCESS 0
#define FAIL 1

// VARIABLES GLOBAL
int debug; 						// Utilisé pour le mode debug

// TYPEDEF
typedef time_t unique_id_t; 	// UNIQUE ID
typedef int fd_t; 				// DESCRIPTEUR DE FICHIER
typedef int bool;				// TYPE BOOLEAN

#endif