/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

//
state req_verify_login(struct user_t * client, struct server_t * server, char * buffer)
{
	char ligne[512];
	struct user_t temp_client;
	
	info("Requête de vérification du login");
	if(sscanf(buffer,"REQ_VERIFY_LOGIN = %s\n", client->login) != 1)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->auth_file, 0, SEEK_SET); // reset file
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		debugm(ligne);
		if(decode_user(&temp_client, ligne) == TRUE)
		{
			if(strcmp(client->login,temp_client.login) == 0)
			{
				info("Le login demandé par l'utilisateur n'est pas libre");
				clean_b(buffer);
				sprintf(buffer, "LOGIN_BUSY = %s\n", client->login);
				debugm(buffer);
				send_socket(client, buffer);	//-->
				fseek(server->auth_file, 0, SEEK_SET); // reset file
				return LOGIN_BUSY;
			}
		}
		else
		{
			errorm("Erreur lors du décodage de la ligne");
			error_msg(client, "0x999");
			fseek(server->auth_file, 0, SEEK_SET); // reset file
			return FAIL;
		}
	}
	
	fseek(server->auth_file, 0, SEEK_SET); // reset file
	info("Le login demandé par l'utilisateur est libre");
	clean_b(buffer);
	sprintf(buffer, "LOGIN_FREE = %s \n", client->login);
	send_socket(client, buffer);
	debugm(buffer);
	
	return LOGIN_FREE; // OK
}

//
state req_new_user(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[1024];	
	
	info("Requête d'ajout d'un utilisateur");
	if(sscanf(buffer,"REQ_NEW_USER = %s + %s \n", client->login, client->password) != 2)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	
	clean_b(buffer);
	
	time(&client->uid);						// ATTRIBUTION UID
	client->admin = -1; 					// MODE INDERTERMINEE
	client->last_connect = 0;				// PAS DE PREMIERE CONNEXION	
	encode_user(client, ligne);	
	
	fseek(server->auth_file, 0, SEEK_END); // DEPALCEMENT DU CURSEUR en fin de fichier
	if(fprintf(server->auth_file, "%s \n",ligne) <0)
	{
		errorm("Impossible d'ajouter le nouvel utilisateur");
		error_msg(client, "0x000");	
		return FAIL;
	}
	
	log_inscription(client, server);
	fseek(server->auth_file, 0, SEEK_SET); // RESET FILE
	info("Nouvel utilisateur ajouté");
	clean_b(buffer);
	sprintf(buffer, "USER_ADDED \n");
	send_socket(client, buffer);
	return SUCCESS;
}

//
state req_connect(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[512];
	struct user_t temp_client;
	
	info("Requête de connexion");
	if(sscanf(buffer,"REQ_CONNECT = %s + %s \n", client->login, client->password) != 2)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->auth_file, 0, SEEK_SET); // RESET FILE
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		debugm(ligne);
		if(decode_user(&temp_client, ligne) == FAIL)
		{
			errorm("Erreur de récupération des termes de la ligne");
			error_msg(client, "0x010");
			return FAIL;
		}
		else
		{	
			debugm("Décodage de la ligne réussi:");
			debugm("Login");
			debugm(temp_client.login);
			debugm("Password");
			debugm(temp_client.password);

		 	if(strcmp(client->login,temp_client.login) == 0)
			{
				debugm("Login trouvé");
				if(strcmp(client->password, temp_client.password) == 0)
				{
					debugm("Password OK");
				/***********************************************************/
					client->uid = temp_client.uid;
					time(&(client->last_connect));
					sprintf(buffer, "USER_CONNECTED = %ld \n", client->uid);
					send_socket(client, buffer);
					clean_b(buffer);
				/***********************************************************/
					log_connect(client, server);					
					info("La requête s'est terminée avec succès !"); 
					printf("Utilisateur connecté: %s\n", client->login);
					return CONNECT_SUCCESS;
				}
				else
				{
					error_msg(client, "0x012");
					return CONNECT_FAIL;
				}
			}
		}
	}
	
	info("Connection non-aboutie");
	error_msg(client, "0x011");
	
	return CONNECT_FAIL; //PAS DE LOGIN TROUVEE
	
}

