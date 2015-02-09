mode req_mode(struct user_t * client, char * buffer)
{
	int mode = -1;
	
	debugm("Choix du mode");
	do
	{
		__fpurge(stdin);
		printf("Choix du mode:\n"); // CHOIX DU MODE
			printf("\t 1 - Mode Acheteur\n");
			printf("\t 2 - Mode Vendeur\n");
			printf("\t 3 - Mode Administrateur\n");
		printf("Choix[1|2|3]: ");		
	}while(((scanf("%d", &mode)) != 1) || (mode > 3));
	
	switch(mode)
	{
		case 1: // ACHETEUR
		{
		/*	// la demande au serveur n'est pas obligatoire pour ce mode
			clean_b(buffer);	
			sprintf(buffer, "REQ_MODE = BUY FOR %ld", client->uid);
			send_socket(client, buffer);
		
			
			if(rcv_socket(client, buff) == -1) // réponse du serveur
			{
				errorm("Erreur lors de la réception");
				return FAIL;
			}
		*/
			client->mode = 'b';
			return BUYER;
			break;
		}	
		case 2: // VENDEUR
		{
		/* // la demande au serveur n'est pas obligatoire pour ce mode
			clean_b(buffer);	
			sprintf(buffer, "REQ_MODE = SELL FOR %ld", client->uid);
			send_socket(client, buffer);
			
			if(rcv_socket(client, buff) == -1) // réponse du serveur
			{
				errorm("Erreur lors de la réception");
				return FAIL;
			}
		*/
			client->mode = 's';
			return SELLER;
			break;
		}	
		case 3:	// ADMIN
		{
			clean_b(buffer);	
			sprintf(buffer, "REQ_MODE = ADMIN FOR %ld \n", client->uid);
			debugm(buffer);
			send_socket(client, buffer);
			
			rcv_socket(client, buffer);
			
			if(strncmp(buffer, "ERROR", 5) == 0)
			{
				echecm("Vous n'avez pas pu choisir ce mode !");
				printf(buffer);
				return FAIL;
			}
			else
			{
				client->mode = 'a';
				return ADMIN;
			}
			break;
		}	
		default:
			return 'i'; /* indéterminée */
	}
	
	debugm("Erreur inconnu req_mode()");
	return FAIL;
}