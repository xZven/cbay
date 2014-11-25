#ifndef _HEADER_H_
#define _HEADER_H_

struct user_t 				// STRUCTURE DEFINISSANT UN UTILISATEUR 
{
	unique_id_t uid; 		// IDENTIFIANT UNIQUE
	char login[21];			// NOM D'UTILISATEUR
	char password[21];		// MOT DE PASSE
	char mode[21];			// MODE (administrateur/acheteur/vendeur)
	char cmd[30];			// DERNIERE REQUETE UTILISEE
	time_t last_connect;	// HEURE DE LA DERNIERE CONNEXION (format seconde)
	//IP					// DERNIERE ADDRESSE IP DE CONNEXION
	fd_t socket_fd;			// DESCRIPTEUR DE FICHIER DU SOCKET DE COMMUNICATION
};

struct object_t 			// STRUCTURE DEFINISSANT UN OBJET DES ECNHERES
{
	char name[51];			// NOM DE L'OBJET (à l'unité) exemple: pour un lot de XXX: "XXX"
	char category[51];		// CATEGORIE (mobilier, électro, auto, vêtement, livre, )
	char description[1024];	// DESCRIPTION DE L'OBJET OU DU LOT MIS AUX ENCHERES
	char url_image[100];	// URL IMAGE
	float start_price; 		// PRIX DE DEPART DE L'ENCHERE
	float temp_price;  		// PRIX INTERMEDIAIRES (prix proposé durant l'enchère)
	float final_price; 		// PRIX FINAL DE L'ENCHERE
	int quantity;			// QUANTITE (le prix de l'enchère est pour tout le lot)
	char place[50];         // ADRESSE DU VENDEUR
	unique_id_t vendeur;	// UID DU VENDEUR
	unique_id_t acheteur;	// UID DE L'ACHETEUR
};

struct server_t				// STRUCTURE REGROUPANT TOUS LES PARAMETRES DU SERVEUR
{
	char server_name[51];	// NOM DU SERVEUR
	int port_number;		// NUMERO DE PORT
	FILE * auth_file;		// POINTEUR VERS LE FICHIER D'AUTHENTIFICATION DES UTILISATEURS
	FILE * object_file;		// POINTEUR VERS LE FICHIER DES OBJETS AUX ENCHERES
	FILE * log_file;		// POINTEUR VERS LE FICHIER DES LOGS DES ACHATS ET DES VENTES 
};							// CES PARAMETRES SONT CHANGEABLE DANS LE FICHIER DE CONF DU SERVEUR

#endif