//
state req_mode(struct user_t * client, struct server_t * server, char * buffer)
{
	
	char mode[10];
	unique_id_t client_uid;
	
	info("Requête du mode d'utilisateur");
	if(sscanf(buffer,"REQ_MODE = %s FOR %ld \n", mode, &client_uid) != 2);
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999"); 
		close(client->socket_fd);
		return FAIL;
	}
	clean_b(buffer);
	
	if(client_uid == client->uid)
	{
		if(strcmp(mode, "ADMIN") == 0) // vérifié si la ligne est remplacé
		{
			if(client->admin == TRUE)
			{
				client->mode = 'a';
				sprintf(buffer, "MODE = %s FOR %ld \n", mode, client->uid);
				send_socket(client, buffer);
				clean_b(buffer);
				return SUCCESS;
			}
			else
			{
				warm("Un utilisateur a essayé d'obtenir le mode utilisateur");
				error_msg(client, "0x021");
				return FAIL;
			}
		}
		else if(strcmp(mode, "BUY") == 0)
		{
			client->mode = 'b';
			sprintf(buffer, "MODE = %s FOR %ld \n", mode, client->uid);
					send_socket(client, buffer);
				clean_b(buffer);
				return SUCCESS;
		}
		else if(strcmp(mode, "SELL") == 0)
		{
			client->mode = 's';
			sprintf(buffer, "MODE = %s FOR %ld \n", mode, client->uid);
					send_socket(client, buffer);
				clean_b(buffer);
				return SUCCESS;
		}
		else
		{
			error_msg(client, "0x022");
			return FAIL;
		}
	}
	else
	{
		error_msg(client, "0x025");
		return FAIL;
	}
}


