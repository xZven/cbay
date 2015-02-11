/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

/*
 * Ces fonctions porte le m�me nom que les requ�tes de la RFC 
 * du projet Cbay.
 * Ces fonctions re�oivent en param�tre:
 * struct user_t: structure regroupant les informations du client
 * connect� ou qui envoi la requ�te.
 * struct server_t: structure regroupant les param�tres du serveur
 * pour faciliter le passage de plusieurs param�tres
 *
 * Etant donn�e que ces fonctions g�re les erreurs, les �tats des fonctions
 * renvoy�s ne sont pas trait�s par le programme principale
 */

// fonction de v�rification de la disponibilit� d'un login
state req_verify_login(struct user_t * client, struct server_t * server, char * buffer)
{
	char ligne[512];
	struct user_t temp_client;
	
	info("Requ�te de v�rification du login");
	if(sscanf(buffer,"REQ_VERIFY_LOGIN = %s\n", client->login) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	rewind(server->auth_file); // reset file
	
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		debugm(ligne);
		decode_user(&temp_client, ligne);

			if(strcmp(client->login,temp_client.login) == 0)
			{
				info("Le login demand� par l'utilisateur n'est pas libre");
				clean_b(buffer);
				sprintf(buffer, "LOGIN_BUSY = %s\n", client->login);
				debugm(buffer);
				send_socket(client, buffer);	//-->
				fseek(server->auth_file, 0, SEEK_SET); // reset file
				return LOGIN_BUSY;
			}
		
	}
	
	rewind(server->auth_file); // reset file
	info("Le login demand� par l'utilisateur est libre");
	clean_b(buffer);
	sprintf(buffer, "LOGIN_FREE = %s \n", client->login);
	send_socket(client, buffer);
	debugm(buffer);
	
	return LOGIN_FREE; // OK
}

