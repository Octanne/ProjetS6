#include "liste.h"

#include <stdlib.h>

ListeObjet* creerListeObjet() {
    ListeObjet* listeObjet = malloc(sizeof(ListeObjet));
    listeObjet->tete = (EltListe_o*) NULL;
    listeObjet->taille = 0;
    return listeObjet;
}

void listeAjouterObjet(ListeObjet* listeObjet, Objet* objet) {
    EltListe_o* eltListe = malloc(sizeof(EltListe_o));
    eltListe->objet = objet;
    if (listeObjet->tete != NULL) eltListe->suivant = listeObjet->tete;
    else eltListe->suivant = NULL;

    listeObjet->tete = eltListe;
    listeObjet->taille++;
}

void listeSupprimerObjet(ListeObjet* listeObjet, Objet* objet, int freeObjet) {
    EltListe_o* eltListe = listeObjet->tete;
    EltListe_o* precedent = NULL;
    while (eltListe != NULL) {
        if (eltListe->objet == objet) {
            if (precedent == NULL) {
                listeObjet->tete = eltListe->suivant;
            } else {
                precedent->suivant = eltListe->suivant;
            }
            if (freeObjet) objet_free(eltListe->objet);
            free(eltListe);
            listeObjet->taille--;
            return;
        }
        precedent = eltListe;
        eltListe = eltListe->suivant;
    }
}

void listeObjet_free(ListeObjet* listeObjet, int freeObjet) {
    EltListe_o* eltListe = listeObjet->tete;
    while (eltListe != NULL) {
        if(freeObjet) objet_free(eltListe->objet);
        EltListe_o* tmp = eltListe;
        if (eltListe->suivant != NULL) eltListe = eltListe->suivant;
        else eltListe = NULL;
        free(tmp);
    }
    free(listeObjet);
}

ListeEntier* creerListeEntier() {
    ListeEntier* listeEntier = malloc(sizeof(ListeEntier));
    listeEntier->tete = (EltListe_i*)NULL;
    listeEntier->taille = 0;
    return listeEntier;
}

void listeAjouterEntier(ListeEntier* listeEntier, int entier) {
    EltListe_i* eltListe = malloc(sizeof(EltListe_i));
    eltListe->entier = entier;

    if (listeEntier->tete == NULL) eltListe->suivant = NULL;
    else eltListe->suivant = listeEntier->tete;

    listeEntier->tete = eltListe;
    listeEntier->taille++;
}

void listeSupprimerEntier(ListeEntier* listeEntier, int entier) {
    EltListe_i* eltListe = listeEntier->tete;
    EltListe_i* precedent = NULL;
    while (eltListe != NULL) {
        if (eltListe->entier == entier) {
            if (precedent == NULL) {
                listeEntier->tete = eltListe->suivant;
            } else {
                precedent->suivant = eltListe->suivant;
            }
            free(eltListe);
            listeEntier->taille--;
            return;
        }
        precedent = eltListe;
        eltListe = eltListe->suivant;
    }
}

void listeEntier_free(ListeEntier* listeEntier) {
    EltListe_i* eltListe = listeEntier->tete;
    while (eltListe != NULL) {
        EltListe_i* tmp = eltListe;
        if (eltListe->suivant != NULL) eltListe = eltListe->suivant;
        else eltListe = NULL;
        free(tmp);
    }
    free(listeEntier);
}