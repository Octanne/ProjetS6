
#ifndef LISTE_H
#define LISTE_H

#include <stdbool.h>

typedef struct EltListe_t EltListe;
struct EltListe_t {
    void* elmt;
    char type;
    EltListe* suivant;
};

typedef struct {
    EltListe* tete;
    EltListe* queue;
    int taille;
    bool isQueue;
} Liste;

Liste liste_create(bool isQueue);
void liste_add(Liste* liste, void* elmt, char type);
void liste_remove(Liste* liste, void* elmt, int freeElmt);
void liste_free(Liste* liste, int freeElmt);

void liste_add_tete(Liste* liste, void* elmt, char type);
void liste_add_queue(Liste* liste, void* elmt, char type);

void* liste_get(Liste* liste, int index);
void* liste_pop(Liste* liste);

#endif