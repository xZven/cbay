/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
 /*
  * cette fonction est appel� pour le mode vendeur
  * il re�oit en param�tre une structure de type user_t et un
  * pointeur sur un buffer qui sera utilis� pour former les requ�tes
  * et recevoir les r�ponse du serveur.
  */
state seller_mode(struct user_t * client, char * buffer)
{
	int choix = -1;
	int choix_item = 0;
	int choix_op = 0;
	int index = 0;
	
	struct object_t vente[100];		// tableau des item vendu
	struct object_t bid[100];		// tableau des ventes en cours
	struct object_t new;			
	
	while(1) // boucle infinie
	{	
		do // choix fonctionnalit�
		{
			__fpurge(stdin);
			printf("Que souhaitez-vous faire? \n");
			printf("\t 1 - Consulter mon historique de vente \n ");
			printf("\t 2 - G�rer mes ventes en cours \n"); 
			printf("\t 3 - Effectuer une nouvelle vente \n");
			printf("\t 0 - Quitter le Programme \n");
			printf("Choix[1|2|3|0]: ");
		}while((scanf("%d", &choix) != 1 ) || choix > 3 || choix < 0);
		
		
		switch(choix) // selon le choix de la fonctionnalit�
		{
			case 0:
				exit(SUCCESS);
				break;
			
			case 1: // consulter l'Historique de d'achat de l'utilisateur
			{
				debugm("Consultation de l'historique d'achat");
				clean_b(buffer);
				sprintf(buffer, "REQ_HIST_ITEM_BOUGHT = %ld \n", client->uid); // formatage de la requ�te
				debugm(buffer);
				send_socket(client, buffer); //envoi de la requ�te
				clean_b(buffer);
				index = 0;
				do // affichage des item vendu
				{
					rcv_socket(client, buffer); // r�ception
					
					debugm("BUFFER:");
					debugm(buffer);
					if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						printf("\nFIN D'AFFICHAGE\n");
						break;
					}
					else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
					{
						echecm("Erreur re�u du serveur");
						printf(buffer);
						break;
					}
					else
					{
					
						if(sscanf(buffer, "ITEM = %ld + %51[A-Za-z0123456789 ] + %51[A-Za-z ] + %f + %f + %d \n",
							&vente[index].uid,
							vente[index].name,
							vente[index].category,
							&vente[index].start_price,
							&vente[index].final_price,
							&vente[index].quantity) != 6)
						{
							warm("sscanf(), historique des ventes");
						}
						
						
						
						printf("%d - \tUID:%ld\n\tNom: %s\n\tCat�gorie: %s\n\tPrix de d�part: %f Euro\n\tDernier ench�re: %f Euro\n\tQuantit�: %d\n\n", index + 1,
							bid[index].uid,
							bid[index].name,
							bid[index].category,
							bid[index].start_price,
							bid[index].final_price,
							bid[index].quantity);
						index++;
					}
						
				}while(1);
				break;
			}
			case 2: // g�rer les vente en cours /* une op�ration � finir */
			{
				debugm("Gestion des ventes en cours");
				clean_b(buffer);
				sprintf(buffer, "REQ_ITEM_USER = %ld \n", client->uid);
				debugm(buffer);
				send_socket(client, buffer); clean_b(buffer);
				index = 0;
				do // affichage des item
				{
					rcv_socket(client, buffer); // r�ception
					if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						printf("\nFIN D'AFFICHAGE\n");
						index--;
						break;
					}
					else if(strncmp("ERROR", buffer, 5) == 0) // si erreur 
					{
						echecm("Erreur re�u du serveur");
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
							warm("Un objet n'a pas pu �tre affich� correctement car il ne respecte pas le format de la RFC");
						}
						
						
						
						printf("%d - \tUID:%ld\n\tNom: %s\n\tCat�gorie: %s\n\tPrix de d�part: %f Euro\n\tDernier ench�re: %f Euro\n\tQuantit�: %d\n\n", index + 1,
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
					printf("Num�ro d'item [1 - %d]: ", index + 1);
				}while((scanf("%d", &choix_item) != 1) || (choix_item > index + 1) || (choix_item <= 0));
				
				do // choix de l'op�ration
				{
						__fpurge(stdin);
					printf("Que voules-vous faire ?\n");
					
					printf("1 - Supprimer l'ench�re \n");
					greenm("Permet de supprimer une ench�re en cours.\nL�objet sera supprim� du serveur.\n");
					
					printf("2 - Annuler l'ench�re \n");
					greenm("Permet d�annuler une ench�re.\nTous les compteurs de l�objet seront remis � z�ro.\n");
					
					printf("3 - Termin� une vente\n");
					greenm("Permet de mettre fin � une vente.\nLe dernier � avoir ench�rit l'objet sera l'acheteur de cet l'objet.\n");
					
					printf("0 - Quitter cette fonctionnalit�e \n");
					
					printf("Choix[1|2|3|0]: ");				
				}while((scanf("%d", &choix_op) != 1) || (choix_op > 3) || (choix_op < 0));
				
				debugm("Op�ration sur ITEM");
				switch(choix_op) // Op�ration sur l'item
				{
					case 0:
						break;
					case 1: // suppresion de l'ench�re
					{
						debugm("Suppresion de l'ench�re");
						clean_b(buffer);
						sprintf(buffer, "REQ_OP = DELETE_BID ON %ld \n", bid[choix_item-1].uid);
						send_socket(client, buffer);
						debugm(buffer);
						rcv_socket(client, buffer); // r�ception
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'ench�re a bien �t� supprim�\n");		
						}
						else if(strncmp("ERROR", buffer, 5) == 0)
						{
							echecm("Erreur re�u du serveur");
							printf("%s", buffer);
						}
						else
						{
						}
						break;
					}
					case 2:	// annuler l'ecnh�re
					{
						clean_b(buffer);
						sprintf(buffer, "REQ_OP = CANCEL_BID ON %ld \n", bid[choix_item-1].uid);
						send_socket(client, buffer);
						rcv_socket(client, buffer); // r�ception
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'ench�re a bien �t� annul�\n");		
						}
						else if(strncmp("ERROR", buffer, 5) == 0)
						{
							echecm("Erreur re�u du serveur");
							printf("%s", buffer);
						}
						else
						{
						}
						break;
					}
					case 3: // termin� une vente
					{
					clean_b(buffer);
						sprintf(buffer, "REQ_OP = END_BID ON %ld \n", bid[choix_item-1].uid);
						rcv_socket(client, buffer); // r�ception
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							printf("L'ench�re a bien �t� termin�\n");		
						}
						else if(strncmp("ERROR", buffer, 5) == 0)
						{
							echecm("Erreur re�u du serveur");
							printf("%s", buffer);
						}
						else
						{
						}
						break;
					}	
					default: // erreur
						errorm("Choix des fonctionnalit�s du mode vendeur\Exiting...");
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
				printf("Cat�gorie: ");
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
				
					do // Quantit�
					{
						printf("Quantit�: ");
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
					new.place); // formatage de la requ�te
				
				debugm(buffer);
				send_socket(client, buffer); clean_b(buffer); // envoi
				rcv_socket(client, buffer); // r�ception de la r�ponse
				debugm("BUFFER:");
				debugm(buffer);
				if(strcmp("ITEM_ADDED \n", buffer) == 0) // l'objet a bien �t� ajout�
				{
					greenm("L'ench�re a bien �t� ajout�\n");		
				}
				else if(strncmp("ERROR", buffer, 5) == 0) // erreur re�u
				{
					echecm("Erreur re�u du serveur");
					printf(buffer);
				}
				else
				{
				}
				break;
			}	
			
			default: // cas impossible mais pr�vu
				errorm("Choix des fonctionnalit�s du mode vendeur\Exiting...");
				exit(FAIL);
		}
	}	
}