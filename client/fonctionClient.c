/* PROTOTYPES */
void welcome_message(int argc, char * argv[]);
void clean_b(char * buffer);
int req_verify_login(struct user_t * client,  char * buffer);

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

int req_verify_login(struct user_t * client, char * buffer)
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
	 
	
	return SUCCESS;
}




