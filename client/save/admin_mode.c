state admin_mode(struct user_t * client, char * buffer)
{
	int choix = 0, item_n = 0;
	int index = 0;
	int tabex = 0;
	struct user_t temp_user;
	char  temp_password1[20], temp_password2[20];
	struct object_t item[NMAX_ITEM];
	
while(1)
{	
	do
	{
		printf("Que voulez-vous faire ?\n"); // CHOIX DU MODE
			printf("\t 1 - G�rer les utilisateurs \n");
			printf("\t 2 - G�rer les ventes(supprimer/annuler une vente \n");
			printf("\t 0 - Quitter le programme \n");
		printf("Choix[1|2|0]: ");
		__fpurge(stdin);
	}while(((scanf("%d", &choix)) != 1) 
	|| (choix > 2));
	
	switch(choix)
	{
		case 0:
			exit(0);
			break;
		case 1:
			printf("\n\nGestion des utilisateurs\n");
			do
			{
				printf("Que voulez-vous faire ?\n"); // CHOIX DU MODE
					printf("\t 1 - Changer le mot de passe d'un Utilisateur \n");
					printf("\t 2 - Supprimer un utilisateur \n");
					printf("\t 3 - Revenir en arri�re \n");
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
					
					printf("Envoi de la requ�te au serveur...\n");
					
					clean_b(buffer);
					sprintf(buffer, "REQ_NEW_PW = %s ON %s BY %ld \n", temp_user.password, temp_user.login, client->uid);					
					send_socket(client, buffer); // Envoi de la requ�te
					
					if(rcv_socket(client, buffer) == -1)
					{
						fprintf(stderr,"[ERROR]: Erreur lors de la r�ception: %s\n", strerror(errno));
						return FAIL;
					}
					
					if(strncmp(buffer, "PASSWORD_CHANGED \n", 16) ==0) // PASSWORD CHANGED SUCCESSFULL
					{
						printf("Le mot de passe de l'utilisateur a �t� chang� avec succ�s ! \n");
					}
					else
					{
						printf("Le mot de passe n'a pas pu �tre chang�\n");
						printf("%s\n", buffer);
					}
					printf("\n\nRetour...\n\n");
					break;
				case 2: // DELETE USER
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
					
					printf("Envoi de la requ�te au serveur...\n");
					
					clean_b(buffer);
					sprintf(buffer, "REQ_DEL_USER = %s BY %ld \n", temp_user.login, client->uid);					
					send_socket(client, buffer); 			// Envoi de la requ�te
					
					if(rcv_socket(client, buffer) == FAIL) // attente de la r�ponse
					{
						fprintf(stderr,"[ERROR]: Erreur lors de la r�ception: %s\n", strerror(errno));
						return FAIL;
					}
					
					if(strncmp(buffer, "USER_DELETED", 12) ==0) // PASSWORD CHANGED SUCCESSFULL
					{
						printf("L'utilisateur a �t� supprim� avec succ�s ! \n");
					}
					else
					{
						printf("L'utilisateur n'a pas p� �tre supprim�\n");
						printf("%s\n", buffer);
					}
					printf("\nRetour...\n\n");
					
					
					break;
				case 3: // BACK
					break;
			}
			break;
		case 2:
			printf("\n\nGestion des Ench�res\n");
			
			clean_b(buffer);
			sprintf(buffer, "REQ_ALL_ITEM BY %ld \n", client->uid);					
			send_socket(client, buffer); // Envoi de la requ�te
			printf("Contact du serveur pour affichage des items...\n\n");
			do
			{
				if(rcv_socket(client, buffer) == -1)
				{
					fprintf(stderr,"[ERROR]: Erreur lors de la r�ception: %s\n", strerror(errno));
					return FAIL;
				}
				
				if(strncmp(buffer,"END_ITEM \n",11) ==0)
				{
					break;
				}
				
				if(sscanf(buffer, "ITEM = %ld %s %s %f %f %d \n",
				&item[index].uid,
				item[index].name,
				item[index].category,
				&item[index].start_price,
				&item[index].temp_price,
				&item[index].quantity) != 6);
				{
					fprintf(stderr,"[ERROR]: Erreur lors de la r�ception des items: %s\n", strerror(errno));
					if(debug)printf("[DEBUG]: buffer:%s \n", buffer);
				}
				index++;
			}while(1);
			// AFFICHAGE DES ITEMS
			
			for(tabex = 0; tabex <= index; index++)
			{
			
			printf("%d - %ld\n \t%s\n \t%s\n \t%f\n \t%f\n \t%d\n \n",
				tabex,
				item[tabex].uid,
				item[tabex].name,
				item[tabex].category,
				item[tabex].start_price,
				item[tabex].temp_price,
				item[tabex].quantity);	
			sleep(1);
			}
			
			do
			{
				__fpurge(stdin);
				printf("Num�ro de l'item de vous voulez modifier \n");
				printf("[1 - %d]: ", index+1);
			}while(((scanf("%d", &item_n)) != 1) || (choix > index));
			
			do
			{
				__fpurge(stdin);
				printf("Que voules-vous faire ? \n");
				printf("\t 1 - Annuler l'ench�re: Remettre l'ench�re � son prix initial \n");
				printf("\t 2 - Supprimer l'objet de la base de donn�e du serveur \n");
				printf("\t 0 - Quitter le programme");
				printf("[0|1|2]: ");
			}while(((scanf("%d", &choix)) != 1) || (choix  > 2));
			
			switch(choix)
			{
				case 0: 
					exit(0);
					break;
				
				case 1:
					printf("Envoi de la requ�te d'annulation d'ench�re de l'item: %d\n", item_n);
					clean_b(buffer);
					sprintf(buffer, "REQ_OP = CANCEL ON %ld \n", item[item_n-1].uid);					
					send_socket(client, buffer); // Envoi de la requ�te
					
					if(rcv_socket(client, buffer) == -1)
				{
					fprintf(stderr,"[ERROR]: Erreur lors de la r�ception: %s\n", strerror(errno));
					return FAIL;
				}
				
					if(strncmp(buffer, "OP_OK", 5) == 0)
					{
						printf("L'ench�re a �t� annul� avec succ�s !\n");
					}
					else if(strncmp(buffer, "ERROR", 5) == 0)
					{
						printf("L'ench�re n'a pas p� �tre annul� ! \n");
						printf("%s", buffer);
					}
					else
						if(debug)printf("[ERROR] buffer: %s\n", buffer);
					break;
				
				case 2:
					printf("Envoi de la requ�te de supression de l'item: %d\n", item_n);
					clean_b(buffer);
					sprintf(buffer, "REQ_OP = DELETE ON %ld \n", item[item_n-1].uid);					
					send_socket(client, buffer); // Envoi de la requ�te
					
					if(rcv_socket(client, buffer) == -1)
					{
						fprintf(stderr,"[ERROR]: Erreur lors de la r�ception: %s\n", strerror(errno));
						return FAIL;
					}
					
						if(strncmp(buffer, "OP_OK", 5) == 0)
					{
						printf("L'item a �t� supprim� avec succ�s !\n");
					}
					else if(strncmp(buffer, "ERROR", 5) == 0)
					{
						printf("L'item n'a pas p� �tre supprim� ! \n");
						printf("%s", buffer);
					}
					else
						if(debug)printf("[ERROR] buffer: %s\n", buffer);
						
					printf("Retour au menu...\n");
					break;
					
			}
			
			break;
	}
}

}