// fonction d'ajout d'un nouvel utilisateur
state req_new_user(struct user_t * client,  struct server_t * server, char * buffer)
{
	char ligne[1024];	
	
	info("Requ�te d'ajout d'un utilisateur");
	if(sscanf(buffer,"REQ_NEW_USER = %s + %s \n", client->login, client->password) != 2)
	{
		errorm("Impossible d'extraire la requ�te");
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
	info("Nouvel utilisateur ajout�");
	clean_b(buffer);
	sprintf(buffer, "USER_ADDED \n");
	send_socket(client, buffer);
	return SUCCESS;
}

// fonction pour la connexion d'un utilisateur
state req_connect(struct user_t * client,  struct server_t * server, char * buffer)
{
	char cmd[100];
	char ligne[512];
	struct user_t temp_client;
	FILE * temp_file;
	
	
	info("Requ�te de connexion");
	if(sscanf(buffer,"REQ_CONNECT = %s + %s \n", client->login, client->password) != 2)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	temp_file = tmpfile();
	if(temp_file == NULL)
	{
		errorm("Erreur cr�ation fichier temporaire");
		exit(FAIL);
	}
	
	rewind(server->auth_file); // RESET FILE
	rewind(temp_file);
	
	debugm("rechercher ligne");
	while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
	{
		debugm(ligne);
		if(decode_user(&temp_client, ligne) == FAIL)
		{
			errorm("Erreur de r�cup�ration des termes de la ligne");
			error_msg(client, "0x010");
			return FAIL;
		}
		else
		{	
		 	if(strcmp(client->login,temp_client.login) == 0)
			{
				if(strcmp(client->password, temp_client.password) == 0)
				{
				/***********************************************************/
					client->uid = temp_client.uid;
					client->admin = temp_client.admin;
					time(&(client->last_connect));
					
					
					sprintf(buffer, "USER_CONNECTED = %ld \n", client->uid);
					send_socket(client, buffer);
					clean_b(buffer);
					
					encode_user(client, ligne); debugm(ligne);
					fputs(ligne, temp_file);
					
					debugm("On recopie les autres ligne dans temp");
					while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
					{
						debugm(ligne);
						fputs(ligne, temp_file);
					}
					
					debugm("Supression du contenue du fichier auth");
					sprintf(cmd, "echo -n "" > %s", auth_file);
					system(cmd);
					
					rewind(server->auth_file);
					rewind(temp_file);
					
					debugm("Recopie du fichier temp");
					while(fgets(ligne, sizeof(ligne), temp_file) != NULL)
					{ 
						debugm(ligne);
						fputs(ligne, server->auth_file);						
					}
					
					
					rewind(server->auth_file);

				fclose(temp_file);
				/***********************************************************/
					log_connect(client, server);					
					info("La requ�te s'est termin�e avec succ�s !"); 
					printf("Utilisateur connect�: %s\n", client->login);
					return CONNECT_SUCCESS;
				}
				else // mauvais mot de passe
				{
					error_msg(client, "0x012");
					return CONNECT_FAIL;
				}
			}
			else 
				fputs(ligne, temp_file);
		}
	}
	
	info("Connection non-aboutie");
	error_msg(client, "0x011");
	
	return CONNECT_FAIL; //PAS DE LOGIN TROUVEE
	
}

// fonction pour le choix du mode
state req_mode(struct user_t * client, struct server_t * server, char * buffer)
{
	
	char mode[10];
	unique_id_t client_uid;
	
	info("Requ�te du mode d'utilisateur");
	 
	sscanf(buffer,"REQ_MODE = %s FOR %ld \n", mode, &client_uid);
/*	if(0);
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999"); 
		close(client->socket_fd); // fermeture de la connexion
		client->socket_fd = -1;
		return FAIL;
	} */
	
	clean_b(buffer);
	
	if(client_uid == client->uid) // v�rification du client connect� et de la requ�te
	{
		if(strcmp(mode, "ADMIN") == 0) // on analyse le mode demand�
		{
			if(client->admin == TRUE) // s'il a le droit au mode admin
			{
				client->mode = 'a';
				sprintf(buffer, "MODE = %s FOR %ld \n", mode, client->uid);
				send_socket(client, buffer);
				clean_b(buffer);
				return SUCCESS;
			}
			else
			{
				warm("Un utilisateur a essay� d'obtenir le mode administrateur alors qu'il n'est pas autoris�");
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

// fonction de changement de mot de passe
state req_new_pw(struct user_t * client, struct server_t * server, char * buffer)
{
	
	char new_pw[51];
	char client_login[51];
	char ligne[2048];
	char cmd[100];
	unique_id_t client_uid;	
	struct user_t temp_client;
	
	FILE * temp_file;
	
	
	info("Requ�te de changement de mot de passe");
	
	sscanf(buffer,"REQ_NEW_PW = %s ON %s BY %ld \n", new_pw, client_login, &client_uid);
	if(0)
	{
		printf("%s %s %ld\n", new_pw, client_login, client_uid);
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	temp_file = tmpfile();
	if(temp_file == NULL)
	{
		errorm("Erreur cr�ation fichier temporaire");
		exit(FAIL);
	}
	
	rewind(server->auth_file);
	rewind(temp_file);
	
	if(client_uid == client->uid && client->admin == TRUE) // v�rification user == admin
	{
		debugm("D�but de la recherche...");
		while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
		{
			debugm(ligne);
			decode_user(&temp_client, ligne);
			
			if(strcmp(temp_client.login, client_login) == 0) // utilisateur � modifi� trouv� !
			{
				debugm("Utilisateur trouv� !");
				
				if(temp_client.admin != TRUE)				 // utilisateur � modifier n'est pas un admin
				{
					debugm("L'utilisateur n'est pas un admin");
					if(temp_client.password != new_pw)		 // si le nouveau mot de passe est diff�rent de l'ancien
					{
						strcpy(temp_client.password, new_pw);
						
						
						encode_user(&temp_client, ligne); debugm(ligne);
						debugm(ligne);
						fputs(ligne, temp_file);
						
						
						sprintf(buffer, "PASSWORD_CHANGED \n");
						send_socket(client, buffer);
						clean_b(buffer);
					
						debugm("On recopie les autres ligne dans temp");
						while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
						{
							debugm(ligne);
							fputs(ligne, temp_file);
						}
						
						debugm("Supression du contenue du fichier auth");
						sprintf(cmd, "echo -n "" > %s", auth_file);
						system(cmd);
						
						rewind(server->auth_file);
						rewind(temp_file);
						
						debugm("Recopie du fichier temp");
						while(fgets(ligne, sizeof(ligne), temp_file) != NULL)
						{ 
							debugm(ligne);
							fputs(ligne, server->auth_file);						
						}
						
						
						rewind(server->auth_file);

						fclose(temp_file);					
							
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
				fputs(ligne, temp_file);
			}				
		}
		error_msg(client, "0x042");
	}
	else // si l'utilisateur n'est pas un admin ou qu'il n'a pas les droits
	{
		echecm("L'utilisateur n'est pas un administrateur");
		error_msg(client, "0x041");
		/* fermeture de la connexion */
		close(client->socket_fd);
		client->socket_fd = -1;
		
		return FAIL;
	}

	return FAIL;
}

// fonction de suppresion d'un utilisateur
state req_del_user(struct user_t * client, struct server_t * server, char * buffer)
{
	unique_id_t client_uid;
	struct user_t temp_client;
	char client_del_login[21];	
	char ligne[128];
	char cmd[100];
	
	FILE * temp_file;
	
	debugm("Requ�te de supression d'un utilisateur");	
	if(sscanf(buffer,"REQ_DEL_USER = %s BY %ld \n", client_del_login, &client_uid) != 2)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	temp_file = tmpfile();
	if(temp_file == NULL)
	{
		errorm("Erreur cr�ation fichier temporaire");
		exit(FAIL);
	}
	
	rewind(server->auth_file);
	rewind(temp_file);
		
	if(client_uid == client->uid && client->admin == TRUE)		// si le client connect� est un admin
	{
		debugm("D�but de la recherche");
		while(fgets(ligne, sizeof(ligne), server->auth_file) != NULL)
		{
			decode_user(&temp_client, ligne);
			debugm(ligne);
			if(strcmp(temp_client.login, client_del_login) == 0) // si c'est l'utilisateur � supprimer
			{
				debugm("Utilisateur � supprim� trouv� !");
				/* on ignore la ligne */
			}
			else
			{
				fputs(ligne, temp_file);
			}
		}
		
		debugm("Supression du contenue du fichier auth");
		sprintf(cmd, "echo -n "" > %s", auth_file);
		system(cmd);
						
		rewind(server->auth_file);
		rewind(temp_file);
		
		while(fgets(ligne, sizeof(ligne), temp_file) != NULL)
		{
			fputs(ligne, server->auth_file);
		}
		
		rewind(server->auth_file);
		fclose(temp_file);
	}
	else
	{
		echecm("L'utilisateur n'est pas un administrateur, sending error");
		error_msg(client, "0x042");
		return FAIL;
	}
	
	clean_b(buffer);
	sprintf(buffer, "USER_DELETED = %s BY %s \n", client_del_login, client->login);
	send_socket(client, buffer);
	return SUCCESS;
}

// fonction de requ�te des item en cours d'ench�re
state req_all_item(struct user_t * client, struct server_t * server, char * buffer)
{
	
	struct object_t temp_item;
	unique_id_t client_uid;
	char ligne[2048];
	
	
	if(sscanf(buffer,"REQ_ALL_ITEM BY %ld \n", &client_uid) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	if(client_uid == client->uid && client->admin == TRUE) 							 // IF ADMIN
	{
		rewind(server->object_file);
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			decode_item(client, &temp_item, ligne);			 // DECODE LINE
			
			sprintf(buffer, "ITEM = %ld + %s + %s + %f + %f + %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.temp_price,
				temp_item.quantity);						 // FORMAT
			usleep(10000);
			send_socket(client, buffer);					 // SEND
			
			clean_b(buffer);								 // CLEAN BUFFER
			
		}
	}
	else													 // IF NOT ADMIN
	{
		error_msg(client, "0x041");
		return FAIL;
	}
	
	usleep(10000);
	sprintf(buffer, "END_ITEM \n");
	send_socket(client, buffer);					 		 // SEND
	clean_b(buffer);								 	 	 // CLEAN BUFFER
	rewind(server->object_file);				 // RESET FILE
	return SUCCESS;
}

// fonction pour appliquer une op�ration sur un item choisit
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
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	temp_file = tmpfile();
	if(temp_file  == NULL)
	{
		errorm("Erreur lors de l'ouverture du fichier temporaire (req_del_user)");
		error_msg(client, "0x999");
		return FAIL;
	}
	
	rewind(temp_file);
	rewind(server->object_file);				 // RESET FILE
	
	while(fgets(ligne, sizeof(ligne), server->object_file) != NULL) 	 // GET EACH LINE
	{
		debugm(ligne);
		decode_item(client, &temp_item, ligne);			 	 // DECODE LINE
		if(temp_item.uid == item_uid)					 	 // IF ITEM FOUND
		{
			if(strcmp(op, "DELETE_BID") ==0)				 	 // IF DELETE BID DO NOTHING
			{
				info("Requ�te de suppression d'un ench�re");
			}
			else if(strcmp(op, "CANCEL_BID") ==0)			 	 // IF RESET BID  DELETE TEMP_PRICE
			{	
				info("Requ�te de r�initialisation de l'ench�re");
				temp_item.temp_price = 0;
				encode_item(client, &temp_item, ligne);					
				fputs(ligne, temp_file);
				 log_cancel_bid(client, &temp_item, server);
			}
			else if(strcmp(op, "END_BID") ==0)
			{
				//
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
	
	clean_b(buffer);	
	sprintf(buffer, "OP_OK = %s FOR %ld BY %s \n", op, item_uid, client->login); // formatage de la r�ponse
	debugm(buffer);
	send_socket(client, buffer);		// SEND
	
	debugm("copie termin�e");
	debugm("Supression du contenue");
	
	sprintf(cmd, "echo -n "" > %s", object_file); 
	system(cmd); 									// suppression contenue du fichier
	
	rewind(server->object_file);				// RESET FILE
	rewind(temp_file);

	while(fgets(ligne, sizeof(ligne), temp_file) != NULL)
	{ 
		debugm(ligne);
		fputs(ligne, server->object_file);
	}
	rewind(server->object_file);				// RESET FILE
	fclose(temp_file);
	
	info("la requ�te s'est termin�e avec succ�s");	
	return SUCCESS;
	
}

// fonction d'envoi des item vendu par un vendeur
state req_item_sold(struct user_t * client, struct server_t * server, char * buffer)
{
	struct object_t temp_item;
	char user[20];
	char ligne[2048];
	
	info("Requ�te d'affichage des objets vendu");
	if(sscanf(buffer,"REQ_ITEM_SOLD = %s \n", user) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	rewind(server->object_file);
	
	/* on regarde si la req�ete correspond au client connect� */
	
	if(strcmp(user,client->login) == 0)						  // CHECKING USERNAME
	{
		
		while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
		{
			decode_item(client, &temp_item, ligne);			  // DECODE LINE
			
			usleep(10000);
			/* si l'item appartient au client connect� et si son prix final est positif (vendu) */
			if((temp_item.vendeur == client->uid) && (temp_item.final_price > 0))			  // IF CLIENT -> ITEM
			{
				sprintf(buffer, "ITEM = %ld + %s + %s + %f + %f + %d \n" , 
				temp_item.uid,
				temp_item.name,
				temp_item.category,
				temp_item.start_price,
				temp_item.final_price,
				temp_item.quantity);						 // FORMAT
			debugm(buffer);
			send_socket(client, buffer);					 // SEND			
			clean_b(buffer);								 // CLEAN BUFFER
			
			}
		}
		
		usleep(10000);
		sprintf(buffer, "END_ITEM \n");
		debugm(buffer);
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		rewind(server->object_file);				 			 // RESET FILE
		return SUCCESS;
	}
	else // si ce n'est pas l'utilisateur ...
	{
		error_msg(client, "0x062");
		warm("Impossible de r�pondre � la requ�te !");
		return FAIL;
	}
	
}

// fonction d'envoi des item en cours d'ench�res
state req_item_user(struct user_t * client, struct server_t * server, char * buffer)
{
	
	struct object_t temp_item;
	unique_id_t user_uid;
	char ligne[2048];
	
	info("Requ�te d'affichage des ventes en cours");
	if(sscanf(buffer,"REQ_ITEM_USER = %ld \n", &user_uid) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	rewind(server->object_file);
	
	if(user_uid == client->uid)						  // CHECKING USERNAME
	{
		
		while(fgets(ligne, sizeof(ligne), server->object_file) != NULL) // GET EACH LINE
		{
			if(decode_item(client, &temp_item, ligne) == FAIL)			  // DECODE LINE
			{
				debugm("d�codage de la ligne �chec");
				echecm("Impossible de r�pondre correctement � la requ�te");
				error_msg(client, "0x999");
				return FAIL;
			}
			
			if((temp_item.vendeur == client->uid) && ((temp_item.final_price == 0)	&&	(temp_item.temp_price >= 0)))  // si l'objet appartient au client connect� ET qu'il n'a pas �t� marqu� comme vendu
			{
				// formatage de la r�ponse du serveur
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
		info("La requ�te s'est termin� avec succ�s !");
		return SUCCESS;
	}
	
	warm("Chargement des ench�res impossible");
	error_msg(client, "0x062");
	return FAIL;
}

// fonction d'ajout d'un nouvel item aux ench�res
state put_new_item(struct user_t * client, struct server_t * server, char * buffer)
{

	char ligne[256];
	struct object_t temp_item;

	info("Requ�te d'ajout dun nouvel objet au vente");
	if(sscanf(buffer,"PUT_NEW_ITEM = %51[^+\n] + %51[^+\n] + %1024[^+\n] + %99[^+\n] + %f + %d + %99[^+\n] \n",
		temp_item.name, 
		temp_item.category, 
		temp_item.description, 
		temp_item.url_image, 
		&temp_item.start_price, 	
		&temp_item.quantity,			
		temp_item.place) != 7)
	{	
		errorm("Impossible d'extraire la requ�te");
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
	time(&temp_item.expire_time);
	temp_item.expire_time = temp_item.expire_time + 3600; // temps d'expiration de 1H apr�s avoir mis le nouvel objet
	
	encode_item(client, &temp_item, ligne); 	// encodage en chaine de caract�res pour �tre plac� dans le fichier
	fseek(server->object_file, 0, SEEK_END);	// on se place � la fin du fichier
	if(fputs(ligne, server->object_file) == EOF)
	{
		errorm("Impossible d'ajouter un nouvel item");
		error_msg(client, "0x064");
		return FAIL;
	}
	
	send_socket(client, "ITEM_ADDED \n");
	fseek(server->object_file, 0, SEEK_SET);	// on se replace en d�but de fichier
	
	info("La requ�te s'est termin�e avec succ�s");
	return SUCCESS;
}

// fonction d'envoi des item achet�s
state req_hist_item_bought(struct user_t * client, struct server_t * server, char * buffer)
{
	struct object_t temp_item;
	unique_id_t user_uid;
	char ligne[256];
	
	
	if(sscanf(buffer,"REQ_HIST_ITEM_BOUGHT BY %ld \n", &user_uid) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	rewind(server->object_file);
	
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

// fonction d'envoi des cat�gorie sur le serveur
state req_cat(struct user_t * client, struct server_t * server, char * buffer)
{
	int tabex = 0;
	int index = 0;
	bool egal = FALSE;
	struct object_t temp_item;
	char ligne[2048];
	char  category[50][50]; 
	
	clean_b(buffer);
	
	info("Requ�te des cat�gorie du serveur");
	rewind(server->object_file);	
	while(fgets(ligne, sizeof(ligne), server->object_file) != NULL ) // GET EACH LINE
	{	
		egal = FALSE;
		if(decode_item(client, &temp_item, ligne) == FAIL)			  // DECODE LINE
			break;
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
		debugm(buffer);
		usleep(10000);
		send_socket(client, buffer);					 		 // SEND
		clean_b(buffer);								 	 	 // CLEAN BUFFER
		index++;
	}
	
	
	sprintf(buffer, "END_CATEGORY \n");
	debugm(buffer);
	usleep(10000);
	send_socket(client, buffer);					 		 // SEND
	
	rewind(server->object_file);							// RESET FILE
	
	info("La requ�te a bien aboutit");
	return SUCCESS;
}

// fonction d'envoi des items d'une cat�gories
state req_cat_access(struct user_t * client, struct server_t * server, char * buffer)
 {
 
	char category[50];
	char ligne[2048];
	struct object_t temp_item;
	
	info("Requ�te d'acc�s au objet d'une cat�gorie");
	
	if(sscanf(buffer,"REQ_CAT_ACCESS = %s \n", category) != 1)
	{
		
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	rewind(server->object_file);				 // RESET FILE
	
	debugm(category);
	debugm("d�but de la recher...");
	while(fgets(ligne, sizeof(ligne), server->object_file) != NULL) // GET EACH LINE
	{	
		debugm(ligne);
		decode_item(client, &temp_item, ligne);			  // DECODE LINE
		debugm(temp_item.category);
		
		if(strcmp(temp_item.category, category) >= 0 && temp_item.expire_time > server_time) // si c'est dans la bonne cat�gorie et si ce n'est pas expir�
		{
			clean_b(buffer);								 // CLEAN BUFFER
			sprintf(buffer, "ITEM = %ld + %s + %s + %f + %f + %d \n" , 
			temp_item.uid,
			temp_item.name,
			temp_item.category,
			temp_item.start_price,
			temp_item.temp_price,
			temp_item.quantity);						 	 // FORMAT
			
			usleep(10000);
		
			send_socket(client, buffer);					 // SEND
		}
		else // sinon on ignore la ligne et on continue
		{
			// IGNRORE
		}
	}
	
	usleep(10000);
	clean_b(buffer); 										 // CLEAN BUFFER
	sprintf(buffer, "END_ITEM \n");							 // FORMAT
	send_socket(client, buffer);					 		 // SEND
	
	rewind(server->object_file);
	
	info("La requ�te a bien aboutit");
	return SUCCESS;
 }
 
 // fonction d'envoi des d�tails d'un item
state req_item(struct user_t * client, struct server_t * server, char * buffer)
 {
	unique_id_t item_uid = 0;
	char ligne[256];
	struct object_t temp_item;
	
	
	
	if(sscanf(buffer,"REQ_ITEM = %ld \n", &item_uid) != 1)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	rewind(server->object_file);
	
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{
		decode_item(client, &temp_item, ligne);			  // DECODE LINE

		if(temp_item.uid == item_uid)
		{
			sprintf(buffer, "ITEM = %ld + %s + %s + %s + %ld \n",
			temp_item.uid,
			temp_item.description,
			temp_item.url_image,
			temp_item.place,
			temp_item.expire_time);				 	 	  // FORMAT
		
			send_socket(client, buffer);					  // SEND			
			clean_b(buffer);
			info("La requ�te a bien aboutit");
			return SUCCESS;
			
		}
				
	}
	
	error_msg(client, "0x070");
	return FAIL;
 }
 
// fonction de demande d'ench�re sur un item 
state req_bid_price(struct user_t * client, struct server_t * server, char * buffer)
 {
	float item_new_price = 0;
	unique_id_t item_uid = 0;
	
	char ligne[256];
	struct object_t temp_item;

	
	info("Requ�te d'ench�re");
	if(sscanf(buffer,"REQ_BID_PRICE = %f FOR %ld \n", &item_new_price, &item_uid) != 2)
	{
		errorm("Impossible d'extraire la requ�te");
		error_msg(client, "0x999");
		return FAIL;
	}
	clean_b(buffer);
	
	debugm("D�but de la rechercher...");
	rewind(server->object_file);
	while(fgets(ligne, sizeof(ligne), server->object_file)) // GET EACH LINE
	{
		debugm(ligne);
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
				temp_item.acheteur = client->uid;
				
				encode_item(client, &temp_item, ligne);
				fseek(server->object_file, - strlen(ligne), SEEK_CUR);
				fputs(ligne, server->object_file);
				break;
			}		
		}
		else
		{
			// CONTINUE
		}
	}
	
	clean_b(buffer);
	send_socket(client, "BID_OK \n");
	rewind(server->object_file);
	return SUCCESS;
 }
