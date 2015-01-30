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

#include "../headers/defines.h"
#include "../headers/struct.h"
#include "./fonctions/fonctionClient.c"

#include "./fonctions/sign_in.c"
#include "./fonctions/sign_up.c"
#include "./fonctions/mode.c"

#include "./fonctions/admin_mode.c"

int main(int argc, char * argv[]) 
{

	// VARIABLES
	
	int choix = -1;
	int mode = -1;
	char buffer[1024]; char ask; //(y/n)
	clean_b(buffer);


	char * server_name;
	struct hostent * hp;
	struct sockaddr_in nom_svr;
	struct user_t client;
	size_t size_recv;

	// DEBUT
	welcome_message(argc, argv);	
	
	do
	{
		printf("Serveur: "); // ADRESSE DU SERVEUR
		__fpurge(stdin);
	}
	while(scanf("%s", buffer) != 1);
	
	debugm("[DEBUG]: malloc => server_name\n"); //DEBUG
	if((server_name = malloc(strlen(buffer))) == NULL)
	{
		fprintf(stderr,"[ERROR]: Erreur malloc\nExiting...\n");
		exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: malloc reussi => @server_name = %p\n", &server_name); //DEBUG
	if(debug) fprintf(stdout, "[DEBUG]: copie: buffer -> server_name\n"); //DEBUG
	strcpy(server_name, buffer);  // COPIE DU NOM DU SERVEUR DEPUIS LE BUFFER
	if(debug) fprintf(stdout, "[DEBUG]: copie reussi => server_name:%s\n", server_name); //DEBUG
	if(debug) fprintf(stdout, "[DEBUG]: Vidange buffer\n"); //DEBUG
	clean_b(buffer);
	if(debug) fprintf(stdout, "[DEBUG]: Vidange buffer terminée: buffer = '%s'\n", buffer); //DEBUG
	
	
	// CONNEXION AU SERVEUR
	
	/* construction du socket local */
	if(debug) fprintf(stdout, "[DEBUG]: Construction du socket local\n"); //DEBUG
	if((client.socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) /* (IPv4 ,flux de donnée binaire (en mode connecte), protocole TCP) */
	{
	 fprintf(stderr, "[ERROR]: Could not create de socket\n");
	 exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: Construction réussi: sock = %d\n", client.socket_fd); //DEBUG

	/* construction du nom externe du sokcet distant */
	if(debug) fprintf(stdout, "[DEBUG]: Resolution du Nom de domaine du serveur\n"); // DEBUG
	if((hp = gethostbyname(server_name)) == NULL)
	{
		fprintf(stderr,"[ERROR]: Could not determine the host\nExiting...\n");
		exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: Resolution reussi !\n"); // DEBUG

	nom_svr.sin_family = AF_INET;
	nom_svr.sin_port = PORT_NUMBER;
	bcopy(hp->h_addr, (char *)&(nom_svr.sin_addr.s_addr), hp->h_length);

	/* connexion au serveur */
	if(DEBUG_ON) fprintf(stdout, "[DEBUG]: Connection au serveur...\n"); //DEBUG
	if(connect(client.socket_fd, (struct sockaddr *)&nom_svr, sizeof(nom_svr)) == -1)
	{
		fprintf(stderr, "[ERROR]: Could not open the connection\nExiting...\n");
		exit(-1);
	}
	fprintf(stdout,"\n\n *** CONNEXION ETABLIE ***\n\n");
	
	/* Connexion ou inscription */
	
	do
	{
		printf("Que voulez-vous faire ?\n"); // CHOIX DU MODE
			printf("\t 1 - Se connecter au serveur\n");
			printf("\t 2 - S'inscrire sur le serveur\n");
			printf("\t 0 - Quitter le programme \n");
		printf("Choix[1|2|0]: ");
		__fpurge(stdin);
	}while(((scanf("%d", &choix)) != 1) || (choix > 2));
	
	switch(choix) // choix de s'incrire ou de se connecter
	{
		case 0:
			exit(SUCCESS);
		case 1: // Connexion au serveur
			debugm("Connexion au serveur");
			if(req_connect(&client, buffer) == FAIL);
			{
				errorm("Vous n'avez pas pû vous connecter au serveur\n Exiting...\n");
				exit(FAIL);
			}
			break;
			
		case 2: //Inscription puis connexion direct
			debugm("Inscription sur le serveur");
			if(req_sign_up(&client, buffer) == FAIL)
			{
				errorm("Vous n'avez pas pû vous inscrire sur le serveur\n Exiting...\n");
				exit(FAIL);
			}
			if(req_connect(&client, buffer) == FAIL);
			{
				errorm("Vous n'avez pas pû vous connecter au serveur\n Exiting...\n");
				exit(FAIL);
			}
			break;
		default:
			debugm("Choix de connexion par défaut");
			errorm("Choix de connexion différent de 0|1|2");
			exit(FAIL);
			break;
	}

 	// CHOIX DU MODE
	/* Vendeur: Permet d'envoyer des nouveaux objets au serveur pour les mettre aux enchères;
 	 * Acheteur: Permet de placer des enchères sur un objet;
 	 * Administrateur: Permet de gérer les objets vendus aux enchères
 	 */
	
	req_mode(&client); 
	if(client.mode == 'b') 			// SI MODE ACHETEUR
	{
	} 
	else if(client.mode == 's') 	// SI MODE VENDEUR
	{
		
	}
	else if(client.mode == 'a')     // SI MODE ADMINISTRATEUR
	{
		admin_mode(&client,buffer);
	}
	else
	{
		// ERREUR
	}
	
	
	// FERMETURE DES CONNEXIONS ET LIBERATIONS DES RESSOURCES
	close(client.socket_fd);
	free(server_name);
	return 0;
}