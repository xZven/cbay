/* PROTOTYPES */
void welcome_message(int argc, char * argv[]);
int ask_login(struct user_t * client,  char * buffer);

//

void welcome_message(int argc, char * argv[])
{
	int index = 0;

	printf("Bienvenue sur Cbay, Application cliente !\n");
	if(argc > 1)
		{
			fprintf(stdout, "[WAR]: Arguments in the launching command [IGNORING SOME]\n");
			while(index != argc)
			{
				if(strcmp(argv[index], "-debug") == 0)
					debug = DEBUG_ON; // MODE DEBUG ON
				else
					debug = DEBUG_OFF; // MODE DEBUG OFF
				fprintf(stdout, "ARGUMENT %d: %s \n", index, argv[index]);
				index++;
			}
			fprintf(stdout, "[WAR]: MODE DEBUG ON\n");
			index = 0;
		}
}

void clean_b(char * buffer)
{
	buffer[0]='\0';
}

int ask_login(struct user_t * client, char * buffer)
{
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
	return SUCCESS;



int rcv_socket(struct user_t * client, char * buffer)
{
size_t size_recv;
do
{
size_recv = recv(client->socket_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
usleep(10000); //PAUSE DE 10 mS
}
while(size_recv == -1);
return SUCCESS;
}



bool send_socket(struct user_t * client, const char * buffer)
{
	send(client->socket_fd, buffer, strlen(buffer)  + 1, MSG_CONFIRM);
	return SUCCESS;
}

bool error_msg(struct user_t * client, const char * error_code)
