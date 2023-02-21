
#include "liste.h"

#include <stdlib.h>

#include "../utils/utils.h"

/**
 * @brief Create a new list of objects.
 * 
 * @return ListeObjet* : The list of objects.
 */
ListeObjet* creerListeObjet() {
	ListeObjet* listeObjet = malloc(sizeof(ListeObjet));
	if (listeObjet == NULL) {
		logs(L_DEBUG, "creerListeObjet | ERROR malloc listeObjet");
		perror("Error while allocating memory in creerListeObjet\n");
		exit(EXIT_FAILURE);
	}
	listeObjet->tete = NULL;
	listeObjet->taille = 0;
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
	free(listeObjet);
}

/**
 * @brief Create a new list of integers.
 * 
 * @return ListeEntier* : The list of integers.
 */
ListeEntier* creerListeEntier() {
	ListeEntier* listeEntier = malloc(sizeof(ListeEntier));
	if (listeEntier == NULL) {
		logs(L_DEBUG, "creerListeEntier | ERROR malloc listeEntier");
		perror("Error while allocating memory in creerListeEntier\n");
		exit(EXIT_FAILURE);
	}
	listeEntier->tete = NULL;
	listeEntier->taille = 0;
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
	free(listeEntier);
}

