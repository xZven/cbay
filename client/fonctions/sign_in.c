state req_connect(struct user_t * client, char * buffer)
{
	int tentative = 0;

	do
	{
		
		printf("\tLogin: ");
		scanf("%s", client->login);
		printf("\tMot de passe: ");
		scanf("%s", client->password); // sans écho
		tentative++;
		printf("Tentative de connexion %d ...\n", tentative);
		clean_b(buffer);
		sprintf(buffer, "REQ_CONNECT = %s + %s \n", client->login, client->password);
		send_socket(client, buffer);
	
		if(rcv_socket(client, buffer) == -1)
		{
			errorm("Erreur lors de la réception");
			return FAIL;
		}
		
		if(strcmp(buffer, "USER_CONNECTED \n") == 0)
		{
			printf("%sConnexion au serveur réussi !%s\n", GREEN, NORM);			
			return SUCCESS;
		}
	}while(tentative < 4);
	
	echecm("4 Tentatives");
	
	return FAIL;
	
}

