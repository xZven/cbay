/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

/* AFFICHAGE CONSOLE */
/*
 * Ces fonctions recevoit en paramètres le message qui sera affiché dans la sortie standard
 */

// affichage des info
void info(const char * info_msg)
{
	printf("%s: %s\n", INFO, info_msg);
}

// affichage des message de débuggage si le mode est activé
void debugm(const char * debug_msg)
{
	printf("%s: %s\n", DEBUG_MSG, debug_msg);
}

// affichage des warnings
void warm(const char * war_msg)
{
	printf("%s: %s\n", WAR, war_msg);
}

// affichage des echecs
void errorm(const char * error_msg)
{
	printf("%s: %s  %s\n", ERROR, error_msg, strerror(errno));
}

// affichage des echecs
void echecm(const char * echec_msg)
{
	printf("\n%s: %s\n", ECHEC, echec_msg);
}

// affichage en vert
void greenm(const char * green_msg)
{
	printf("%s%s%s\n", GREEN, green_msg, NORM);
}


//fonction pour le message de bienvenue et pour activer le mode debug
void welcome_message(int argc, char * argv[])
{
	int index = 0;

	printf("\n\n\n\t----------------------------------------");
	printf("\n\tBALBIANI.L		 TEIKITUHAAHAA.M\n");
	printf("\n\tUPSSITECH		            STRI\n");
	printf("\t----------------------------------------\n");
	
	greenm("\n\tBienvenue sur Cbay, Application Serveur!\n\n");
	
	if(argc > 1)
		{
			fprintf(stdout, "[WAR]: Arguments in the launching command...\n");
			while(index != argc)
			{
				if(strcmp(argv[index], "-debug") == 0)
					debug = DEBUG_ON; // MODE DEBUG ON
				else
					debug = DEBUG_OFF; // MODE DEBUG OFF
				fprintf(stdout, "ARGUMENT %d: %s \n", index, argv[index]);
				index++;
			}
			fprintf(stdout, "[WAR]: MODE DEBUG ON\n");
			index = 0;
		}
}

// Cette fonction sert à nettoyer les buffer
void clean_b(char * buffer)
{
	int index=0;
	while(index < 1024)
	{
		buffer[index]='\0';
		index++;
	}
}

// fonction de chargement des paramètres à partir du fichier de configuration du serveur
void load_server(struct server_t * server)
{

	FILE * config_file;
	char buffer[256];
	char arg1[128];
	char arg2[128]; arg2[0] = '\0';

	
	debugm("fopen: Fichier de configuration");
	config_file = fopen("./conf/server.conf", "r"); // ouverture du fichier de configuration du serveur
	if(config_file == NULL)
	{
		errorm("Ouverture du fichier de configuration impossible !");
		exit(FAIL);
	}
	debugm("fopen: OK");	
	debugm("Lecture en cours");
	while(fgets(buffer, sizeof(buffer), config_file) != NULL) //LECTURE LIGNE PAR LIGNE DU FICHIER DE CONF
	{
		debugm(buffer); // affichage de la ligne
		if(buffer[0] == '#') //LIGNE DE COMMENTAIRE
		{
			fprintf(stdout,"Ligne de commentaire: %s", buffer + 1);
		}
		else
		{
			if(sscanf(buffer, "%s %s\n", arg1, arg2) == 2) //DECODAGE DE LA LIGNE
			{
				
				if(strcmp(arg1, "server_name") == 0) // ligne du nom du serveur
				{
					strcpy(server->server_name, arg2);
				}
				else if(strcmp(arg1, "port_number") == 0) 	// ligne du numéro de port
				{
					server->port_number = atoi(arg2);
				}
				else if(strcmp(arg1, "auth_file") == 0) 	// fichier d'authentification
				{
					strcpy(auth_file, arg2);
					server->auth_file = fopen(auth_file, "a+");
					if(server->auth_file == NULL)
					{
						errorm("fopen  (auth_file):");
						exit(FAIL);
					}
					else
						debugm("fopen (auth_file): OK");
				}	
				else if(strcmp(arg1, "object_file") == 0)	// fichier des objets
				{
					strcpy(object_file, arg2);
					server->object_file = fopen(object_file, "a+");
					if(server->auth_file == NULL)
					{
						errorm("fopen (object_file)");
						exit(FAIL);
					}
					else
						debugm("fopen (object_file): OK"); //DEBUG
				}
				else if(strcmp(arg1, "log_file") == 0) 	    // fichier des logs et des ventes
				{
					server->log_file = fopen(arg2, "a+");
					if(server->auth_file == NULL)
					{
						errorm("fopen (log_file)");
						exit(FAIL);
					}
					else
						debugm("fopen (log_file): OK"); //DEBUG
					
				}
				else
				{
					warm("Ligne inconnu dans le fichier de configuration");
					exit(FAIL);
				}
			}	
		}	
	}
	
	fclose(config_file);
	debugm("fclose (config_file): OK");
}

// fonction pour l'affichage des paramètres du server
void screen_server(struct server_t server)
{
	fprintf(stdout, "|--------------------PARAMETRES----------------------|\n");
	fprintf(stdout, " Nom du serveur: %s;\n", server.server_name); 
	fprintf(stdout, " Numéro de port: %d;\n", server.port_number);
	fprintf(stdout, " Fichier d'authentification: %p;\n", server.auth_file);
	fprintf(stdout, " Fichier des objets aux enchère: %p;\n", server.object_file);
	fprintf(stdout, " Fichier des logs des achats et des ventes: %p;\n", server.log_file);
	fprintf(stdout, "|----------------------------------------------------|\n\n\n");

}

// fonction pour l'initilisation des paramètres de connexion résaux.
void init_socket(int * socklis, struct sockaddr_in * nom_server, struct server_t * server)
{
	info("Chargement des connexions");
	/* construction du socket local */
	debugm("Construction du socket local"); //DEBUG
	if((*socklis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) /* (IPv4 ,flux de donnée binaire (en mode connecte), protocole TCP) */
	{
		errorm("Socket():\nExiting...\n");
		exit(FAIL); //
	}
	
	if(debug) fprintf(stdout, "[DEBUG]: Construction réussi: sock = %d\n", *socklis); //DEBUG

	/* Association nom interne - nom externe socket local */
	nom_server->sin_family = AF_INET;
	nom_server->sin_port = server->port_number;
	nom_server->sin_addr.s_addr = INADDR_ANY;
	
	debugm("bind ...");
	if(bind(*socklis, (struct sockaddr *)nom_server, sizeof(*nom_server)) == -1)
	{
		errorm("bind()\nExiting...\n");
		exit(FAIL);
	}
	debugm("bind() OK");
  	 
}

// fonction pour la réception des données
int rcv_socket(struct user_t * client, char * buffer)
{
	size_t size_recv;
	
	clean_b(buffer);

	do
	{
		time(&server_time);
		usleep(10000);
		size_recv = recv(client->socket_fd, buffer, 1024, MSG_DONTWAIT);
		
		if(size_recv == 0) // la connexion a été fermé
		{
			warm("Connexion a été fermée par le client");
			
			info("Réinitialisation des paramètres client");
		
			client->uid = 0;
			client->login[0] = '\0';
			client->password[0] = '\0';
			client->admin = -1;
			client->mode = 'i';
			client->last_connect = 0;
			close(client->socket_fd);
			client->socket_fd = -1;
			
			return FAIL;
		}

	}while(size_recv == -1);	
	debugm("buffer:");
	debugm(buffer);	
	return SUCCESS;
}

// fonctions pour l'émession de données
bool send_socket(struct user_t * client, const char * buffer)
{
	send(client->socket_fd, buffer, strlen(buffer) + 1, MSG_DONTWAIT);
	return SUCCESS;
}

// fonction pour l'émission des messages d'erreurs
bool error_msg(struct user_t * client, const char * error_code)
{
	char buffer[20];
	
	sprintf(buffer, "ERROR = %s \n", error_code);
	printf("[REQ ERROR]: %s\n", error_code);
	send_socket(client, buffer);
	return SUCCESS;
}

// fonction pour la mise en arrêt du serveur
void shut_server(struct server_t * server)
{
	fclose(server->auth_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (auth_file) reussi\n");
	fclose(server->object_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (object_file) reussi\n");
	fclose(server->log_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (log_file) reussi\n");
	
	exit(0);

}


/* FONCTIONS DE CODAGE ET DECODAGE DES LIGNES DES FICHIERS */
// fonction pour faciliter les gestion des contenues du fichier
bool decode_user(struct user_t * client, char * ligne)
{
	int n;
	
	n = sscanf(ligne, "%ld %s %s %d %ld \n", &client->uid, client->login, client->password, &client->admin, &client->last_connect);
	if(n == 5)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
		
}

bool encode_user(struct user_t * client, char * ligne)
{ 
	int n;
	
	n = sprintf(ligne, "%ld %s %s %d %ld \n", client->uid, client->login, client->password, client->admin, client->last_connect);
	if(n == 5)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool decode_item(struct user_t * client, struct object_t * item, char * ligne)
{
	int n;
	
	n = sscanf(ligne, " %ld + %51[^+] + %51[^+] + %1024[^+] + %99[^+]+ %f + %f + %f + %d + %99[^+] + %ld + %ld + %ld \n", 
	&item->uid, 
	item->name, 
	item->category, 
	item->description, 
	item->url_image, 
	&item->start_price, 
	&item->temp_price, 
	&item->final_price,		
	&item->quantity,			
	item->place,       
	&item->vendeur,
	&item->acheteur,
	&item->expire_time);
	
	if(n == 13)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool encode_item(struct user_t * client, struct object_t * item, char * ligne)
{
	int n;
	
	n = sprintf(ligne, "%ld + %s + %s + %s + %s + %f + %f + %f + %d + %s + %ld + %ld + %ld \n", 
	item->uid, 
	item->name, 
	item->category, 
	item->description, 
	item->url_image, 
	item->start_price, 
	item->temp_price, 
	item->final_price,		
	item->quantity,			
	item->place,       
	item->vendeur,
	item->acheteur,
	item->expire_time);
	
	if(n == 13)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool decode_log(struct user_t * client, struct log_t * event, struct server_t * server, char * ligne)
{
	int n;
	
	n = sscanf(ligne, "%ld %ld %ld %c %ld\n",
		&event->item_uid,
		&event->seller_uid,	
		&event->buyer_uid,
		&event->event,
		&event->date);
		
	if(n == 5)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool encode_log(struct log_t * event, struct server_t * server)
{
	
	char ligne[256];
	
	sprintf(ligne, "%ld %ld %ld %c %ld \n",
		event->item_uid,
		event->seller_uid,	
		event->buyer_uid,
		event->event,
		event->date);
	
	fseek(server->log_file, 0, SEEK_END); // DEPALCEMENT DU CURSEUR en fin de fichier
	fputs(ligne, server->log_file);
	fseek(server->log_file, 0, SEEK_SET);
	
	return TRUE;
	
}

// cette fonction vérifie la validité des enchère.
/*
	Si l'enchère a expiré, la fonction marque le prix final comme égal au dernier enchère sur l'objet
	ou s'il n'y a pas eu d'enchère marque le vendeur comme acheteur.
*/
void expire_time(struct user_t * client, struct server_t * server)
{
	struct object_t item;
	char ligne[2048];
	char cmd[100];
	
	FILE * temp_file;
	
	
	temp_file = tmpfile();
	if(temp_file == NULL)
	{
		errorm("Erreur création fichier temporaire");
		exit(FAIL);
	}

	rewind(server->auth_file);
	rewind(temp_file);			 // RESET FILES
	
	while(fgets(ligne, sizeof(ligne), server->object_file) != NULL) // GET EACH LINE
	{	
		decode_item(client, &item, ligne);			  // DECODE LINE
		time(&server_time);		
		if(item.expire_time <= server_time) // si l'enchère est expiré
		{
			if(item.acheteur == 0 )
				item.acheteur = item.vendeur;
			
			item.final_price = item.temp_price;
			encode_item(client, &item, ligne);
			fputs(ligne, temp_file);
		}
		else
			fputs(ligne, temp_file);
	}

	rewind(temp_file);			 				 // RESET FILES
	rewind(server->object_file);				 // RESET FILE
	
		sprintf(cmd, "echo -n "" > %s", object_file);
		system(cmd);
	while(fgets(ligne, sizeof(ligne), temp_file) != NULL) // GET EACH LINE
	{
			fputs(ligne, server->object_file);
	}
	
	rewind(temp_file);			 				 // RESET FILES
	rewind(server->object_file);				 // RESET FILE
}