#include "level_edit.h"

#include <stdlib.h>
#include <stdio.h>

#include "../utils/utils.h"

// TODO revoir hitbox semble pas fonctionner correctement
int checkHitBox(Level* level, int x, int y, int xSize, int ySize) {
    if (x+xSize > 60 || y-ySize > 20) {
        logs(L_DEBUG, "checkHitBox => x: %d, y: %d (colision avec le bord)", x, y);
        return 0;
    }
    // calculate number of positions
    int nbPos = xSize * ySize;
    int nbIter = nbPos;

    Position posL[nbPos];
    int i, j;
    logs(L_DEBUG, "nbPos: %d", nbPos);
    // calculate all positions on the hitbox
    for (j = y; j > y-ySize; j--) {
        for (i = x; i < x+xSize; i++) {
            Position pos;
            pos.x = i;
            pos.y = j;
            nbIter--;
            posL[nbIter] = pos;
            logs(L_DEBUG, "[%d] add => x: %d, y: %d", nbIter, pos.x, pos.y);
        }
    }

    // check if all positions are free
    for (i = 0; i < nbPos; i++) {
        Position pos = posL[i];
        // check if in level
        if (pos.x < 0 || pos.x > 60 || pos.y < 0 || pos.y > 20) {
            logs(L_DEBUG, "read => x: %d, y: %d (colision avec le bord)", pos.x, pos.y);
            return 0;
        }

        ListeObjet* objs = rechercherObjet(level, pos.x, pos.y);
        if (objs->tete != NULL) {
            logs(L_DEBUG, "read => x: %d, y: %d (colision avec %d)", pos.x, pos.y, objs->tete->objet->type);
            listeObjet_free(objs, 0);
            return 0;
        }
        listeObjet_free(objs, 0);
        logs(L_DEBUG, "read => x: %d, y: %d (pas de colision)", pos.x, pos.y);
    }

    return 1;
}

int supprimerObjet(Level* level, int x, int y) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete != NULL) {
        levelSupprimerObjet(level, objs->tete->objet);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}

int poserBlock(Level* level, int x, int y) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete == NULL) {
        Objet* bloc = creerBlock(x,y);
        levelAjouterObjet(level,bloc);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}
int poserVie(Level* level, int x, int y) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete == NULL) {
        Objet* vie = creerVie(x,y);
        levelAjouterObjet(level,vie);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}
int poserBomb(Level* level, int x, int y) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete == NULL) {
        Objet* bomb = creerBomb(x,y);
        levelAjouterObjet(level,bomb);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}
int poserTrap(Level* level, int x, int y) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete == NULL) {
        Objet* trap = creerTrap(x,y);
        levelAjouterObjet(level,trap);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}
int poserGate(Level* level, int x, int y, int gateColor) {
    ListeObjet* objs = rechercherObjet(level, x, y);
    if (objs->tete == NULL) {
        Objet* gate = creerGate(x,y,gateColor);
        levelAjouterObjet(level,gate);
        listeObjet_free(objs, 0);
        return 1;
    } else {
        listeObjet_free(objs, 0);
        return 0;
    }
}

int poserKey(Level* level, int x, int y, int keyColor  ) {
    if (checkHitBox(level, x, y, 1, 2)) {
        Objet* obj = creerKey(x,y,keyColor);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserDoor(Level* level, int x, int y, int doorNumber) {
        if (checkHitBox(level, x, y, 3, 4)) {
        Objet* obj = creerDoor(x,y,doorNumber);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserExit(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 3, 4)) {
        Objet* obj = creerExit(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserStart(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 3, 4)) {
        Objet* obj = creerStart(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserProbe(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 3, 2)) {
        Objet* obj = creerProbe(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserRobot(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 4, 3)) {
        Objet* obj = creerRobot(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserLadder(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 3, 1)) {
        Objet* obj = creerLadder(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}
int poserPlayer(Level* level, int x, int y) {
    if (checkHitBox(level, x, y, 3, 3)) {
        Objet* obj = creerPlayer(x,y);
        levelAjouterObjet(level,obj);
        return 1;
    }

    return 0;
}