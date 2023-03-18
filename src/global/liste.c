
#include "liste.h"
#include "utils.h"

#include <stdlib.h>


/**
 * @brief Create a new list of objects.
 * 
 * @return ListeObjet : The list of objects.
 */
ListeObjet creerListeObjet() {
	ListeObjet listeObjet;
	listeObjet.tete = NULL;
	listeObjet.taille = 0;
	return listeObjet;
}

/**
 * @brief Add an object to a list of objects.
 * 
 * @param listeObjet : The list of objects.
 * @param objet : The object to add.
 */
void listeAjouterObjet(ListeObjet* listeObjet, Objet* objet) {
	EltListe_o* eltListe = malloc(sizeof(EltListe_o));
	if (eltListe == NULL) {
		logs(L_DEBUG, "listeAjouterObjet | ERROR malloc eltListe");
		perror("Error while allocating memory in listeAjouterObjet\n");
		exit(EXIT_FAILURE);
	}
	eltListe->objet = objet;
	eltListe->suivant = listeObjet->tete;

	// Add the element to the list.
	listeObjet->tete = eltListe;
	listeObjet->taille++;
}

/**
 * @brief Remove an object from a list of objects.
 * 
 * @param listeObjet : The list of objects.
 * @param objet : The object to remove.
 * @param freeObjet : If the object should be freed.
 */
void listeSupprimerObjet(ListeObjet* listeObjet, Objet* objet, int freeObjet) {
	EltListe_o* eltListe = listeObjet->tete;
	EltListe_o* precedent = NULL;

	// Search for the object in the list.
	while (eltListe != NULL) {
		if (eltListe->objet == objet) {
			if (precedent == NULL)
				listeObjet->tete = eltListe->suivant;
			else
				precedent->suivant = eltListe->suivant;
			
			// Free the object if needed.
			if (freeObjet)
				objet_free(eltListe->objet);
			
			// Free the element and update the list size.
			free(eltListe);
			listeObjet->taille--;
			return;
		}

		// Update the precedent element and the current element to continue the search.
		precedent = eltListe;
		eltListe = eltListe->suivant;
	}
}

/**
 * @brief Free a list of objects.
 * 
 * @param listeObjet : The list of objects.
 * @param freeObjet : If the objects should be freed.
 */
void listeObjet_free(ListeObjet* listeObjet, int freeObjet) {
	EltListe_o* eltListe = listeObjet->tete;

	// Free the objects and the elements. If freeObjet is false, only the elements are freed.
	if (freeObjet) {
		while (eltListe != NULL) {
			objet_free(eltListe->objet);
			EltListe_o* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
	else {
		while (eltListe != NULL) {
			EltListe_o* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
}

/**
 * @brief Create a new list of integers.
 * 
 * @return ListeEntier : The list of integers.
 */
ListeEntier creerListeEntier() {
	ListeEntier listeEntier;
	listeEntier.tete = NULL;
	listeEntier.taille = 0;
	return listeEntier;
}

/**
 * @brief Add an integer to a list of integers.
 * 
 * @param listeEntier : The list of integers.
 * @param entier : The integer to add.
 */
void listeAjouterEntier(ListeEntier* listeEntier, int entier) {
	EltListe_i* eltListe = malloc(sizeof(EltListe_i));
	if (eltListe == NULL) {
		logs(L_DEBUG, "listeAjouterEntier | ERROR malloc eltListe");
		perror("Error while allocating memory in listeAjouterEntier\n");
		exit(EXIT_FAILURE);
	}
	eltListe->entier = entier;
	eltListe->suivant = listeEntier->tete;

	// Add the element to the list.
	listeEntier->tete = eltListe;
	listeEntier->taille++;
}

/**
 * @brief Remove an integer from a list of integers.
 * 
 * @param listeEntier : The list of integers.
 * @param entier : The integer to remove.
 */
void listeSupprimerEntier(ListeEntier* listeEntier, int entier) {
	EltListe_i* eltListe = listeEntier->tete;
	EltListe_i* precedent = NULL;

	// Search for the object in the list.
	while (eltListe != NULL) {
		if (eltListe->entier == entier) {
			if (precedent == NULL)
				listeEntier->tete = eltListe->suivant;
			else
				precedent->suivant = eltListe->suivant;
						
			// Free the element and update the list size.
			free(eltListe);
			listeEntier->taille--;
			return;
		}

		// Update the precedent element and the current element to continue the search.
		precedent = eltListe;
		eltListe = eltListe->suivant;
	}
}

/**
 * @brief Free a list of integers.
 * 
 * @param listeEntier : The list of integers.
 */
void listeEntier_free(ListeEntier* listeEntier) {
	EltListe_i* eltListe = listeEntier->tete;

	while (eltListe != NULL) {
		EltListe_i* tmp = eltListe;
		eltListe = eltListe->suivant;
		free(tmp);
	}
}

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

/**
 * @brief Create a new list of players.
 * 
 * @return ListePlayer : The list of players.
 */
ListePlayer creerListePlayer() {
	ListePlayer listePlayer;
	listePlayer.tete = NULL;
	listePlayer.taille = 0;
	return listePlayer;
}

/**
 * @brief Add an object to a list of players.
 * 
 * @param listePlayer : The list of players.
 * @param player : The object to add.
 */
void listeAjouterPlayer(ListePlayer* listePlayer, Player* player) {
	EltListe_p* eltListe = malloc(sizeof(EltListe_p));
	if (eltListe == NULL) {
		logs(L_DEBUG, "listeAjouterPlayer | ERROR malloc eltListe");
		perror("Error while allocating memory in listeAjouterPlayer\n");
		exit(EXIT_FAILURE);
	}
	eltListe->player = player;
	eltListe->suivant = listePlayer->tete;

	// Add the element to the list.
	listePlayer->tete = eltListe;
	listePlayer->taille++;
}

/**
 * @brief Remove an object from a list of players.
 * 
 * @param listePlayer : The list of players.
 * @param player : The object to remove.
 * @param freePlayer : If the object should be freed.
 */
void listeSupprimerPlayer(ListePlayer* listePlayer, Player* player, int freePlayer) {
	EltListe_p* eltListe = listePlayer->tete;
	EltListe_p* precedent = NULL;

	// Search for the object in the list.
	while (eltListe != NULL) {
		if (eltListe->player == player) {
			if (precedent == NULL)
				listePlayer->tete = eltListe->suivant;
			else
				precedent->suivant = eltListe->suivant;
			
			// Free the object if needed.
			if (freePlayer)
				player_free(eltListe->player);
			
			// Free the element and update the list size.
			free(eltListe);
			listePlayer->taille--;
			return;
		}

		// Update the precedent element and the current element to continue the search.
		precedent = eltListe;
		eltListe = eltListe->suivant;
	}
}

/**
 * @brief Free a list of players.
 * 
 * @param listePlayer : The list of players.
 * @param freePlayer : If the players should be freed.
 */
void listePlayer_free(ListePlayer* listePlayer, int freePlayer) {
	EltListe_p* eltListe = listePlayer->tete;

	// Free the players and the elements. If freePlayer is false, only the elements are freed.
	if (freePlayer) {
		while (eltListe != NULL) {
			player_free(eltListe->player);
			EltListe_p* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
	else {
		while (eltListe != NULL) {
			EltListe_p* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
}