state req_new_pw(struct user_t * client, struct server_t * server, char * buffer)
{
	
	char new_pw[20];
	char client_login[20];
	char ligne[128];
	unique_id_t client_uid;	
	struct user_t temp_client;
	
	
	if(debug)printf("[DEBUG] = req_new_pw\n");
	
	if(sscanf(buffer,"REQ_NEW_PW = %s ON %s BY %ld \n", new_pw, client_login, &client_uid) != 3);
	{
		printf("%s %s %ld\n", new_pw, client_login, client_uid);
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	if(debug)printf("[DEBUG]: Cleanning buffer\n");
	clean_b(buffer);	
	if(debug)printf("[DEBUG]: Extraction de la requête réussi");
	
	if(client_uid == client->uid && client->admin == TRUE) // vérification user == admin
	{
		while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
		{
			if(decode_user(&temp_client, ligne) == FAIL)
			{
				fprintf(stderr,"[ERROR]: Erreur modification de mot de passe: %s\n", strerror(errno));
				error_msg(client, "0x999");
				return FAIL;
			}
			if(strcmp(temp_client.login, client_login) == 0)
			{
				if(temp_client.admin != TRUE)
				{
					if(temp_client.password != new_pw)
					{
						strcpy(temp_client.password, new_pw);
						fseek(server->auth_file, -(strlen(ligne)),SEEK_CUR);
						encode_user(&temp_client, ligne);
						fprintf(server->auth_file,"%s \n", ligne);
						/***************************************************/
						sprintf(buffer, "PASSWORD_CHANGED \n");
						send_socket(client, buffer);
						clean_b(buffer);
						return SUCCESS;
					}
					else
					{
						error_msg(client, "0x044");
						return FAIL;
					}
				}
				else
				{
					error_msg(client, "0x043");
					return FAIL;
				}
			}
			else
			{
			}				
		}
		error_msg(client, "0x042");
	}
	else
	{
		if(debug)printf("[DEBUG]:User is not an admin, sending error...\n");
		error_msg(client, "0x041");
		return FAIL;
	}

	return FAIL;
}

state req_del_user(struct user_t * client, struct server_t * server, char * buffer)
{
	unique_id_t client_uid;
	char client_del_login[20];
	char ligne[128];
	size_t size_l = 0;
	
	fd_t fd;
	
	if(debug)printf("[DEBUG] = req_del_user\n");
	if(debug)printf("[DEBUG] = buffer: %s \n", buffer);
	
	if(sscanf(buffer,"REQ_DEL_USER = %s BY %ld \n", client_del_login, &client_uid) != 2)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	if(debug)printf("[DEBUG]: Cleanning buffer\n");
	clean_b(buffer);
	
	if(debug)printf("[DEBUG]: Extraction de la requête réussi\n");
	
	if(debug)printf("[DEBUG]:Création du fichier virtuelle (pipe)\n");
	if(pipe(&fd) == -1)
	{
		fprintf(stderr,"[ERROR]: Erreur creation d'un pipe (req_del_user): %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	if(debug)printf("[DEBUG]:PIPE OK\n");
		
	if(client_uid == client->uid && client->admin == TRUE)
	{
		if(debug)printf("[DEBUG]: Lecture ligne pas ligne du fichier\n");
		while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
		{
			if(strcmp(client_del_login, ligne) <= 0) // SI CE N'EST PAS LA LIGNE A IGNOREE
			{
				size_l = strlen(ligne);
				write(fd, ligne, size_l);
			}
			else
			{
				// NE RIEN FAIRE
			}
		}
		
		// RECOPIE DU NOUVEAU FICHIER
		system("echo > auth_file.bin");
		lseek(fd, 0, SEEK_SET);
		while(read(fd, ligne, size_l) != 0)
		{
			fputs(ligne, server->auth_file);
		}		
	}
	else
	{
		if(debug)printf("[DEBUG]:User is not an admin, sending error...\n");
		error_msg(client, "0x042");
		return FAIL;
	}
	return SUCCESS;
}

state req_all_item(struct user_t * client, struct server_t * server, char * buffer)
{
	
	struct object_t temp_item;
	unique_id_t client_uid;
	char ligne[256];
	
	
	if(sscanf(buffer,"REQ_DEL_USER BY %ld \n", &client_uid) != 1)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	if(client_uid == client->uid) 							 // IF ADMIN
	{
		fseek(server->object_file, 0, SEEK_SET);
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			decode_item(client, &temp_item, ligne);			 // DECODE LINE
			
			sprintf(buffer, "ITEM = %ld %s %s %f %f %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.temp_price,
				temp_item.quantity);						 // FORMAT
			
			send_socket(client, buffer);					 // SEND
			
			clean_b(buffer);								 // CLEAN BUFFER
			
		}
	}
	else													 // IF NOT ADMIN
	{
		error_msg(client, "0x042");
		return FAIL;
	}
	
	sprintf(buffer, "END_ITEM \n");
	send_socket(client, buffer);					 		 // SEND
	clean_b(buffer);								 	 	 // CLEAN BUFFER
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	return SUCCESS;
}

/**/
state req_op(struct user_t * client, struct server_t * server, char * buffer)
{ // LOG A FAIRE
	char op[10];
	char cmd[100];
	char ligne[2048];
	struct object_t temp_item;
	unique_id_t item_uid;
	FILE * temp_file;
	
	
	if(sscanf(buffer,"REQ_OP = %s ON %ld \n", op, &item_uid) != 2)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	temp_file = fopen("./temp_file.bin", "w+");
	if(temp_file  == NULL)
	{
		errorm("Erreur lors de l'ouverture du fichier temporaire (req_del_user)");
		error_msg(client, "0x999");
		return FAIL;
	}
	
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	
	while(fgets(ligne, sizeof(ligne), server->object_file) != NULL) 	 // GET ITEM LINE
	{
		decode_item(client, &temp_item, ligne);			 	 // DECODE LINE
		if(temp_item.uid == item_uid)					 	 // IF ITEM FOUND
		{
			if(strcmp(op, "DELETE_BID") ==0)				 	 // IF DELETE BID DO NOTHING
			{
				info("Requête de suppression d'un enchère");
			}
			else if(strcmp(op, "CANCEL_BID") ==0)			 	 // IF RESET BID  DELETE TEMP_PRICE
			{	
				info("Requête de réinitialisation de l'enchère");
				temp_item.temp_price = 0;
				encode_item(client, &temp_item, ligne);					
				fputs(ligne, temp_file);
			}
			else										 	 // IF ELSE
			{
				//	NOTHING
			}
		}
		else
		{
			debugm(ligne);
			fputs(ligne, temp_file);
		}
	}
	
	debugm("copie terminée\n refopen");
	
	sprintf(cmd, "echo -n "" > %s", object_file);
	system(cmd);
	
	rewind(server->object_file);
	rewind(temp_file);

	while(fgets(ligne, sizeof(ligne), temp_file))
	{ 
		if(fputs(ligne, server->object_file) == EOF)
		{
			warm("Erreur lors de la recopie de temp_file vers object_file");
			error_msg(client, "0x999");
			return FAIL;
		}
	}

	fclose(temp_file);
	remove("./temp_file.bin");
	clean_b(buffer);	
	sprintf(buffer, "OP_OK = %s FOR %ld BY %s \n", op, item_uid, client->login);
	debugm(buffer);
	send_socket(client, buffer);
	info("la requête s'est terminée avec succès");
	return SUCCESS;
	
}

state req_item_sold(struct user_t * client, struct server_t * server, char * buffer)
{
	struct object_t temp_item;
	char user[20];
	char ligne[256];
	
	info("Requête d'affichage des objets vendu");
	if(sscanf(buffer,"REQ_ITEM_SOLD = %s \n", user) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);
	
	if(strcmp(user,client->login) == 0)						  // CHECKING USERNAME
	{
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			decode_item(client, &temp_item, ligne);			  // DECODE LINE
			
			if((temp_item.vendeur == client->uid) && (temp_item.final_price > 0))			  // IF CLIENT -> ITEM
			{
				sprintf(buffer, "ITEM = %ld %s %s %f %f %f %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.temp_price,
				temp_item.final_price,
				temp_item.quantity);						 // FORMAT
			
			send_socket(client, buffer);					 // SEND			
			clean_b(buffer);								 // CLEAN BUFFER
			
			}
		}
		
		sprintf(buffer, "END_ITEM \n");
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
		return SUCCESS;
	}
	
	error_msg(client, "0x062");
	warm("Impossible de répondre à la requête !");
	return FAIL;
	
}

//
state req_item_user(struct user_t * client, struct server_t * server, char * buffer)
{
	
	struct object_t temp_item;
	unique_id_t user_uid;
	char ligne[256];
	
	info("Requête d'affichage des ventes en cours reçu");
	if(sscanf(buffer,"REQ_ITEM_USER = %ld \n", &user_uid) != 1)
	{
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);
	
	if(user_uid == client->uid)						  // CHECKING USERNAME
	{
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			if(decode_item(client, &temp_item, ligne) == FAIL)			  // DECODE LINE
			{
				debugm("décodage de la ligne échec");
				echecm("Impossible de répondre correctement à la requête");
				error_msg(client, "0x999");
				return FAIL;
			}
			
			if((temp_item.vendeur == client->uid) && ((temp_item.final_price == 0)	&&	(temp_item.temp_price >= 0)))  // si l'objet appartient au client connecté ET qu'il n'a pas été marqué comme vendu
			{
				// formatage de la réponse du serveur
				sprintf(buffer, "ITEM = %ld + %s + %s + %f + %f + %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.temp_price,
				temp_item.quantity);						 // FORMAT
			
			debugm(buffer);
			usleep(10000);
			send_socket(client, buffer);					 // SEND			
			clean_b(buffer);								 // CLEAN BUFFER
			
			}
		}
		
		usleep(10000);
		sprintf(buffer, "END_ITEM \n");
		debugm(buffer);
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
		info("La requête s'est terminé avec succès !");
		return SUCCESS;
	}
	
	warm("Chargement des enchères impossible");
	error_msg(client, "0x062");
	return FAIL;
}

