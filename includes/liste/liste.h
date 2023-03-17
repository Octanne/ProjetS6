
#ifndef LISTE_H
#define LISTE_H

#include "objet.h"
#include "player.h"

typedef struct EltListe_o_t EltListe_o;
struct EltListe_o_t {
    Objet* objet;
    EltListe_o* suivant;
};

typedef struct {
    EltListe_o* tete;
    int taille;
} ListeObjet;

ListeObjet creerListeObjet();
void listeAjouterObjet(ListeObjet* listeObjet, Objet* objet);
void listeSupprimerObjet(ListeObjet* listeObjet, Objet* objet, int freeObjet);
void listeObjet_free(ListeObjet* listeObjet, int freeObjet);

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

typedef struct EltListe_i_t EltListe_i;
struct EltListe_i_t {
    int entier;
    EltListe_i* suivant;
};

typedef struct {
    EltListe_i* tete;
    int taille;
} ListeEntier;

ListeEntier creerListeEntier();
void listeAjouterEntier(ListeEntier* listeEntier, int entier);
void listeSupprimerEntier(ListeEntier* listeEntier, int entier);
void listeEntier_free(ListeEntier* listeEntier);

////////////////////////////////////
////////////////////////////////////
////////////////////////////////////

typedef struct EltListe_p_t EltListe_p;
struct EltListe_p_t {
    Player* player;
    EltListe_p* suivant;
};

typedef struct {
    EltListe_p* tete;
    int taille;
} ListePlayer;

ListePlayer creerListePlayer();
void listeAjouterPlayer(ListePlayer* listePlayer, Player* player);
void listeSupprimerPlayer(ListePlayer* listePlayer, Player* player, int freePlayer);
void listePlayer_free(ListePlayer* listePlayer, int freePlayer);

#endif

