#ifndef _HEADER_H_
#define _HEADER_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio_ext.h>


// PROTOTYPE
void welcome_message(int argc, char * argv[]);

// CONSTANTES
#define DEBUG_ON 1
#define DEBUF_OFF 0
#define PORT_NUMBER 1234


struct user_t
{
	time_t uid; 
	char user_name[31];
	char password[21];
};

struct object_t
{
	char category[51];
	char name[51];
	char description[1024];
	char url_image[100];
	float start_price;
	float final_price;
	int quantity;
	char place[50];
	struct user_t vendeur;
	struct user_t acheteur;
	struct user_t administrateur;
};

#endif
