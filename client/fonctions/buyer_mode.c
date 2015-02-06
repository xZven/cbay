state buyer_mode(struct user_t * client, char * buffer)
{
	int choix = -1;
	int index;
	int choix_item = 0;
	int choix_op = 0;
	int choix_cat = 0;
	
	float prix = 0;
	
	struct object_t achat[100];		// tableau des item achaté
	struct object_t bid[100];		// tableau des ventes en cours
	
	char category[50][50];			// tableau de string 50 lignes de 50c max
	
	
	
	do
		{
			__fpurge(stdin);
			printf("Que souhaitez-vous faire? \n");
			printf("\t 1 - Consulter mon historique de d'achat \n ");
			printf("\t 2 - Gérer mes enchères en cours \n"); 
			printf("\t 3 - Effectuer un nouvel enchère \n");
			printf("\t 0 - Quitter le Programme \n");
			printf("Choix[1|2|3|0]: ");
		}while((scanf("%d", &choix) != 1 ) || choix > 3);
	
	switch(choix)
	{
		case 0: // Quitter le programme
			exit(SUCCESS);
			break;
		
		case 1: // Consulter l'historique des achats
			debugm("Consultation de l'historique d'achat");
			clean_b(buffer);
			sprintf(buffer, "REQ_HIST_ITEM_BOUGHT BY %ld \n", client->uid);
			debugm(buffer);
			send_socket(client, buffer); 
			clean_b(buffer);
			index = 0;
			
			do // affichage des item vendu
			{
				if(rcv_socket(client, buffer) == -1) // réception
				{
					errorm("Erreur lors de la réception");
					return FAIL;
				}
				debugm("BUFFER:");
				debugm(buffer);
				if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage
				{
					printf("\nFIN D'AFFICHAGE\n");
					break;
				}
				else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
				{
					echecm("Erreur reçu du serveur");
					printf("%s", buffer);
					break;
				}
				else
				{
				
					if(sscanf(buffer, "ITEM = %ld %s %s %f %f %d \n",
						&achat[index].uid,
						achat[index].name,
						achat[index].category,
						&achat[index].start_price,
						&achat[index].final_price,
						&achat[index].quantity) != 6)
					{
						warm("sscanf(), historique des ventes");
					}
					
					
					
					printf("%d - %ld\n\tNom:%s [%s]\n\tPrix de départ:%f\n\tPrix final:%f\n\tQuantité:%d\n\n", index + 1,
						achat[index].uid,
						achat[index].name,
						achat[index].category,
						achat[index].start_price,
						achat[index].final_price,
						achat[index].quantity);
					index++;
				}
					
			}while(1);
			
			break;
			
		case 2: // Gérer mes enchères en cours
			debugm("Gestion des enchères en cours");
			clean_b(buffer);
			sprintf(buffer, "REQ_BID_USER = %ld \n", client->uid);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			index = 0;
			
				do // affichage des item acheté
				{
					if(rcv_socket(client, buffer) == -1) // réception
					{
						errorm("Erreur lors de la réception");
						return FAIL;
					}
					debugm("BUFFER:");
					debugm(buffer);
					if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						printf("\nFIN D'AFFICHAGE\n");
						break;
					}
					else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
					{
						echecm("Erreur reçu du serveur");
						printf("%s", buffer);
						break;
					}
					else
					{
					
						if(sscanf(buffer, "ITEM = %ld %s %s %f %f %d \n",
							&bid[index].uid,
							bid[index].name,
							bid[index].category,
							&bid[index].start_price,
							&bid[index].temp_price,
							&bid[index].quantity) != 6)
						{
							warm("sscanf(), historique des ventes");
						}
						
						
						
						printf("%d - %ld\n\tNom:%s [%s]\n\tPrix de départ:%f\n\tPrix final:%f\n\tQuantité:%d\n\n", index + 1,
							bid[index].uid,
							bid[index].name,
							bid[index].category,
							bid[index].start_price,
							bid[index].temp_price,
							bid[index].quantity);
						index++;
					}
				}while(index <= 100);
				
				do // choix de l'item
				{
					__fpurge(stdin);
					printf("Numéro d'item [1 - %d]: ", index + 1);
				}while((scanf("%d", &choix_item) != 1) || (choix_item > index + 1) || (choix_item <= 0));
				
				do // choix de l'opération
				{
						__fpurge(stdin);
					printf("Que voules-vous faire ?\n");
					
					printf("1 - Augmenter l'enchère \n");
					greenm("\nPermet d’augmenter une enchère.\nCette opération est doit être suivi d’un prix supérieur à la mise en cours.");
					
					printf("2 - Annuler l'enchère \n");
					greenm("\nPermet d’annuler une enchère.\n");
					
					printf("0 - Quitter cette fonctionnalitée \n");
					
					printf("Choix[1|2|0]: ");				
				}while((scanf("%d", &choix_op) != 1) || (choix_op > 2) || (choix_op < 0));
				
				switch(choix_op) // Opération sur l'item
				{
					case 0:
						break;
					case 1: // suppresion de l'enchère
						do
						{
							__fpurge(stdin);
							printf("Prix: ");							
						}while(scanf("%f", &prix) != 1);
						clean_b(buffer);
						sprintf(buffer, "REQ_ON %ld = UP_BID + %f \n", bid[choix_item-1].uid, prix); 
						send_socket(client, buffer);
						clean_b(buffer);
						if(rcv_socket(client, buffer) == -1) // réception
						{
							errorm("Erreur lors de la réception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							greenm("L'enchère a bien été supprimé\n");		
						}
						else if(strncmp("ERROR", buffer, 5) == 0)
						{
							echecm("Erreur reçu du serveur");
							printf("%s", buffer);
						}
						else
						{
						}
						break;
					case 2:	// annuler l'ecnhère
						prix = -1;
						clean_b(buffer);
						sprintf(buffer, "REQ_ON %ld = CANCEL_BID + %f \n", bid[choix_item-1].uid, prix); 
						send_socket(client, buffer);
						clean_b(buffer);
						if(rcv_socket(client, buffer) == -1) // réception
						{
							errorm("Erreur lors de la réception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							greenm("L'enchère a bien été annulé");		
						}
						else if(strncmp("ERROR", buffer, 5) == 0)
						{
							echecm("Erreur reçu du serveur");
							printf("%s", buffer);
						}
						else
						{
						}
						break;
						
					default: // erreur
						errorm("Choix des fonctionnalités du mode vendeur\Exiting...");
						exit(FAIL);
				}
				break;
				
			
			
			
			
			break;
			
		case 3: // Effectuer un nouvel enchère
			
			debugm("Gestion des enchères en cours");
			clean_b(buffer);
			sprintf(buffer, "REQ_CAT \n");
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			
			index = 0;
			do // affichage des catégories
				{
					if(rcv_socket(client, buffer) == -1) // réception
					{
						errorm("Erreur lors de la réception");
						return FAIL;
					}
					debugm("BUFFER:");
					debugm(buffer);
					if(strcmp("END_CATEGORY \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						printf("\nFIN D'AFFICHAGE\n");
						break;
					}
					else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
					{
						echecm("Erreur reçu du serveur");
						printf("%s", buffer);
						break;
					}
					else
					{
					
						if(sscanf(buffer, "CATEGORY = %s \n", category[index]) != 1);
						{
							warm("sscanf(), Affichage des catégories");
						}
						
						printf("%d - %s", index + 1,category[index]);							
						index++;
					}
				}while(index <= 50);
			
			do // choix de la catégorie
				{
					__fpurge(stdin);
					printf("Numéro de catégorie [1 - %d]: ", index + 1);
				}while((scanf("%d", &choix_cat) != 1) || (choix_cat > index + 1) || (choix_cat <= 0));
			
			clean_b(buffer);
			sprintf(buffer, "REQ_CAT_ACCESS = %s \n", category[choix_cat - 1]);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			
			break;
	}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
}