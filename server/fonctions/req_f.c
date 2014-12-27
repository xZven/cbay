/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */


state req_verify_login(struct user_t * client, struct server_t * server, char * buffer)
{

	char ligne[512];
	struct user_t temp_chck;
	
	fseek(server->auth_file, 0, SEEK_SET); // deplacement du curseur en d�but de fichier.
	
	sscanf(buffer,"REQ_VERIFY_LOGIN = %s\n", client->login);
	clean_b(buffer);
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		if(sscanf(ligne, "%ld %s", &temp_chck.uid, temp_chck.login) != 2)
		{
			fprintf(stderr,"[ERROR]: Erreur lors de la v�rification du login: %s", strerror(errno) );
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
	
	time(&client->uid);						// ATTRIBUTION UID
	sscanf(buffer,"REQ_NEW_USER = %s + %s \n", client->login, client->password);
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
	fseek(server->auth_file, 0, SEEK_SET); // deplacement du curseur en debut de fichier
	return SUCCESS;
}

state req_connect(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[512];
	struct user_t temp_chck;

	sscanf(buffer,"REQ_CONNECT = %s + %s \n", client->login, client->password);
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
	
	sscanf(buffer,"REQ_MODE = %s FOR %ld \n", mode, &client_uid);
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
	
	sscanf(buffer,"REQ_NEW_PW = %s ON %s BY %ld \n", new_pw, client_login, &client_uid);
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
	char new_pw[20];
	unique_id_t client_uid;
	char client_del_login[20];
	char ligne[128];
	
	sscanf(buffer,"REQ_DEL_USER = %s BY %ld \n", client_del_login, &client_uid);
	clean_b(buffer);
	
	if(client_uid == client_uid && client->admin == TRUE)
	{
		while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
		{
			if(decode_user(&temp_client, ligne) == FAIL)
			{
				fprintf(stderr,"[ERROR]: Erreur modification de mot de passe: %s\n", strerror(errno));
				error_msg(client, "0x999");
				return FAIL;
			}
		}
	}
	else
	{
	]
}

/*
bool req_all_item(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_op(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_item_sold(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_item_user(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_hist_item_bought(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_bid_user(struct user_t * client, struct server_t * server, char * buffer)
{
}

bool req_cat(struct user_t * client, struct server_t * server, char * buffer)
{
}

 bool req_cat_access(struct user_t * client, struct server_t * server, char * buffer)
 {
 }
 
 bool req_tiem(struct user_t * client, struct server_t * server, char * buffer)
 {
 
 }
 bool req_bid_price(struct user_t * client, struct server_t * server, char * buffer)
 {
 
 }
*/