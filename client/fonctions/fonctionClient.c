/* PROTOTYPES */

void warm(const char * war_msg)
{
	printf("%s: %s\n", WAR, war_msg);
}

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

void greenm(const char * green_msg)
{
	printf("%s%s%s\n", GREEN, green_msg, NORM);
}

void welcome_message(int argc, char * argv[])
{
	int index = 0;

	printf("\n\n\n\t----------------------------------------");
	printf("\n\tBALBIANI.L		 TEIKITUHAAHAA.M\n");
	printf("\n\tUPSSITECH		            STRI\n");
	printf("\t----------------------------------------\n");
	
	greenm("\n\tBienvenue sur Cbay, Application Cliente!\n\n");
	if(argc > 1)
		{
			warm("Arguments in the launching command[IGNORING ELSE THAN DEBUG]");
			while(index != argc)
			{
				if(strcmp(argv[index], "-debug") == 0)
					debug = DEBUG_ON; // MODE DEBUG ON
				else
					debug = DEBUG_OFF; // MODE DEBUG OFF
				fprintf(stdout, "ARGUMENT %d: %s \n", index, argv[index]);
				index++;
			}
			warm("MODE DEBUG ON");
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

state rcv_socket(struct user_t * client, char * buffer)
{
	size_t size_recv;
	
	clean_b(buffer);

	do
	{
		usleep(10000);
		size_recv = recv(client->socket_fd, buffer, 1024, MSG_DONTWAIT);
		
		if(size_recv == 0) // la connexion a été fermé
		{
			errorm("Connexion fermée par le serveur");
			close(client->socket_fd);
			exit(FAIL);
		}

	}while(size_recv == -1);	
	debugm("buffer:");
	debugm(buffer);	
	return SUCCESS;
}

bool send_socket(struct user_t * client, const char * buffer)
{
	send(client->socket_fd, buffer, strlen(buffer)  + 1, MSG_DONTWAIT);
	return SUCCESS;
}
