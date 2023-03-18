
#ifndef LISTE_H
#define LISTE_H

typedef struct EltListe_t EltListe;
struct EltListe_t {
    void* elmt;
    char type;
    EltListe* suivant;
};

typedef struct {
    EltListe* tete;
    int taille;
} Liste;

Liste liste_create();
void liste_add(Liste* liste, void* elmt, char type);
void liste_remove(Liste* liste, void* elmt, int freeElmt);
void liste_free(Liste* liste, int freeElmt);

#endif