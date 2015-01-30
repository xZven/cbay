#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio_ext.h>
//
#include "../headers/defines.h"
#include "../headers/struct.h"
#include "./client.h"
#include "./fonctionClient.c"


//


int main(int argc, char * argv[]) 
{

	// VARIABLES
	
	char buffer[1024]; char ask; //(y/n)
	clean_b(buffer);
	
	int mode;  //variable servant à choisir le mode de connexion

	char * server_name;
	struct hostent * hp;
	struct sockaddr_in nom_svr;
	struct user_t client;
	size_t size_recv;

	// DEBUT
	welcome_message(argc, argv);	
	
	do
	{
		printf("Entrez l'adresse du serveur: "); // ADRESSE DU SERVEUR
		__fpurge(stdin);
	}
	while(scanf("%s", buffer) != 1);
	
	if(debug) fprintf(stdout, "[DEBUG]: malloc => server_name\n"); //DEBUG
	if((server_name = malloc(strlen(buffer))) == NULL)
	{
		fprintf(stderr,"[ERROR]: Erreur malloc\nExiting...\n");
		exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: malloc reussi => @server_name = %p\n", &server_name); //DEBUG
	if(debug) fprintf(stdout, "[DEBUG]: copie: buffer -> server_name\n"); //DEBUG
	strcpy(server_name, buffer);  // COPIE DU NOM DU SERVEUR DEPUIS LE BUFFER
	if(debug) fprintf(stdout, "[DEBUG]: copie reussie => server_name:%s\n", server_name); //DEBUG
	if(debug) fprintf(stdout, "[DEBUG]: Vidange buffer\n"); //DEBUG
	clean_b(buffer);
	if(debug) fprintf(stdout, "[DEBUG]: Vidange buffer terminée: buffer = '%s'\n", buffer); //DEBUG
	
	
	// CONNEXION AU SERVEUR
	
	/* construction du socket local */
	if(debug) fprintf(stdout, "[DEBUG]: Construction du socket local\n"); //DEBUG
	if((client.socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) /* (IPv4 ,flux de donnée binaire (en mode connecté), protocole TCP) */
	{
	 fprintf(stderr, "[ERROR]: Could not create the socket\n");
	 exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: Construction réussie: sock = %d\n", client.socket_fd); //DEBUG

	/* construction du nom externe du sokcet distant */
	if(debug) fprintf(stdout, "[DEBUG]: Resolution du Nom de domaine du serveur\n"); // DEBUG
	if((hp = gethostbyname(server_name)) == NULL)
	{
		fprintf(stderr,"[ERROR]: Could not determine the host\nExiting...\n");
		exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: Resolution reussie !\n"); // DEBUG

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
	fprintf(stdout,"\n\n[INFO]: *** CONNEXION ETABLIE ***\n\n");







 	// CHOIX DU MODE
	/* Vendeur: Permet d'envoyer des nouveaux objets au serveur pour les mettre aux enchères;
 	 * Acheteur: Permet de placer des enchères sur un objet;
 	 * Administrateur: Permet de gérer les objets vendus aux enchères
 	 */

printf("Connexion en tant que:\n 1-acheteur\n 2-vendeur\n 3-Administrateur(nécessite les droits)");
scanf("%d", &mode);

/**/*/**/
// SI MODE ACHETEUR

if(mode == 1){
	sprintf(buff, "REQ_MODE = BUY FOR %ld \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	if(strcmp(buffer, "MODE = BUY FOR %ld \n") == 0){
		printf("Vous allez entrez dans le mode acheteur, préparez vous mentalement...");
	else
		printf("erreur serveur, vous allez être déconnecté...");
	exit;
	}

do	{
	printf("Que souhaitez-vous faire?\n 1-consulter mon historique d'achats\n 2-gérer mes enchères en cours\n 3-effectuer une nouvelle enchère\n")
	}
while ((scanf("%d", &choix) != 1 ) && ((choix != 1) && (choix != 2) && (choix != 3));

if(choix == 1) //historique achats
	sprintf(buff, "REQ_HIST_BOUGHT BY %ld \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	if(strcmp(buffer, "REQ_HIST_BOUGHT BY %ld \n") == 0){
//HELP ME BREAUUUUUU pour la récpetion des données des objets et surtout leur affichage...


}

if(choix == 2) //gestion enchères en cours
	sprintf(buff, "	REQ_CAT \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	int i = 0;
	int j = 0;
	char * category[20];
	printf("A quelle catégorie souhaitez-vous accéder?");
	do	{
		category[i] = buffer;
		i = i+1;		
		}while(strcmp(buffer, "END_CATEGORY \n") != 0); // boucler tant que le client n'a pas reçu "end_category"

for (j = 0 ; j < i ; i++)

    {
        printf("%d\n", tableau[i]);
    }

scanf(%d, &choix);
printf("Vous avez choisi la catégorie %s", tableau[choix]);
	sprintf(buff, "	REQ_CAT_ACCESS =  \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
//TOUJOURS NEED UR HILP









}


// SI MODE VENDEUR
else if (mode == 2){
	sprintf(buff, "REQ_MODE = BUY FOR %ld \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	if(strcmp(buffer, "MODE = BUY FOR %ld \n") == 0){
		printf("Vous êtes sur le point de vendre un de vos biens, prenez garde...");
	else
		printf("erreur serveur, vous allez être déconnecté...");
	exit;
	}


do	{
	printf("Que souhaitez-vous faire?\n 1-consulter mon historique de ventes\n 2-gérer mes ventes en cours\n 3-ajouter un nouvel objet en vente\n")
	}
while ((scanf("%d", &choix) != 1 ) && ((choix != 1) && (choix != 2) && (choix != 3));

if(choix == 1) //historique des ventes
	

sprintf(buff, "REQ_ITEM_SOLD = %ld \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
//HELP ME BREAUUUUUU pour la récpetion des données des objets et surtout leur affichage...

}
if(choix == 2))
	sprintf(buff, "REQ_ITEM_USER = %ld \n", client->uid);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
//HELP ME BREAUUUUUU pour la récpetion des données des objets et surtout leur affichage...

}

if(choix == 3)
	





	





}
 // SI MODE ADMINISTRATEUR

 // FERMETURE DES CONNEXIONS ET LIBERATIONS DES RESSOURCES
	close(client.socket_fd);
	free(server_name);
	return 0;
}
