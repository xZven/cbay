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
			else if(strncmp(buffer, "REQ_VERIFY_LOGIN", 	13) == 0)
			{
				req_verify_login(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_NEW_USER", 		12) == 0) 	
			{
				req_new_user(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_CONNECT", 			11) == 0) 	
			{
				req_connect(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_MODE", 			8) == 0)
			{
				req_mode(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_NEW_PW", 			10) == 0)
			{
				req_new_pw(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_DEL_USER", 		12) == 0)
			{
				req_del_user(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_ALL_ITEM", 		12) == 0)
			{
				req_all_item(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_OP", 				6) == 0)
			{
				req_op(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_ITEM_SOLD", 		13) == 0)
			{
				req_item_sold(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_ITEM_USER", 		13) == 0)
			{
				req_item_user(&client, &server, buffer);
			}
			else if(strncmp(buffer, "PUT_NEW_ITEM", 		12) == 0)
			{
				put_new_item(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_HIST_ITEM_BOUGHT", 20) == 0)
			{
				req_hist_item_bought(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_BID_USER", 		12) == 0)
			{
				req_bid_user(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_CAT", 				7) == 0)
			{
				req_cat(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_CAT_ACCESS", 		14) == 0)
			{
				req_cat_access(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_ITEM", 			8) == 0)
			{
				req_item(&client, &server, buffer);
			}
			else if(strncmp(buffer, "REQ_BID_PRICE", 		13) == 0)
			{
				req_bid_price(&client, &server, buffer);
			}
			else
			{
			}
			
		}
}
	
	
	shut_server(&server);
	exit(0);
}	