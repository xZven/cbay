/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

// STRUCTURE DEFINISSANT UN UTILISATEUR 
struct user_t 				
{
	unique_id_t uid; 		// IDENTIFIANT UNIQUE
	char login[51];			// NOM D'UTILISATEUR
	char password[51];		// MOT DE PASSE
	bool admin;				// MODE (TRUE or FALSE)
	char mode;				// MODE DE CONNEXION (a, b ou s)
	time_t last_connect;	// HEURE DE LA DERNIERE CONNEXION (format seconde)
	//IP					// DERNIERE ADDRESSE IP DE CONNEXION
	fd_t socket_fd;			// DESCRIPTEUR DE FICHIER DU SOCKET DE COMMUNICATION
};

// STRUCTURE DEFINISSANT UN OBJET DES ECNHERES
struct object_t 			
{
	unique_id_t uid;		// IDENTIFIANT UNIQUE DE L ITEM
	char name[51];			// NOM DE L'OBJET (à l'unité) exemple: pour un lot de XXX: "XXX"
	char category[51];		// CATEGORIE (mobilier, électro, auto, vêtement, livre, )
	char description[1024];	// DESCRIPTION DE L'OBJET OU DU LOT MIS AUX ENCHERES
	char url_image[100];	// URL IMAGE
	float start_price; 		// PRIX DE DEPART DE L'ENCHERE
	float temp_price;  		// PRIX INTERMEDIAIRES (prix proposé durant l'enchère par les utilisateurs, le prix affiché est celui le plus élevé)
	float final_price; 		// PRIX FINAL DE L'ENCHERE
	int quantity;			// QUANTITE (le prix de l'enchère est pour tout le lot)
	char place[100];         // ADRESSE DE L'OBJETS
	unique_id_t vendeur;	// UID DU VENDEUR
	unique_id_t acheteur;	// UID DE L'ACHETEUR (initialisé à 0 / mis à 0 lors de la suppresion d'un utilisateur lros d'un enchère)
};

// STRUCTURE REGROUPANT TOUS LES PARAMETRES DU SERVEUR
struct server_t				
{
	char server_name[51];	// NOM DU SERVEUR
	int port_number;		// NUMERO DE PORT
	FILE * auth_file;		// POINTEUR VERS LE FICHIER D'AUTHENTIFICATION DES UTILISATEURS
	FILE * object_file;		// POINTEUR VERS LE FICHIER DES OBJETS AUX ENCHERES
	FILE * log_file;		// POINTEUR VERS LE FICHIER DES LOGS DES ACHATS ET DES VENTES 
};							// CES PARAMETRES SONT CHANGEABLE DANS LE FICHIER DE CONF DU SERVEUR

struct log_t
{
	unique_id_t item_uid;	// IDENTIFIANT UNIQUE DE L'ITEM
	unique_id_t seller_uid;	// IDENTIFIANT UNIQUE DU VENDEUR
	unique_id_t buyer_uid;	// IDENTIFIANT UNIQUE DE L'ACHETEUR
	char event;				// EVENEMENT
	time_t date;			// DATE DE L'EVENEMENT
};

/* EVENEMENT POSSIBLE */
/*
 *  i 		inscription
 *	c		connection	// connection d'un utilisateur
 *	b		bid 		// mise au enchère
 *	l		canceL		// annulation d'une enchère
 *	d		delete		// suppresion d'un item
 *
 *
 *
 *
 */