/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
 /*
  * il re�oit en param�tre une structure de type user_t et un
  * pointeur sur un buffer qui sera utilis� pour former les requ�tes
  * et recevoir les r�ponse du serveur.
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
		
			
			if(rcv_socket(client, buff) == -1) // r�ponse du serveur
			{
				errorm("Erreur lors de la r�ception");
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
			
			if(rcv_socket(client, buff) == -1) // r�ponse du serveur
			{
				errorm("Erreur lors de la r�ception");
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
			sprintf(buffer, "REQ_MODE = ADMIN FOR %ld \n", client->uid); // formatage de la req�ete
			debugm(buffer);
			send_socket(client, buffer); // envoi de la requ�te
			
			rcv_socket(client, buffer); // r�ception de la r�ponse
			
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
		default: // pas possible mais on pr�voit un cas par d�faut
			return 'i'; /* mode ind�termin�e */
	}
	
	debugm("Erreur inconnu req_mode()");
	return FAIL;
}