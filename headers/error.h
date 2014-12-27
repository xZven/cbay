/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
/* Projet Cbay BALBIANI Lorrain - Manavai TEIKITUHAAHAA */
/*
 *
 *
 */
 // Erreur général
 
#define 0x999 "Internal Server Error"
 
// Codes d'erreur lors l'inscription d'un nouvel utilisateur

#define 0x000 "Unknown Error"
#define 0x001 "Login is not free"
#define 0x002 "Login too long"
#define 0x003 "Password too long"

// Codes d'erreur lors de la connexion d'un utilisateur

#define 0x010 "Unknown Error"
#define 0x011 "Login not found"
#define 0x012 "Bad Password"
#define 0x015 "You are actually Banned !"

// Codes d'erreur lors du choix du mode

#define 0x020 "Unknown Error"
#define 0x021 "You can not choose admin mode"
#define 0x022 "Bad mode: not existing (admin, sell, buy)
#define 0x025 "Erreur UID: Bad UID"


// Mode Administrateur

	//Erreurs liées au changement de mot de passe d'un utilisateur.

#define 0x040 "Unknown Error"	
#define 0x041 "You are not and Admin"
#define 0x042 "Can not modify password: User not found"
#define 0x043 "Can not modify password: User is an admin"
#define 0x044 "Can not modify password: New password is the same"

	//Erreurs liées à la suppression d'un utilisateur.
	
#define 0x051 "Can not delete user: User not found"
#define 0x052 "Can not delete user: User is an Admin"
#define 0x054 "Can not delete user: You are not an admin"
#define 0x055 "Can not delete user: Unknown Error"

// Mode Vendeur

#define 0x060 "Unknown Error"
#define 0x061 "Can not load your selled Items"
#deinfe 0x062 "Can not load your current bid"
#define 0x064 "Can not add new item"

// Mode Acheteur

#define 0x070 "Unknown Error"
#define 0x071 "Can not load category..."
#define 0x072 "Can not make a bid..."
#define 0x074 "Cant not load your buy history"
#define 0x075 "Bad price: Lower that current bid"


