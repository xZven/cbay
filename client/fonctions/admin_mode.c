state admin_mode(struct user_t * client, char * buffer)
{
	int choix = 0;
	int choix_op = -1;
	int choix_item = 0;
	int index = 0;
	struct user_t temp_user;
	char  temp_password1[20], temp_password2[20];
	struct object_t item[NMAX_ITEM];
	
while(1)
{	
	do
	{
		printf("Que voulez-vous faire ?\n"); // CHOIX DU MODE
			printf("\t 1 - Gérer les utilisateurs \n");
			printf("\t 2 - Gérer les ventes(supprimer/annuler une vente \n");
			printf("\t 0 - Quitter le programme \n");
		printf("Choix[1|2|0]: ");
		__fpurge(stdin);
	}while(((scanf("%d", &choix)) != 1) || (choix > 2));
	
	switch(choix)
	{
		case 0:
			exit(0);
			break;
		case 1:
		{
			printf("\n\nGestion des utilisateurs\n");
			do
			{
				printf("Que voulez-vous faire ?\n"); // CHOIX DU MODE
					printf("\t 1 - Changer le mot de passe d'un Utilisateur \n");
					printf("\t 2 - Supprimer un utilisateur \n");
					printf("\t 3 - Revenir en arrière \n");
					printf("\t 0 - Quitter le programme \n");
				printf("Choix[1|2|3|0]: ");
				__fpurge(stdin);
			}while(((scanf("%d", &choix)) != 1)	|| (choix > 3));
			
			switch(choix)
			{
				case 0: // QUIT
					exit(0);
					break;					
				case 1: // CHANGE PW
				{
					do
					{
						__fpurge(stdin);
						printf("\n");
						printf("Nom de l'utilisateur dont vous voulez changer le mot de passe: ");
							scanf("%s", temp_user.login);
						printf("Nouveau mot de passe: ");
							scanf("%s", temp_password1);
						printf("Retaper le mot de passe: ");
							scanf("%s", temp_password2);
						if(strcmp(temp_password1, temp_password2) == 0)
						{
							strcpy(temp_user.password, temp_password2);
							break;
						}
					}while(1);
					
					printf("Envoi de la requête au serveur...\n");
					
					clean_b(buffer);
					sprintf(buffer, "REQ_NEW_PW = %s ON %s BY %ld \n", temp_user.password, temp_user.login, client->uid);					
					send_socket(client, buffer); // Envoi de la requête
					
					if(rcv_socket(client, buffer) == -1)
					{
						errorm("Erreur lors de la réception");
						return FAIL;
					}
					
					if(strncmp(buffer, "PASSWORD_CHANGED \n", 16) ==0) // PASSWORD CHANGED SUCCESSFULL
					{
						printf("Le mot de passe de l'utilisateur a été changé avec succès ! \n");
					}
					else
					{
						printf("Le mot de passe n'a pas pu être changé\n");
						printf("%s\n", buffer);
					}
					printf("\n\nRetour...\n\n");
					break;
				}
				case 2: // DELETE USER
				{
					do
					{
						__fpurge(stdin);
						printf("\n");
						printf("Nom de l'utilisateur que vous voulez supprimer: ");
							if(scanf("%s", temp_user.login) == 1)
							{
								break;
							}						
					}while(1);
					
					printf("Envoi de la requête au serveur...\n");
					
					clean_b(buffer);
					sprintf(buffer, "REQ_DEL_USER = %s BY %ld \n", temp_user.login, client->uid);					
					send_socket(client, buffer); 			// Envoi de la requête
					
					rcv_socket(client, buffer); // attente de la réponse
	
					
					if(strncmp(buffer, "USER_DELETED", 12) ==0) // PASSWORD CHANGED SUCCESSFULL
					{
						greenm("L'utilisateur a été supprimé avec succès ! \n");
					}
					else
					{
						redm("L'utilisateur n'a pas pû être supprimé\n");
						printf("%s\n", buffer);
					}
					printf("\nRetour...\n\n");
					
					
					break;
				}
				case 3: // BACK
					break;
			}
			break;
		}
		case 2:
		{
			printf("\n\nGestion des Enchères\n");
			
			clean_b(buffer);
			sprintf(buffer, "REQ_ALL_ITEM BY %ld \n", client->uid);					
			send_socket(client, buffer); // Envoi de la requête
			index = 0;
			printf("Contact du serveur pour affichage des items...\n\n");
				do // affichage des item
				{
						rcv_socket(client, buffer); // réception
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
								&item[index].uid,
								item[index].name,
								item[index].category,
								&item[index].start_price,
								&item[index].temp_price,
								&item[index].quantity) != 6)
							{
								warm("Un objet n'a pas pu être affiché correctement car il ne respecte pas le format de la RFC");
							}
							
							
							
							printf("%d - \tUID:%ld\n\tNom: %s\n\tCatégorie: %s\n\tPrix de départ: %f Euro\n\tDernier enchère: %f Euro\n\tQuantité: %d\n\n", index + 1,
								item[index].uid,
								item[index].name,
								item[index].category,
								item[index].start_price,
								item[index].temp_price,
								item[index].quantity);
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
					
					printf("0 - Quitter cette fonctionnalitée \n");
					
					printf("Choix[1|2|3|0]: ");				
				}while((scanf("%d", &choix_op) != 1) || (choix_op > 2) || (choix_op < 0));
				
				debugm("Opération sur ITEM");
				switch(choix_op) // Opération sur l'item
				{
					case 0:
						break;
					case 1: // suppresion de l'enchère
					{
						debugm("Suppresion de l'enchère");
						clean_b(buffer);
						sprintf(buffer, "REQ_OP = DELETE_BID ON %ld \n", item[choix_item-1].uid);
						send_socket(client, buffer);
						debugm(buffer);
						rcv_socket(client, buffer); // réception
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
						sprintf(buffer, "REQ_OP = CANCEL_BID ON %ld \n", item[choix_item-1].uid);
						send_socket(client, buffer);
						rcv_socket(client, buffer); // réception
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
					default: // erreur
						errorm("Choix des fonctionnalités du mode vendeur\Exiting...");
						exit(FAIL);
				}
			break;
		}
	}
	
}

}
