/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
 /*
  * cette fonction permet de se connecter à un server d'enchère
  * il reçoit en paramètre une structure de type user_t et un
  * pointeur sur un buffer qui sera utilisé pour former les requêtes
  * et recevoir les réponse du serveur.
  */
state req_connect(struct user_t * client, char * buffer)
{
	int tentative = 0;

	do
	{
		// demande des identifiants
		greenm("\nConnexion:\n");
		printf("\tLogin: ");
		scanf("%s", client->login);
		printf("\tMot de passe: ");
		scanf("%s", client->password); // sans écho
		tentative++;
		printf("\nTentative de connexion %d ...\n", tentative);
		clean_b(buffer);
		sprintf(buffer, "REQ_CONNECT = %s + %s \n", client->login, client->password);
		send_socket(client, buffer);
	
		rcv_socket(client, buffer); // réception de la réponse
		
		if(strncmp(buffer, "USER_CONNECTED", 14) == 0) // si la connexion a bien aboutit
		{	
			sscanf(buffer, "USER_CONNECTED = %ld \n", &client->uid);
			clean_b(buffer);
			system("clear");
			printf("%sConnecté en tant que:%s %s\n", GREEN, NORM, client->login);
			debugm ("connecion réussi");
			return SUCCESS;
		}
	}while(tentative < 4);
	
	echecm("4 Tentatives");
	echecm("Vous n'avez pas pû vous connecter au serveur\nExiting...");
	exit(FAIL);	
}

