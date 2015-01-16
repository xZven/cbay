#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio_ext.h>
#include <signal.h>

#include "../headers/defines.h"
#include "../headers/struct.h"

#include "./fonctions/server_f.c"
#include "./fonctions/log_f.c"
#include "./fonctions/req_f.c"




int main (int argc, char * argv[])
{

//VARIABLES
	

	struct server_t server;
	char buffer[1024]; buffer[0]='\0';
	
	/* variables liées au socket */
	int socklis;	
	struct sockaddr_in nom_server;
	struct sockaddr nom_client;
	socklen_t len_nom_client;
	
	size_t size_recv;

	/* variables liées aux client */
	struct user_t client;
	
//DEBUT

	welcome_message(argc, argv);

	//CHARGEMENT DES PARAMETRES
	fprintf(stdout, "[INFOS]: Chargement des paramètres du serveurs\n");
	load_server(&server);
	fprintf(stdout, "[INFOS]: Chargement des paramètres terminés\n\n\n");
	screen_server(server);
	
	//PREPARATION DES CONNEXIONS
	init_socket(&socklis, &nom_server, &server);
	
	while(1)
{
	//ATTENTE DE CONNEXION D'UN CLIENT	
	if(listen(socklis, 1) == -1)
	{
		fprintf(stderr, "[ERROR]: listen(): %s\nExiting...\n", strerror(errno));
		exit(-1);
	}
	fprintf(stdout, "\n\n***[WAR]: ATTENTE DE CONNEXION ***\n\n");
	
	//CONNECTION
	len_nom_client = sizeof(nom_client);
	client.socket_fd = accept(socklis, &nom_client, &len_nom_client);
	if(client.socket_fd == -1)
	 {
		fprintf(stderr, "[ERROR]: accept(): %s\nExiting...\n", strerror(errno));
		exit(-1);
	 }
	fprintf(stdout, "*** CONNECTED TO CLIENT ***\n");
	
	while(client.socket_fd > 0) // TANT QUE LE CLIENT EST CONNECTE
	{
		clean_b(buffer);	
		do
		{
			size_recv = recv(client.socket_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
			usleep(10000); //PAUSE DE 10 mS	
		}while(size_recv == -1);		
		if(debug) fprintf(stdout, "[DEBUG]: Buffer: %s\n", buffer);
		
		if(0)
		{
		}
		else if(strncmp(buffer, "REQ_VERIFY_LOGIN", 13) == 0)	//passro
		{
			req_verify_login(&client, &server, buffer);
		}
		else if(strncmp(buffer, "REQ_NEW_USER", 12) == 0) 	// 
		{
			req_new_user(&client, &server, buffer);
		}
		else if(strncmp(buffer, "REQ_CONNECT", 14) == 0) 	// 
		{
			req_connect(&client, &server, buffer);
		}
	}
}
	
	
	shut_server(&server);
	exit(0);
}	