int req_inscription(struct user_t * client, char * buffer)
{ 
//VARIABLES

	char password[100];
	char buff[250];


///////////////////////////////////////!!:DECLARATION VARIABLES :!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


	do
	{
		do
		{
				__fpurge(stdin);	
				printf("Veuillez rentrer un login: ");		
		}
		while ((scanf("%s", client->login) != 1 ));
			if(debug) fprintf(stdout, "[DEBUG]: login: %s\n", client->login);
			if(debug) fprintf(stdout, "[DEBUG]: REQ_VERIFY_LOGIN = %s\n", client->login); //DEBUG
		clean_b(buffer);	
		sprintf(buffer, "REQ_VERIFY_LOGIN = %s ", client->login);
			if(debug) fprintf(stdout, "[DEBUG]: buffer: %s\n", buffer);
		send(client->socket_fd, buffer, strlen(buffer) + 1, MSG_CONFIRM);
			if(debug) fprintf(stdout, "[DEBUG]: Envoyé !\n");
		clean_b(buffer);
		// en attente de la réponse du serveur
	}
	while(strncmp(buffer, "LOGIN_FREE", 10) == 0));	//Sort du while ssi la réponse du serveur est LOGIN_FREE
	printf("Votre login est libre, veuillez rentrer un mot de passe svp: ")
	scanf("%s", password);
	sprintf(buff, "REQ_NEW_USER = %s + %s \n", client->login, password);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	if(strcmp(buffer, "USER_ADDED \n") == 0)
	return SUCCESS;
	else
	printf("erreur serveur");
	exit;

// INSCRIPTION TERMINEE

