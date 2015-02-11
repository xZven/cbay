/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
/*
 * Cette fonction permet de choisir entre se connecter directement au serveur,
 * ou de s'inscrire avant de se connecter.
 * il reçoit en paramètre une structure de type user_t et un
 * pointeur sur un buffer qui sera utilisé pour former les requêtes.
 */
state req_sign_up(struct user_t * client, char * buffer)
{ 
	char buff[50]; // permet de recevoir les réponse du serveur
	int tentative = 0;
	do // tant que le login est libre (4 tentatives)
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
		
		rcv_socket(client, buff); // réponse du serveur
		
		debugm(buff); debugm("Réception");
	}while(strncmp(buff, "LOGIN_FREE", 10) != 0);	//Sort de la boucle si la réponse est LOGIN_FREE
	
	printf("%sLe nom d'utilisateur est libre.\n%s", GREEN, NORM);
	printf("Entrer un mot de passe SVP: ");
	scanf("%s", client->password);
	
	clean_b(buffer);	
	sprintf(buffer, "REQ_NEW_USER = %s + %s \n", client->login, client->password);
	send_socket(client, buffer); clean_b(buffer);
	
	rcv_socket(client, buffer); // réponse du serveur
	
	if(strcmp(buffer, "USER_ADDED \n") == 0) // si l'utilisateur a bien été ajouté
	{
		greenm("Votre compte a bien été créé !");
		return SUCCESS;
	}
	else
	{
		echecm("L'utilisateur n'a pas pû être ajouté");
		printf(buffer);
		return FAIL;
	}
}