//
state put_new_item(struct user_t * client, struct server_t * server, char * buffer)
{

	char ligne[256];
	struct object_t temp_item;

	info("Requête d'ajout dun nouvel objet au vente");
	if(sscanf(buffer,"PUT_NEW_ITEM = %51[A-Za-z0123456789 ] + %51[A-Za-z ] + %1024[A-Za-z0123456789. ] + %99[A-Za-z0123456789:// ] + %f + %d + %99[A-Za-z0123456789 ]\n",
		temp_item.name, 
		temp_item.category, 
		temp_item.description, 
		temp_item.url_image, 
		&temp_item.start_price, 	
		&temp_item.quantity,			
		temp_item.place) != 7)
	{	
		errorm("Impossible d'extraire la requête");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	time(&temp_item.uid);
	temp_item.uid = temp_item.uid - 123456789; 
	temp_item.temp_price = 0; 
	temp_item.final_price = 0;
	temp_item.vendeur = client->uid;
	temp_item.acheteur = 0;
	
	encode_item(client, &temp_item, ligne); 	// encodage en chaine de caractères pour être placé dans le fichier
	fseek(server->object_file, 0, SEEK_END);	// on se place à la fin du fichier
	if(fputs(ligne, server->object_file) == EOF)
	{
		errorm("Impossible d'ajouter un nouvel item");
		error_msg(client, "0x064");
		return FAIL;
	}
	
	send_socket(client, "ITEM_ADDED \n");
	fseek(server->object_file, 0, SEEK_SET);	// on se replace en début de fichier
	
	info("La requête s'est terminée avec succès");
	return SUCCESS;
}



state req_hist_item_bought(struct user_t * client, struct server_t * server, char * buffer)
{
	struct object_t temp_item;
	unique_id_t user_uid;
	char ligne[256];
	
	
	if(sscanf(buffer,"REQ_HIST_ITEM_BOUGHT BY %ld \n", &user_uid) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);
	
	if(user_uid == client->uid)						  // CHECKING USERNAME
	{
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			decode_item(client, &temp_item, ligne);			  // DECODE LINE
			
			if((temp_item.acheteur == client->uid)&&(temp_item.final_price > 0))  // IF CLIENT -> ITEM
			{
				sprintf(buffer, "ITEM = %ld %s %s %f %f %f %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.temp_price,
				temp_item.final_price,
				temp_item.quantity);						 // FORMAT
			
			send_socket(client, buffer);					 // SEND			
			clean_b(buffer);								 // CLEAN BUFFER
			
			}
		}
		
		sprintf(buffer, "END_ITEM \n");
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
		return SUCCESS;
	}
	
	error_msg(client, "0x062");
	return FAIL;
}

