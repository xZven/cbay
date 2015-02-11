/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
 /*
  * il reçoit en paramètre une structure de type user_t et un
  * pointeur sur un buffer qui sera utilisé pour former les requêtes
  * et recevoir les réponse du serveur.
  */
mode req_mode(struct user_t * client, char * buffer)
{
	int mode = -1; // variable pour le choix du mode
	
	debugm("Choix du mode");
	do // choix du mode
	{
		__fpurge(stdin);
		printf("Choix du mode:\n"); // CHOIX DU MODE
			printf("\t 1 - Mode Acheteur\n");
			printf("\t 2 - Mode Vendeur\n");
			printf("\t 3 - Mode Administrateur\n");
		printf("Choix[1|2|3]: ");		
	}while(((scanf("%d", &mode)) != 1) || (mode > 3));
	
	switch(mode) // selon le mode
	{
		case 1: // MODE ACHETEUR
		{
		/*	inutile depuis la seconde version de la rfc
			// la demande au serveur n'est pas obligatoire pour ce mode
			clean_b(buffer);	
			sprintf(buffer, "REQ_MODE = BUY FOR %ld", client->uid);
			send_socket(client, buffer);
		
			
			if(rcv_socket(client, buff) == -1) // réponse du serveur
			{
				errorm("Erreur lors de la réception");
				return FAIL;
			}
		*/
			client->mode = 'b'; // mode acheteur
			return BUYER;
			break;
		}	
		case 2: // MODE VENDEUR
		{
		/* inutile depuis la seconde version de la rfc
			// la demande au serveur n'est pas obligatoire pour ce mode
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
		case 3:	// MODE ADMIN
		{
			clean_b(buffer);	
			sprintf(buffer, "REQ_MODE = ADMIN FOR %ld \n", client->uid); // formatage de la reqûete
			debugm(buffer);
			send_socket(client, buffer); // envoi de la requête
			
			rcv_socket(client, buffer); // réception de la réponse
			
			if(strncmp(buffer, "ERROR", 5) == 0) // s'il y a une erreur
			{
				echecm("Vous n'avez pas pu choisir ce mode !");
				printf(buffer);
				return FAIL;
			}
			else // sinon on devient administrteur
			{
				client->mode = 'a';
				return ADMIN;
			}
			break;
		}	
		default: // pas possible mais on prévoit un cas par défaut
			return 'i'; /* mode indéterminée */
	}
	
	debugm("Erreur inconnu req_mode()");
	return FAIL;
}