/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */

state buyer_mode(struct user_t * client, char * buffer)
{
	int choix = -1;
	int index;
	int choix_item = 0;
	int choix_op = 0;
	int choix_cat = 0;
	float enchere = 0;
	
	float prix = 0;
	
	time_t time_exp;
	
	struct object_t achat[100];		// tableau des item achat�
	struct object_t bid[100];		// tableau des ventes en cours
	struct object_t new_bid[100];	// tableau pour les nouveaux ench�re lors de l'acc�s � une cat�gorie
	
	char category[50][50];			// tableau de string 50 lignes de 50c max
	
	
while(1)
{	
	do // choix de fonctionnalit�e
	{
		__fpurge(stdin);
		printf("Que souhaitez-vous faire? \n");
		printf("\t 1 - Consulter mon historique de d'achat \n ");
		printf("\t 2 - G�rer mes ench�res en cours \n"); 
		printf("\t 3 - Effectuer un nouvel ench�re \n");
		printf("\t 0 - Quitter le Programme \n");
		printf("Choix[1|2|3|0]: ");
	}while((scanf("%d", &choix) != 1 ) || choix > 3  || choix < 0);
	
	switch(choix)
	{
		case 0: // Quitter le programme
			exit(SUCCESS);
			break;
		
		case 1: // Consulter l'historique des achats
		{
			debugm("Consultation de l'historique d'achat");
			clean_b(buffer);
			sprintf(buffer, "REQ_HIST_ITEM_BOUGHT BY %ld \n", client->uid);
			debugm(buffer);
			send_socket(client, buffer); 
			clean_b(buffer);
			index = 0;
			
			do // affichage des item vendu
			{
				rcv_socket(client, buffer);  // r�ception				
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
					printf("%s", buffer);
					break;
				}
				else
				{
				
					if(sscanf(buffer, "ITEM = %ld + %51[^+] + %51[^+] + %f + %f + %d \n",
							&achat[index].uid,
							achat[index].name,
							achat[index].category,
							&achat[index].start_price,
							&achat[index].final_price,
							&achat[index].quantity) != 6)
						{
							warm("sscanf(), historique des ventes");
						}
						
						
						
						printf("%d - \tUID:%ld\n\tNom: %s\n\tCat�gorie: %s\n\tPrix de d�part: %f Euro\n\tDernier ench�re: %f Euro\n\tQuantit�: %d\n\n", index + 1,
							achat[index].uid,
							achat[index].name,
							achat[index].category,
							achat[index].start_price,
							achat[index].final_price,
							achat[index].quantity);
						index++;
				}
					
			}while(1);
		}	
			break;
			
		case 2: // G�rer mes ench�res en cours
		{
			debugm("Gestion des ench�res en cours");
			clean_b(buffer);
			sprintf(buffer, "REQ_BID_USER = %ld \n", client->uid);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			index = 0;
			
				do // affichage des item achet�
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
						printf("%s", buffer);
						break;
					}
					else
					{
					if(sscanf(buffer, "ITEM = %ld + %51[A-Za-z0123456789 ] + %51[A-Za-z ] + %f + %f + %d \n",
							&bid[index].uid,
							bid[index].name,
							bid[index].category,
							&bid[index].start_price,
							&bid[index].final_price,
							&bid[index].quantity) != 6)
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
				}while(index <= 100);
				
				do // choix de l'item
				{
					__fpurge(stdin);
					printf("Num�ro d'item [1 - %d]: ", index + 1);
				}while((scanf("%d", &choix_item) != 1) || (choix_item > index + 1) || (choix_item <= 0));
				
				do // choix de l'op�ration
				{
						__fpurge(stdin);
					printf("Que voules-vous faire ?\n");
					
					printf("1 - Augmenter l'ench�re \n");
					greenm("\nPermet d�augmenter une ench�re.\nCette op�ration est doit �tre suivi d�un prix sup�rieur � la mise en cours.");
					
					printf("2 - Annuler l'ench�re \n");
					greenm("\nPermet d�annuler une ench�re.\n");
					
					printf("0 - Quitter cette fonctionnalit�e \n");
					
					printf("Choix[1|2|0]: ");				
				}while((scanf("%d", &choix_op) != 1) || (choix_op > 2) || (choix_op < 0));
				
				switch(choix_op) // Op�ration sur l'item
				{
					case 0:
						break;
					case 1: // suppresion de l'ench�re
					{
						do
						{
							__fpurge(stdin);
							printf("Prix: ");							
						}while(scanf("%f", &prix) != 1);
						clean_b(buffer);
						sprintf(buffer, "REQ_ON %ld = UP_BID + %f \n", bid[choix_item-1].uid, prix); 
						send_socket(client, buffer);
						clean_b(buffer);
						if(rcv_socket(client, buffer) == -1) // r�ception
						{
							errorm("Erreur lors de la r�ception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							greenm("L'ench�re a bien �t� supprim�\n");		
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
						prix = -1;
						clean_b(buffer);
						sprintf(buffer, "REQ_ON %ld = CANCEL_BID + %f \n", bid[choix_item-1].uid, prix); 
						send_socket(client, buffer);
						clean_b(buffer);
						if(rcv_socket(client, buffer) == -1) // r�ception
						{
							errorm("Erreur lors de la r�ception");
							return FAIL;
						}
						debugm("BUFFER:");
						debugm(buffer);
						if(strncmp("OP_OK", buffer,5) == 0) // s
						{
							greenm("L'ench�re a bien �t� annul�");		
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
		case 3: // Effectuer un nouvel ench�re
		{
			debugm("Gestion des ench�res en cours");
			clean_b(buffer);
			sprintf(buffer, "REQ_CAT \n");
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			
			index = 0;
			do // affichage des cat�gories
				{
					rcv_socket(client, buffer); // r�ception
					
					if(strcmp("END_CATEGORY \n", buffer) == 0) // si c'est la fin de l'affichage
					{
						index--;
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
					
						sscanf(buffer, "CATEGORY = %s \n", category[index]);
						printf("%d - %s\n", index + 1,category[index]);							
						index++;
					}
				}while(index <= 50);
			
			do // choix de la cat�gorie
				{
					__fpurge(stdin);
					printf("Num�ro de cat�gorie [1 - %d] (0 to leave): ", index + 1);
				}while((scanf("%d", &choix_cat) != 1) || (choix_cat > index + 1) || (choix_cat < 0));
				
			if(choix_cat == 0)
				break; // on quitte la fonctionnalit�e
			
			clean_b(buffer);
			sprintf(buffer, "REQ_CAT_ACCESS = %s \n", category[choix_cat - 1]);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			
			index = 0;
			do // affichage des item de la cat�gorie
				{
					rcv_socket(client, buffer); // r�ception
					
					if(strcmp("END_ITEM \n", buffer) == 0) // si c'est la fin de l'affichage on quitte la boucle
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
					if(sscanf(buffer, "ITEM = %ld + %51[^+] + %51[^+] + %f + %f + %d \n",
							&new_bid[index].uid,
							new_bid[index].name,
							new_bid[index].category,
							&new_bid[index].start_price,
							&new_bid[index].temp_price,
							&new_bid[index].quantity) != 6)
						{
							warm("sscanf(), historique des ventes");
						}
						
						
						
						printf("%d - \tUID:%ld\n\tNom: %s\n\tCat�gorie: %s\n\tPrix de d�part: %f Euro\n\tDernier ench�re: %f Euro\n\tQuantit�: %d\n\n", index + 1,
							new_bid[index].uid,
							new_bid[index].name,
							new_bid[index].category,
							new_bid[index].start_price,
							new_bid[index].temp_price,
							new_bid[index].quantity);
						index++;
					}
				}while(index <= 100);

			do // choix de l'item
			{
				__fpurge(stdin);
				greenm("Choisissez l'Objet dont vous souhaitez avoir plus d'information.");
				printf("Num�ro d'item [1 - %d] (0 to leave): ", index + 1);
			}while((scanf("%d", &choix_item) != 1) || (choix_item > index + 1) || (choix_item < 0));
			
			if(choix_item == 0)
				break; // on quitte la fonctionnalit�e
		
			clean_b(buffer);
			sprintf(buffer, "REQ_ITEM = %ld \n", new_bid[choix_cat - 1].uid);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			
			rcv_socket(client, buffer); // r�ception
			
			if(sscanf(buffer, "ITEM = %ld + %1024[^+] + %99[^+] + %99[^+] + %ld \n",
							&new_bid[index-1].uid,
							new_bid[index-1].description,
							new_bid[index-1].url_image,
							new_bid[index-1].place,
							&new_bid[index-1].expire_time) != 5 )
			{
				warm("Erreur lors de la r�ception de l'item");
				printf(buffer);
				break;
				
			}
			else if(strncmp("ERROR", buffer, 5) == 0)
			{
				echecm("Erreur re�u du serveur");
				printf(buffer);
				break;
			}
			else // tout est bon, alors on affiche l'item en d�taille,
			{
				time(&time_exp);
				 printf("\tUID:%ld\n\tNom: %s\n\tCat�gorie: %s\n\tDescription: %s\n\tURL IMAGE: %s\n\tPrix de d�part: %f Euro\n\tDernier ench�re: %f Euro\n\tQuantit�: %d\n \tAdresse du vendeur: %s\n\tTemps avant expiration de l'ench�re [en min]: %ld \n",
							new_bid[index-1].uid,
							new_bid[index-1].name,
							new_bid[index-1].category,
							new_bid[index-1].description,
							new_bid[index-1].url_image,
							new_bid[index-1].start_price,
							new_bid[index-1].temp_price,
							new_bid[index-1].quantity,
							new_bid[index-1].place,
							(new_bid[index-1].expire_time - time_exp)/60);
			}
			

			do // montant de l'ench�re
			{
				__fpurge(stdin);
				greenm("\nVotre montant doit �tre sup�rieur � l'ench�re en cours ou au prix initial (s'il n'y pas d'ench�re en cours)");
				printf("Montant [en Euro]: ");
			}while((scanf("%f", &enchere) != 1) || enchere < new_bid[index-1].start_price || enchere < new_bid[index-1].temp_price); // ench�re inf�rieur au prix initial ou � l'ench�re en cours

			
			clean_b(buffer);
			sprintf(buffer, "REQ_BID_PRICE = %f FOR %ld \n", enchere, new_bid[index-1].uid);
			debugm(buffer);
			send_socket(client, buffer); clean_b(buffer);
			/**/
			
			rcv_socket(client, buffer); // r�ception
			if(strcmp("BID_OK \n", buffer) == 0) // OK !
			{
				greenm("L'ench�re a bien aboutit.\nVous pouvez maintenant voir l'objet dans vos ench�re en cours.");
			}
			else // r�ception d'une erreur
			{
				echecm("L'ench�re n'a pas aboutit");
				printf(buffer); 
			}
			break;
		}	
	}
}	
	return SUCCESS;

}