///////////////////******************///////////////
state req_bid_user(struct user_t * client, struct server_t * server, char * buffer)
{
	clean_b(buffer);
	return SUCCESS;
}
state req_cat(struct user_t * client, struct server_t * server, char * buffer)
{
	int tabex = 0;
	int index = 0;
	bool egal = FALSE;
	struct object_t temp_item;
	char ligne[256];
	char * category[10]; 
	
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);	
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{	
		egal = FALSE;
		decode_item(client, &temp_item, ligne);			  // DECODE LINE
		while(index < tabex)
		{ // il faut que ca soit pareil qu'une seule fois pour sortir
			if(strcmp(category[index],temp_item.category) == 0)
				egal = TRUE;
				break;
			index++;
		}
		if(egal == FALSE)
		{
			strcpy(category[tabex], temp_item.category);
			tabex++;
		}
		index = 0;
	}
	
	// ENVOI DES CATEGORIES
	
	index = 0;
	while((index <= tabex) && ((strlen(category[index]) > 0)))
	{
		sprintf(buffer, "CATEGORY = %s \n", category[index]);
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		index++;
	}
	
	sprintf(buffer, "END_CATEGORY \n");
	
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	return SUCCESS;

/*	
	error_msg(client, "0x062");
	return FAIL;
*/
}


