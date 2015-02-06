state seller_mode(struct user_t * client, char * buffer)
{
	int choix = -1;
	int choix_item = 0, choix_op = 0;
	int index = 0;
	struct object_t vente[100];		// tableau des item vendu
	struct object_t bid[100];		// tableau des ventes en cours
	struct object_t new;
	
	while(1)
	{	
		do
		{
			__fpurge(stdin);
			printf("Que souhaitez-vous faire? \n");
			printf("\t 1 - Consulter mon historique de vente \n ");
			printf("\t 2 - Gérer mes ventes en cours \n"); 
			printf("\t 3 - Effectuer une nouvelle vente \n");
			printf("\t 0 - Quitter le Programme \n");
			printf("Choix[1|2|3|0]: ");
		}while((scanf("%d", &choix) != 1 ) || choix > 3);
		
		
		switch(choix)
		{
			case 0:
				exit(SUCCESS);
				break;
			
			case 1: // consulter l'Historique de vente de l'utilisateur
			{
				debugm("Consultation de l'historique d'achat");
				clean_b(buffer);
				sprintf(buffer, "REQ_ITEM_SOLD = %s \n", client->login);
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
							&vente[index].uid,
							vente[index].name,
							vente[index].category,
							&vente[index].start_price,
							&vente[index].final_price,
							&vente[index].quantity) != 6)
						{
							warm("sscanf(), historique des ventes");
						}
						
						
						
						printf("%d - %ld\n\tNom:%s [%s]\n\tPrix de départ:%f\n\tPrix final:%f\n\tQuantité:%d\n\n", index + 1,
							vente[index].uid,
							vente[index].name,
							vente[index].category,
							vente[index].start_price,
							vente[index].final_price,
							vente[index].quantity);
						index++;
					}
						
				}while(1);
				break;
			}
			case 2: // gérer les vente en cours
			{
				debugm("Gestion des ventes en cours");
				clean_b(buffer);
				sprintf(buffer, "REQ_ITEM_USER = %ld \n", client->uid);
				debugm(buffer);
				send_socket(client, buffer); clean_b(buffer);
				index = 0;
				do // affichage des item vendu
				{
					if(rcv_socket(client, buffer) == -1) // réception
					{
						errorm("Erreur lors de la réception");
						return FAIL;
					}
					if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						printf("\nFIN D'AFFICHAGE\n");
						index--;
						break;
					}
					else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
					{
						echecm("Erreur reçu du serveur");
						printf(buffer);
						break;
					}
					else
					{
		/*************************************************/			
						if(sscanf(buffer, "ITEM = %ld + %51[A-Za-z0123456789 ] + %51[A-Za-z ] + %f + %f + %d \n",
							&bid[index].uid,
							bid[index].name,
							bid[index].category,
							&bid[index].start_price,
							&bid[index].temp_price,
							&bid[index].quantity) != 6)
						{
							warm("Un objet n'a pas pu être affiché correctement car il ne respecte pas le format de la RFC");
						}
						
						
						
						printf("%d - \tUID:%ld\n\tNom: %s\n\tCatégorie: %s\n\tPrix de départ: %f Euro\n\tDernier enchère: %f Euro\n\tQuantité: %d\n\n", index + 1,
							bid[index].uid,
							bid[index].name,
							bid[index].category,
							bid[index].start_price,
							bid[index].temp_price,
							bid[index].quantity);
						index++;
					}
					debugm("boucle");
				}while(1);
				
				do // choix de l'item
				{
					__fpurge(stdin);
					printf("Numéro d'item [1 - %d]: ", index + 1);
				}while((scanf("%d", &choix_item) != 1) || (choix_item > index + 1) || (choix_item <= 0));
				
				do // choix de l'opération
				{
						__fpurge(stdin);
					printf("Que voules-vous faire ?\n");
					
					printf("1 - Supprimer l'enchère \n");
					greenm("Permet de supprimer une enchère en cours.\nL’objet sera supprimé du serveur.\n");
					
					printf("2 - Annuler l'enchère \n");
					greenm("Permet d’annuler une enchère.\nTous les compteurs de l’objet seront remis à zéro.\n");
					
					printf("3 - Terminé une vente\n");
					greenm("Permet de mettre fin à une vente.\nLe dernier à avoir enchérit l'objet sera l'acheteur de cet l'objet.\n");
					
					printf("0 - Quitter cette fonctionnalitée \n");
					
					printf("Choix[1|2|3|0]: ");				
				}while((scanf("%d", &choix_op) != 1) || (choix_op > 3) || (choix_op < 0));
				
				debugm("Opération sur ITEM");
				switch(choix_op) // Opération sur l'item
				{
					case 0:
						break;
					case 1: // suppresion de l'enchère
					{
						debugm("Suppresion de l'enchère");
						clean_b(buffer);
						sprintf(buffer, "REQ_OP = DELETE_BID ON %ld \n", bid[choix_item-1].uid);
						send_socket(client, buffer);
						debugm(buffer);
						if(rcv_socket(client, buffer) == -1) // attente de la réponse
						{
							errorm("Erreur lors de la réception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'enchère a bien été supprimé\n");		
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
					}
					case 2:	// annuler l'ecnhère
					{
						clean_b(buffer);
						sprintf(buffer, "REQ_OP = CANCEL_BID ON %ld \n", bid[choix_item-1].uid);
						send_socket(client, buffer);
						if(rcv_socket(client, buffer) == -1) // attente de la réponse
						{
							errorm("Erreur lors de la réception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'enchère a bien été annulé\n");		
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
					}
					case 3: // terminé une vente
					{
					clean_b(buffer);
						sprintf(buffer, "REQ_OP = END_BID ON %ld \n", bid[choix_item-1].uid);
						if(rcv_socket(client, buffer) == -1) // réception
						{
							errorm("Erreur lors de la réception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'enchère a bien été terminé\n");		
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
					}	
					default: // erreur
						errorm("Choix des fonctionnalités du mode vendeur\Exiting...");
						exit(FAIL);
				}
				break;
			}	
			case 3: //effectuer une nouvelle vente
			{
				debugm("Ajout d'une nouvelle vente");
				
				printf("Nom: ");
					__fpurge(stdin);
					fgets(new.name, 51, stdin);
				printf("Catégorie: ");
					__fpurge(stdin);
					fgets(new.category, 51, stdin);
				printf("Description: ");
					__fpurge(stdin);
					fgets(new.description, 1024, stdin);
				printf("URL d'une image: ");
					__fpurge(stdin);
					fgets(new.url_image, 100, stdin);
				
					do // Prix
					{
						printf("Prix (tous le lot): ");
						__fpurge(stdin);
					}while(scanf("%f", &new.start_price)!=1);
				
					do // Quantité
					{
						printf("Quantité: ");
						__fpurge(stdin);
					}while(scanf("%d", &new.quantity)!= 1);
				
				printf("Adresse de l'objet: ");
					__fpurge(stdin);
					fgets(new.place, 100, stdin);
				
				
				
				clean_b(buffer);
				sprintf(buffer, "PUT_NEW_ITEM = %s + %s + %s + %s + %f + %d + %s \n", 
					new.name,
					new.category,
					new.description,
					new.url_image,
					new.start_price,
					new.quantity,
					new.place);
				
				debugm(buffer);
				send_socket(client, buffer); clean_b(buffer);
				if(rcv_socket(client, buffer) == -1) // réception
				{
					errorm("Erreur lors de la réception");
					return FAIL;
				}
				debugm("BUFFER:");
				debugm(buffer);
				if(strcmp("ITEM_ADDED \n", buffer) == 0) // l'objet a bien été ajouté
				{
					greenm("L'enchère a bien été ajouté\n");		
				}
				else if(strncmp("ERROR", buffer, 5) == 0)
				{
					echecm("Erreur reçu du serveur");
					printf(buffer);
				}
				else
				{
				}
				break;
			}	
			
			default:
				errorm("Choix des fonctionnalités du mode vendeur\Exiting...");
				exit(FAIL);
		}
	}	
}