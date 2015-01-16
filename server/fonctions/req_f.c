/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */


state req_verify_login(struct user_t * client, struct server_t * server, char * buffer)
{

	char ligne[512];
	struct user_t temp_chck;
	
	if(sscanf(buffer,"REQ_VERIFY_LOGIN = %s\n", client->login) != 1)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	fseek(server->auth_file, 0, SEEK_SET); // deplacement du curseur en début de fichier.
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		if(sscanf(ligne, "%ld %s", &temp_chck.uid, temp_chck.login) != 2)
		{
			fprintf(stderr,"[ERROR]: Erreur lors de la vérification du login: %s", strerror(errno) );
			error_msg(client, "0x999");
			return FAIL;
		}
		else
		{			
		 	if(client->login == temp_chck.login)
			{
				strcpy(buffer, "LOGIN_BUSY\n");
				send_socket(client, buffer);	//-->
				clean_b(buffer);
				return LOGIN_BUSY;
			}
		}
	}
	
	fseek(server->auth_file, 0, SEEK_SET); // DEPALCEMENT DU CURSEUR EN DEBUT DE fichier
			strcpy(buffer, "LOGIN_FREE\n");
			send_socket(client, buffer);
			clean_b(buffer);
	return LOGIN_FREE; // OK
}

state req_new_user(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[1024];	
	
	if(sscanf(buffer,"REQ_NEW_USER = %s + %s \n", client->login, client->password) != 2)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	
	clean_b(buffer);
	
	time(&client->uid);						// ATTRIBUTION UID
	client->admin = -1; 					// MODE INDERTERMINEE
	client->last_connect = 0;				// PAS DE PREMIERE CONNEXION	
	encode_user(client, ligne);	clean_b(buffer);	
	
	fseek(server->auth_file, 0, SEEK_END); // DEPALCEMENT DU CURSEUR en fin de fichier
	if(fprintf(server->auth_file, "%s\n",ligne) <0)
	{
		fprintf(stderr,"[ERROR]: Ajout nouvel utilisateur impossible: %s\n",strerror(errno));
		error_msg(client, "0x000");	
		return FAIL;
	}
	
	log_inscription(client, server);
	fseek(server->auth_file, 0, SEEK_SET); // RESET FILE
	return SUCCESS;
}

