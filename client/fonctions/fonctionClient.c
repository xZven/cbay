/* PROTOTYPES */


void info(const char * info_msg)
{
	printf("%s: %s\n", INFO, info_msg);
}

void debugm(const char * debug_msg)
{
	if(debug)printf("%s: %s\n", DEBUG_MSG, debug_msg);
}

void errorm(const char * error_msg)
{
	printf("%s: %s  %s\n", ERROR, error_msg, strerror(errno));
}

void echecm(const char * echec_msg)
{
	printf("%s: %s\n", ECHEC, echec_msg);
}

void welcome_message(int argc, char * argv[]);
int ask_login(struct user_t * client,  char * buffer);

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
	int index=0;
	while(index < sizeof(buffer))
	{
		buffer[index]='\0';
		index++;
	}
}

int ask_login(struct user_t * client, char * buffer)
{
	do
	{
		do
		{
				__fpurge(stdin);	
				printf("Veuillez rentrer un login: ");		
		}while ((scanf("%s", client->login) != 1 ));
			if(debug) fprintf(stdout, "[DEBUG]: login: %s\n", client->login);
			if(debug) fprintf(stdout, "[DEBUG]: REQ_VERIFY_LOGIN = %s\n", client->login); //DEBUG
		clean_b(buffer);	
		sprintf(buffer, "REQ_VERIFY_LOGIN = %s ", client->login);
			if(debug) fprintf(stdout, "[DEBUG]: buffer: %s\n", buffer);
		send(client->socket_fd, buffer, strlen(buffer) + 1, MSG_CONFIRM);
			if(debug) fprintf(stdout, "[DEBUG]: Envoyé !\n");
		clean_b(buffer);
		// en attente de la réponse du serveur
	}while(strncmp(buffer, "LOGIN_FREE", 10) == 0);	//Sort du while ssi la réponse du serveur est LOGIN_FREE
	return SUCCESS;
}

int rcv_socket(struct user_t * client, char * buffer) // pb liée a tcp 
{
	size_t size_recv;	
	char temp[2];
	int index = 0;
	
	clean_b(buffer); clean_b(temp);
	
	usleep(10000);
	do
	{
		size_recv = recv(client->socket_fd, temp, 1, MSG_WAITALL);
		if(size_recv == 0) // la connexion a été fermé
		{
			return FAIL;
		}
		buffer[index] = temp[0];
		index++;
			
		debugm(buffer);	
		
	}while(buffer[index - 1] != '\n');
	buffer[index] ='\0';

	return SUCCESS;
}

bool send_socket(struct user_t * client, const char * buffer)
{
	send(client->socket_fd, buffer, strlen(buffer)  + 1, MSG_DONTWAIT);
	return SUCCESS;
}
