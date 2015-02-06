state req_connect(struct user_t * client, char * buffer)
{
	int tentative = 0;

	do
	{
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
	
		if(rcv_socket(client, buffer) == -1)
		{
			errorm("Erreur lors de la réception");
			return FAIL;
		}
		
		if(strncmp(buffer, "USER_CONNECTED", 14) == 0)
		{	
			sscanf(buffer, "USER_CONNECTED = %ld \n", &client->uid);
			clean_b(buffer);
			printf("%sConnecté en tant que:%s %s\n", GREEN, NORM, client->login);
			debugm ("connecion réussi");
			return SUCCESS;
		}
	}while(tentative < 4);
	
	echecm("4 Tentatives");
	echecm("Vous n'avez pas pû vous connecter au serveur\nExiting...");
	exit(FAIL);	
}