state req_connect(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[512];
	struct user_t temp_chck;
	

	if(sscanf(buffer,"REQ_CONNECT = %s + %s \n", client->login, client->password) != 2)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	fseek(server->auth_file, 0, SEEK_SET); // deplacement du curseur en debut de fichier
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		if(sscanf(ligne, "%ld %s %s %d %ld", &temp_chck.uid, temp_chck.login, temp_chck.password, &temp_chck.admin, &temp_chck.last_connect) != 5)
		{
			fprintf(stderr,"[ERROR]: Erreur lors de la connexion d'un utilisateur %s\n", strerror(errno));
			error_msg(client, "0x010");
			return FAIL;
		}
		else
		{			
		 	if(client->login == temp_chck.login)
			{
				if(client->password == temp_chck.password)
				{
				/***********************************************************/
					client->uid = temp_chck.uid;
					time(&(client->last_connect));
					sprintf(buffer, "USER_CONNECTED = %ld \n", client->uid);
					send_socket(client, buffer);
					clean_b(buffer);
				/***********************************************************/
					log_connect(client, server);
				
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
	
	error_msg(client, "0x011");
	return CONNECT_FAIL; //PAS DE LOGIN TROUVEE
	
}

state req_mode(struct user_t * client, struct server_t * server, char * buffer)
{
	
	char mode[10];
	unique_id_t client_uid;
	
	if(sscanf(buffer,"REQ_MODE = %s FOR %ld \n", mode, &client_uid) != 2);
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	if(client_uid == client->uid)
	{
		if(strcmp(mode, "ADMIN") == 0)
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
	unique_id_t client_uid;
	char client_login[20];
	struct user_t temp_client;
	char ligne[128];
	
	if(sscanf(buffer,"REQ_NEW_PW = %s ON %s BY %ld \n", new_pw, client_login, &client_uid) != 3);
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
		if(client_uid == client->uid && client->admin == TRUE)
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
	
	if(sscanf(buffer,"REQ_DEL_USER = %s BY %ld \n", client_del_login, &client_uid) != 3)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	if(pipe(&fd) == -1)
	{
		fprintf(stderr,"[ERROR]: Erreur creation d'un pipe (req_del_user): %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
		
	if(client_uid == client_uid && client->admin == TRUE)
	{
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
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
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

state req_op(struct user_t * client, struct server_t * server, char * buffer)
{ // LOG A FAIRE
	char op[10];
	char ligne[256];
	struct object_t temp_item;
	unique_id_t item_uid;
	size_t size_l = 0;
	fd_t fd;
	
	if(sscanf(buffer,"REQ_OP = %s ON %ld \n", op, &item_uid) != 2)
	{
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	if(pipe(&fd) == -1)
	{
		fprintf(stderr,"[ERROR]: Erreur creation d'un pipe (req_del_user): %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	
	while(fgets(ligne, sizeof(ligne), server->object_file)) 	 // GET ITEM LINE
	{
		decode_item(client, &temp_item, ligne);			 	 // DECODE LINE
		if(temp_item.uid == item_uid)					 	 // IF ITEM FOUND
		{
			if(strcmp(op, "DELETE") ==0)				 	 // IF DELETE BID DO NOTHING
			{
	
			}
			else if(strcmp(op, "CANCEL") ==0)			 	 // IF RESET BID  DELETE TEMP_PRICE
			{	
				temp_item.temp_price = 0;
				encode_item(client, &temp_item, ligne);					
				size_l = strlen(ligne);
				write(fd, ligne, size_l);
			}
			else										 	 // IF ELSE
			{
				//	NOTHING
			}
		}
		size_l = strlen(ligne);
		write(fd, ligne, size_l);	
	}
	
	
	// RECOPIE DU NOUVEAU FICHIER
	system("echo > object_file.bin");						// DELETE CONTAIN
	lseek(fd, 0, SEEK_SET);
	while(read(fd, ligne, size_l) != 0)
	{
		fputs(ligne, server->object_file);
	}
	
	fseek(server->object_file, 0, SEEK_SET);				 // RESET FILE
	close(fd);
	
	return SUCCESS;
	
}

state req_item_sold(struct user_t * client, struct server_t * server, char * buffer)
{
	struct object_t temp_item;
	char user[20];
	char ligne[256];
	
	
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
	return FAIL;
	
}

state req_item_user(struct user_t * client, struct server_t * server, char * buffer)
{
	
	struct object_t temp_item;
	unique_id_t user_uid;
	char ligne[256];
	
	
	if(sscanf(buffer,"REQ_ITEM_USER = %ld \n", &user_uid) != 1)
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
			
			if((temp_item.vendeur == client->uid) && ((temp_item.final_price <= 0)	&&	(temp_item.temp_price > 0)))  // IF CLIENT -> ITEM
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

state put_new_item(struct user_t * client, struct server_t * server, char * buffer)
{

	char ligne[256];
	struct object_t temp_item;

	if(sscanf(buffer,"PUT_NEW_ITEM = %s + %s + %s + %s + %f + %d + %s",
		temp_item.name, 
		temp_item.category, 
		temp_item.description, 
		temp_item.url_image, 
		&temp_item.start_price, 	
		&temp_item.quantity,			
		temp_item.place) != 7)
	{		
		fprintf(stderr,"[ERROR]: Impossible d'extraire la requête: %s\n", strerror(errno));
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	temp_item.temp_price = -1; 
	temp_item.final_price = -1;
	time(&temp_item.vendeur);
	temp_item.acheteur = 0;
	
	encode_item(client, &temp_item, ligne);
	fseek(server->object_file, 0, SEEK_END);
	if(fputs(ligne, server->object_file) == EOF)
	{
		fprintf(stderr,"[ERROR]: Impossible d'ajouter un nouvel item: %s\n", strerror(errno));
		error_msg(client, "0x064");
		return FAIL;
	}
	
	send_socket(client, "ITEM_ADDED \n");
	fseek(server->object_file, 0, SEEK_SET);
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
