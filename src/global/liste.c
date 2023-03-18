
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
Liste liste_create() {
	Liste liste;
	liste.tete = NULL;
	liste.taille = 0;
	return liste;
}

/**
 * @brief Add a pointer to the list.
 * 
 * @param liste : The list of pointers.
 * @param elmt : The pointer to add.
 */
void liste_add(Liste* liste, void* elmt, char type) {
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
			
			// Free the pointer if needed.
			if (freeElmt) {
				switch(eltListe->type) {
					case TYPE_OBJET:
						objet_free(eltListe->elmt);
						break;
					case TYPE_PLAYER:
						player_free(eltListe->elmt);
						break;
					default:
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
				default:
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