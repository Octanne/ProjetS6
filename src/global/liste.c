
#include "liste.h"
#include "utils.h"
#include "constants.h"
#include "player.h"
#include "objet.h"

#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Create a new list of pointers.
 * 
 * @return Liste : The list of pointers.
 */
Liste liste_create(bool isQueue) {
	Liste liste;
	liste.tete = NULL;
	liste.taille = 0;
	liste.queue = NULL;
	liste.isQueue = isQueue;
	return liste;
}

/**
 * @brief Add a pointer to the list.
 * 
 * @param liste : The list of pointers.
 * @param elmt : The pointer to add.
 */
void liste_add(Liste* liste, void* elmt, char type) {
	if (liste->isQueue) {
		liste_add_queue(liste, elmt, type);
	} else {
		liste_add_tete(liste, elmt, type);
	}
}

void liste_add_tete(Liste* liste, void* elmt, char type) {
	EltListe* eltListe = malloc(sizeof(EltListe));
	if (eltListe == NULL) {
		logs(L_DEBUG, "liste_add | ERROR malloc eltListe");
		perror("Error while allocating memory in liste_add\n");
		exit(EXIT_FAILURE);
	}
	eltListe->elmt = elmt;
	eltListe->suivant = liste->tete;

	// Add the element to the list.
	liste->tete = eltListe;
	liste->taille++;

	// Update the queue if needed.
	if (liste->taille == 1)
		liste->queue = eltListe;
}

void liste_add_queue(Liste* liste, void* elmt, char type) {
	EltListe* eltListe = malloc(sizeof(EltListe));
	if (eltListe == NULL) {
		logs(L_DEBUG, "liste_add | ERROR malloc eltListe");
		perror("Error while allocating memory in liste_add\n");
		exit(EXIT_FAILURE);
	}
	eltListe->elmt = elmt;
	eltListe->suivant = NULL;

	// Add the element to the list.
	if (liste->taille == 0) {
		liste->tete = eltListe;
	} else {
		liste->queue->suivant = eltListe;
	}
	liste->taille++;

	// Update the queue if needed.
	liste->queue = eltListe;
}

/**
 * @brief Remove a pointer from the list.
 * 
 * @param liste : The list of pointers.
 * @param elmt : The pointer to remove.
 * @param freeElmt : If the pointer should be freed.
 */
void liste_remove(Liste* liste, void* elmt, int freeElmt) {
	EltListe* eltListe = liste->tete;
	EltListe* precedent = NULL;

	// Search for the pointer in the list.
	while (eltListe != NULL) {
		if (eltListe->elmt == elmt) {
			if (precedent == NULL)
				liste->tete = eltListe->suivant;
			else
				precedent->suivant = eltListe->suivant;

			// Update the queue if needed.
			if (liste->queue == eltListe)
				liste->queue = precedent;
			
			// Free the pointer if needed.
			if (freeElmt) {
				switch(eltListe->type) {
					case TYPE_OBJET:
						objet_free(eltListe->elmt);
						break;
					case TYPE_PLAYER:
						player_free(eltListe->elmt);
						break;
					case TYPE_NET_MESSAGE:
						free(eltListe->elmt);
						break;
					case TYPE_PARTIE_INFO:
						free(eltListe->elmt);
						break;
					case TYPE_MAP_INFO:
						free(eltListe->elmt);
						break;
					case TYPE_SOCKADDR_IN:
						free(eltListe->elmt);
						break;
					default: // TODO
						logs(L_INFO, "liste_free | ERROR type inconnu");
						break;
				}
			}
			
			// Free the element and update the list size.
			free(eltListe);
			liste->taille--;

			return;
		}

		// Update the precedent element and the current element to continue the search.
		precedent = eltListe;
		eltListe = eltListe->suivant;
	}
}

/**
 * @brief Free a list of pointers.
 * 
 * @param liste : The list of pointers.
 * @param freeElmt : If the pointers should be freed.
 */
void liste_free(Liste* liste, int freeElmt) {
	EltListe* eltListe = liste->tete;

	// Free the pointers and the elements. If freeElmt is false, only the elements are freed.
	if (freeElmt) {
		while (eltListe != NULL) {
			switch(eltListe->type) {
				case TYPE_OBJET:
					objet_free(eltListe->elmt);
					break;
				case TYPE_PLAYER:
					player_free(eltListe->elmt);
					break;
				case TYPE_NET_MESSAGE:
					free(eltListe->elmt);
					break;
				case TYPE_PARTIE_INFO:
					free(eltListe->elmt);
					break;
				case TYPE_MAP_INFO:
					free(eltListe->elmt);
					break;
				case TYPE_SOCKADDR_IN:
					free(eltListe->elmt);
					break;
				default: // TODO
					logs(L_INFO, "liste_free | ERROR type inconnu");
					break;
			}

			EltListe* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
	else {
		while (eltListe != NULL) {
			EltListe* tmp = eltListe;
			eltListe = eltListe->suivant;
			free(tmp);
		}
	}
}

void* liste_get(Liste* liste, int index) {
	EltListe* eltListe = liste->tete;
	int i = 0;

	// Search for the pointer in the list.
	while (eltListe != NULL) {
		if (i == index)
			return eltListe->elmt;
		
		// Update the precedent element and the current element to continue the search.
		eltListe = eltListe->suivant;
		i++;
	}

	return NULL;
}

void* liste_pop(Liste* liste) {
	// get from the queue
	EltListe* eltListe = liste->queue;
	void* elmt = eltListe->elmt;

	// remove from the list
	liste_remove(liste, elmt, 0);

	return elmt;
}