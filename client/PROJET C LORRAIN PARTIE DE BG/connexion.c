int req_connexion(struct user_t * client, char * buffer)
{

	char login[250];
	char password[100];


	printf("Veuillez entrer votre login: ");
	scanf("%s", client->login);
	printf("Veuillez entrer votre mot de passe: ");
	scanf("%s", client->password); // sans écho
	sprintf(buff, "REQ_CONNECT = %s + %s \n", client->login, client->password);
	send_socket(client, buff); clean_b(buffer);
	rcv_socket(client, buffer);
	if(strcmp(buffer, "USER_CONNECTED \n") == 0)
	return SUCCESS;
	else
	printf("erreur serveur");
	printf("la connexion va se couper")
	exit;
}
