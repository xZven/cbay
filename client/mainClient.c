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
#include "./fonctions/seller_mode.c"
#include "./fonctions/buyer_mode.c"

int main(int argc, char * argv[]) 
{

	// VARIABLES
	
	int choix = -1;
	char buffer[1024];
	

	char * server_name;
	struct hostent * hp;
	struct sockaddr_in nom_svr;
	struct user_t client;

	welcome_message(argc, argv);	
	clean_b(buffer);
	
	 // demande du serveur
	do
	{
		__fpurge(stdin);
		printf("Adresse du serveur: "); // ADRESSE DU SERVEUR
	}while(scanf("%s", buffer) != 1);
	debugm(buffer);
	
	if((server_name = malloc(strlen(buffer))) == NULL)
	{
		errorm("Erreur d'allocation mémoire pour le nom du serveur\nExiting...");
		exit(FAIL); //
	}
	strcpy(server_name, buffer);  // COPIE DU NOM DU SERVEUR DEPUIS LE BUFFER
	clean_b(buffer);
	
	// CONNEXION TCP AU SERVEUR
	
	/* construction du socket local */
	debugm("Construction du socket local");
	if((client.socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) /* (IPv4 ,flux de donnée binaire (en mode connecte), protocole TCP) */
	{
		errorm("Could not create de socket\nExiting...");
		exit(FAIL); //
	}
	debugm("Construction réussi"); 

	/* construction du nom externe du sokcet distant */
	debugm("Resolution du Nom de domaine du serveur");
	if((hp = gethostbyname(server_name)) == NULL)
	{
		errorm("Could not determine the host\nExiting...\n");
		exit(FAIL); //
	}
	debugm("Resolution reussi !");

	nom_svr.sin_family = AF_INET;
	nom_svr.sin_port = PORT_NUMBER; // NUMERO DE PORT
	bcopy(hp->h_addr, (char *)&(nom_svr.sin_addr.s_addr), hp->h_length);

	/* connexion au serveur */
	debugm("Connection au serveur...");
	
	if(connect(client.socket_fd, (struct sockaddr *)&nom_svr, sizeof(nom_svr)) == -1)
	{
		errorm("Could not open the connection\nExiting...");
		exit(FAIL);
	}
	
	greenm("\n\n *** CONNEXION ETABLIE AVEC LE SERVEUR ***\n");
	
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
			req_connect(&client, buffer);
			
			break;
			
		case 2: //Inscription puis connexion direct
			debugm("Inscription sur le serveur");
			if(req_sign_up(&client, buffer) == FAIL)
			{
				echecm("Vous n'avez pas pû vous inscrire sur le serveur\nExiting...\n");
				exit(FAIL);
			}
			req_connect(&client, buffer);
			
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
	
	req_mode(&client, buffer); 
	if(client.mode == 'b') 			// SI MODE ACHETEUR
	{
		buyer_mode(&client, buffer);
		debugm("MODE = ACHETEUR");
	} 
	else if(client.mode == 's') 	// SI MODE VENDEUR
	{
		debugm("MODE = VENDEUR");
		seller_mode(&client, buffer);
	}
	else if(client.mode == 'a')     // SI MODE ADMINISTRATEUR
	{
		debugm("MODE = ADMINISTRATEUR");
		admin_mode(&client, buffer);
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