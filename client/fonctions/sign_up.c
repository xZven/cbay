/*
 * Cette fonction permet de choisir entre se connecter directement au serveur,
 * ou de s'inscrire avant de se connecter.
 */
state req_sign_up(struct user_t * client, char * buffer)
{ 
	char buff[50];
	int tentative = 0;
	do
	{
		do // LOGIN
		{
			__fpurge(stdin);
			if(tentative <4)
			{
				if(tentative == 0)printf("\nVeuillez rentrer un login: ");
				if(tentative > 0)printf("\nTENTATIVE %d :Veuillez rentrer un autre login SVP: ", tentative +1);
			}
			else
			{
				echecm("4 tentatives");
			}
		}while ((scanf("%s", client->login) != 1 ));
		tentative ++;

		clean_b(buffer);	
		sprintf(buffer, "REQ_VERIFY_LOGIN = %s \n", client->login);
		send_socket(client, buffer);
		
		if(rcv_socket(client, buff) == -1) // réponse du serveur
		{
			errorm("Erreur lors de la réception");
			return FAIL;
		}
		
		debugm(buff);
	}while(strncmp(buff, "LOGIN_FREE", 10) != 0);	//Sort de la boucle si la réponse est LOGIN_FREE
	
	printf("%sLe nom d'utilisateur est libre.\n%s", GREEN, NORM);
	printf("Entrer un mot de passe SVP: ");
	scanf("%s", client->password);
	
	clean_b(buffer);	
	sprintf(buffer, "REQ_NEW_USER = %s + %s \n", client->login, client->password);
	send_socket(client, buffer); clean_b(buffer);
	
	if(rcv_socket(client, buffer) == -1)
		{
			fprintf(stderr,"[ERROR]: Erreur lors de la réception: %s\n", strerror(errno));
			return FAIL;
		}
	
	if(strcmp(buffer, "USER_ADDED \n") == 0)
	{
		printf("%sVotre compte a bien été créé !%s", GREEN, NORM);
		return SUCCESS;
	}
	else
	{
		printf("%s\n", buffer);
		return FAIL;
	}
}