state req_cat_access(struct user_t * client, struct server_t * server, char * buffer)
 {
 
	char category[50];
	char ligne[256];
	struct object_t temp_item;
	
	if(sscanf(buffer,"REQ_CAT_ACCESS = %s \n", category) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{	
		decode_item(client, &temp_item, ligne);			  // DECODE LINE
		
		if(strcmp(temp_item.category, category) == 0)
		{
		
			sprintf(buffer, "ITEM = %ld %s %s %f %f %f %d \n" , 
			temp_item.uid,
			temp_item.name,
			temp_item.category,
			temp_item.start_price,
			temp_item.temp_price,
			temp_item.final_price,
			temp_item.quantity);						 	 // FORMAT
		
			send_socket(client, buffer);					 // SEND			
			clean_b(buffer);								 // CLEAN BUFFER
		}
		else
		{
			// IGNRORE
		}
	}
	
	sprintf(buffer, "END_ITEM \n");
	send_socket(client, buffer);					 		 // SEND
	clean_b(buffer);								 	 	 // CLEAN BUFFER
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	return SUCCESS;
 }
 
 
state req_item(struct user_t * client, struct server_t * server, char * buffer)
 {
	unique_id_t item_uid = 0;
	char ligne[256];
	struct object_t temp_item;
	
	
	if(sscanf(buffer,"REQ_ITEM = %ld \n", &item_uid) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	fseek(server->object_file, 0, SEEK_SET);
	
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{
		decode_item(client, &temp_item, ligne);			  // DECODE LINE

		if(temp_item.uid == item_uid)
		{
			sprintf(buffer, "ITEM = %s + %s + %s + %s + %f + %d + %s \n" , 
			temp_item.name,
			temp_item.category,
			temp_item.description,
			temp_item.url_image,
			temp_item.start_price,
			temp_item.quantity,
			temp_item.place);						 	 	  // FORMAT
		
			send_socket(client, buffer);					  // SEND			
			clean_b(buffer);
			return SUCCESS;
			
		}
				
	}
	
	error_msg(client, "0x070");
	return FAIL;
 }
 
 
state req_bid_price(struct user_t * client, struct server_t * server, char * buffer)
 {
	float item_new_price = 0;
	unique_id_t item_uid = 0;
	
	char ligne[256];
	struct object_t temp_item;

	
	
	if(sscanf(buffer,"REQ_BID_PRICE = %f FOR %ld\n", &item_new_price, &item_uid) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->object_file, 0, SEEK_SET);
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{
		decode_item(client, &temp_item, ligne);			  // DECODE LINE
		
		if(temp_item.uid == item_uid)
		{
			if(item_new_price <= temp_item.temp_price)
			{
				error_msg(client, "0x075");
				return FAIL;
			}
			else
			{
				temp_item.temp_price = item_new_price;
				
				encode_item(client, &temp_item, ligne);
				if(fseek(server->object_file, - strlen(ligne), SEEK_CUR) != 0)
				{
					fprintf(stderr,"[ERROR]: Impossible de déplacer le curseur dans object_file: %s\n", strerror(errno));
					error_msg(client, "0x999");
					return FAIL;
				}
				if(fputs(ligne, server->object_file) == EOF)
				{
					fprintf(stderr,"[ERROR]: Impossible d'écrire la ligne dans object_file: %s\n", strerror(errno));
					error_msg(client, "0x999");
					return FAIL;
				}
				break;
			}		
		}
		else
		{
			// CONTINUE
		}
	}
	
	fseek(server->object_file, 0, SEEK_SET);	
	return SUCCESS;
 }
