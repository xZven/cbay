#ifndef _HEADER_H_
#define _HEADER_H_

/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
// CONSTANTES
#define DEBUG_ON 1
#define DEBUG_OFF 0
#define PORT_NUMBER 1234

//type bool
#define TRUE 1
#define FALSE 0

#define LOGIN_FREE 1
#define LOGIN_BUSY 0

#define CONNECT_SUCCESS 1
#define CONNECT_FAIL 0

//type state
#define SUCCESS 1
#define FAIL 0

//client
#define NMAX_CATEGORY 20
#define NMAX_ITEM 256

//printf color
#define ECHEC "[\033[31mECHEC\033[0m]"
#define ERROR "[\033[31mERROR\033[0m]"
#define WAR	  "[\033[34mWARNING\033[0m]"
#define INFO  "[\033[32mINFO\033[0m]"
#define DEBUG_MSG "[\033[35mDEBUG\033[0m]"
#define GREEN "\033[32m"
#define RED   "\033[31m"
#define NORM  "\033[0m"

// mode de connexion

#define BUYER  'b'
#define SELLER 's'
#define ADMIN  'a'

// VARIABLES GLOBAL
int debug; 						// Utilisé pour le mode debug
time_t server_time;
char auth_file[50];
char object_file[50];

// TYPEDEF
typedef time_t unique_id_t; 	// UNIQUE ID
typedef int fd_t; 				// DESCRIPTEUR DE FICHIER
typedef int bool;				// TYPE BOOLEAN
typedef int state;				// type state pour les états des requêtes
typedef char mode;				// TYPE mode pour le choix du mode

#endif

/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */