/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

/*
 * Cette fonction sert � afficher un message d'acceuil et � 
 * activer ou d�sactiver le mode DEBUG.
 */
void welcome_message(int argc, char * argv[])
{
	int index = 0;

	printf("\n\n\n\t----------------------------------------");
	printf("\n\tBALBIANI.L		 TEIKITUHAAHAA.M\n");
	printf("\n\tUPSSITECH		            STRI\n");
	printf("\t----------------------------------------\n");
	
	printf("\n\tBienvenue sur Cbay, Application serveur!\n\n");
	
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

/*
 * Cette fonction sert � nettoyer les buffer
 */
void clean_b(char * buffer)
{
	buffer[0]='\0';
}

/*
 * Cette fonction sert � charger les param�tres du serveur
 * � partir du fichier de configuration.
 */
 /* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
void load_server(struct server_t * server)
{
	int index = 1; // affichage du num�ro de ligne
	errno = 0;
	FILE * config_file;
	char buffer[256];
	char arg1[128];
	char arg2[128]; arg2[0] = '\0';

	
	if(debug) fprintf(stdout, "[DEBUG]: fopen: Fichier de configuration\n"); //DEBUG
	config_file = fopen("./conf/server.conf", "r"); // ouverture du fichier de configuration du serveur
	if(config_file == NULL)
	{
		fprintf(stderr, "[ERROR]: %s\n\nExiting...\n", strerror(errno));
		exit(-1);
	}
	if(debug) fprintf(stdout, "[DEBUG]: fopen reussi\n"); //DEBUG	
	if(debug) fprintf(stdout, "[DEBUG]: Lecture du fichier lgine par ligne...\n\n");	//DEBUG
	while(fgets(buffer, sizeof(buffer), config_file) != NULL) //LECTURE LIGNE PAR LIGNE DU FICHIER DE CONF
	{
		if(debug) fprintf(stdout, "[DEBUG]: Ligne %d: %s\n", index, buffer); //DEBUG
		if(buffer[0] == '#') //LIGNE DE COMMENTAIRE
			{
				if(debug) fprintf(stdout, "[DEBUG]: Ligne de commentaire: %s", buffer + 1); //DEBUG
			}
		
		if(sscanf(buffer, "%s %s\n", arg1, arg2) == 2) //DECODAGE DE LA LIGNE
		{
			if(debug) fprintf(stdout, "[DEBUG]: sscanf OK\n"); //DEBUG	
			
			if(strcmp(arg1, "server_name") == 0) // ligne du nom du serveur
			{
				strcpy(server->server_name, arg2);
			}
			else if(strcmp(arg1, "port_number") == 0) // ligne du num�ro de port
			{
				server->port_number = atoi(arg2);
			}
			else if(strcmp(arg1, "auth_file") == 0) // fichier d'authentification
			{
				server->auth_file = fopen(arg2, "a+");
				if(server->auth_file == NULL)
				{
					fprintf(stderr, "[ERROR]: fopen  (auth_file) erreur: %s\nExiting...\n", strerror(errno));
					exit(-1);
				}
				else
					if(debug) fprintf(stdout, "[DEBUG]: fopen (auth_file) reussi\n"); //DEBUG
			}
			else if(strcmp(arg1, "object_file") == 0) // fichier des objets
			{
				server->object_file = fopen(arg2, "a+");
				if(server->auth_file == NULL)
				{
					fprintf(stderr, "[ERROR]: fopen (object_file) erreur: %s\nExiting...\n", strerror(errno));
					exit(-1);
				}
				else
					if(debug) fprintf(stdout, "[DEBUG]: fopen (object_file) reussi\n"); //DEBUG
			}
			else if(strcmp(arg1, "log_file") == 0) // fichier des logs et des ventes
			{
				server->log_file = fopen(arg2, "a+");
				if(server->auth_file == NULL)
				{
					fprintf(stderr, "[ERROR]: fopen (log_file) erreur: %s\nExiting...\n", strerror(errno));
					exit(-1);
				}
				else
					if(debug) fprintf(stdout, "[DEBUG]: fopen (log_file) reussi\n"); //DEBUG
				
			}
			else
			{
				//ERROR
			}
			index++;
		}
		
		
	}
	fclose(config_file);
	if(debug) fprintf(stdout, "[DEBUG]: fclose (config_file) reussi\n");
}

/*
 * Cette fonction sert � afficher les param�tres du server
 */
void screen_server(struct server_t server)
{
	fprintf(stdout, "|--------------------PARAMETRES----------------------|\n");
	fprintf(stdout, " Nom du serveur: %s;\n", server.server_name); 
	fprintf(stdout, " Num�ro de port: %d;\n", server.port_number);
	fprintf(stdout, " Fichier d'authentification: %p;\n", server.auth_file);
	fprintf(stdout, " Fichier des objets aux ench�re: %p;\n", server.object_file);
	fprintf(stdout, " Fichier des logs des achats et des ventes: %p;\n", server.log_file);
	fprintf(stdout, "|----------------------------------------------------|\n\n\n");

}

/*
 * Cette fonction charge les connexions r�seaux.
 */
void init_socket(int * socklis, struct sockaddr_in * nom_server, struct server_t * server)
{
	fprintf(stdout, "[INFOS]: CHARGEMENT: connexion ...\n");
	/* construction du socket local */
	if(debug) fprintf(stdout, "[DEBUG]: Construction du socket local\n"); //DEBUG
	if((*socklis = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) /* (IPv4 ,flux de donn�e binaire (en mode connecte), protocole TCP) */
	{
		fprintf(stderr, "[ERROR]: Socket(): %s\nExiting...\n", strerror(errno));
		exit(-1); //
	}
	if(debug) fprintf(stdout, "[DEBUG]: Construction r�ussi: sock = %d\n", *socklis); //DEBUG

	/* Association nom interne - nom externe socket local */
	nom_server->sin_family = AF_INET;
	nom_server->sin_port = server->port_number;
	nom_server->sin_addr.s_addr = INADDR_ANY;
	
	if(debug) fprintf(stdout, "[DEBUG]: bind ...\n");
	if(bind(*socklis, (struct sockaddr *)nom_server, sizeof(*nom_server)) == -1)
	{
		fprintf(stderr,"[ERROR]: bind(): %s\nExiting...\n", strerror(errno));
		exit(-1);
	}
	if(debug) fprintf(stdout, "[DEBUG]: bind() OK\n");
  	 
}

int rcv_socket(struct user_t * client, char * buffer)
{
	size_t size_recv;
	do
	{
		size_recv = recv(client->socket_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
		usleep(10000); //PAUSE DE 10 mS	
	}
	while(size_recv == -1);
	return SUCCESS;
}

bool send_socket(struct user_t * client, const char * buffer)
{
	send(client->socket_fd, buffer, strlen(buffer) + 1, MSG_CONFIRM);
	return SUCCESS;
}

bool error_msg(struct user_t * client, const char * error_code)
{
	char buffer[20];
	
	sprintf(buffer, "ERROR = %s\n", error_code);
	printf("%s \n", error_code);
	send_socket(client, buffer);
	return SUCCESS;
}

void shut_server(struct server_t * server)
{
	fclose(server->auth_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (auth_file) reussi\n");
	fclose(server->object_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (object_file) reussi\n");
	fclose(server->log_file); if(debug) fprintf(stdout, "[DEBUG]: fclose (log_file) reussi\n");
	
	exit(0);

}

bool decode_user(struct user_t * client, char * ligne)
{
	int n;
	
	n = sscanf(ligne, "%ld %s %s %d %ld\n", &client->uid, client->login, client->password, &client->admin, &client->last_connect);
	if(n == 5)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
		
}

bool encode_user(struct user_t * client, char * ligne)
{ 
	int n;
	
	n = sprintf(ligne, "%ld %s %s %d %ld", client->uid, client->login, client->password, client->admin, client->last_connect);
	if(n == 5)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool decode_item(struct user_t * client, struct object_t * item, char * ligne)
{
	int n;
	
	n = sscanf(ligne, "%ld %s %s %s %s %f %f %f %d %s %ld %ld\n", 
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
	&item->acheteur);
	
	if(n == 12)  // OK
		return TRUE;
	else		// !OK
		return FALSE; 
}

bool encode_item(struct user_t * client, struct object_t * item, char * ligne)
{
	int n;
	
	n = sprintf(ligne, "%ld %s %s %s %s %f %f %f %d %s %ld %ld", 
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
	item->acheteur);
	
	if(n == 12)  // OK
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
	int n;
	char ligne[256];
	
	n = sprintf(ligne, "%ld %ld %ld %c %ld\n",
		event->item_uid,
		event->seller_uid,	
		event->buyer_uid,
		event->event,
		event->date);
	
	fseek(server->log_file, 0, SEEK_END); // DEPALCEMENT DU CURSEUR en fin de fichier
	fprintf(server->log_file, "%s\n",ligne);
	fseek(server->log_file, 0, SEEK_SET);
	
	return TRUE;
	
}