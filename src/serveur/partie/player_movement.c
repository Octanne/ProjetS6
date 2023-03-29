
#include "player_movement.h"

#include <stdbool.h>

#include "level_update.h"

void player_movement(Player *player, Level *level, int newX, int newY) {
    bool canMoveX = true;
    bool canMoveY = true;
    bool hasLadder = false;
    bool hasBlockUnderFeet = false;

    // Check if the player is on a ladder
    Liste oldCollision = objectInHitBox(level, player->obj->x, player->obj->y, 3, 4);
    // Parcourir la liste
    if (oldCollision.tete != NULL) {
        EltListe* elt = oldCollision.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            if (obj->type == LADDER_ID) {
                hasLadder = true;
                break;
            }
            elt = elt->suivant;
        }
    }
    liste_free(&oldCollision,false);

    // Check if there is a collision in the new position
    Liste newCollision = objectInHitBox(level, newX, newY, 3, 4);
    // Parcourir la liste
    if (newCollision.tete != NULL) {
        EltListe* elt = newCollision.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            if (!obj->isActive) {
                elt = elt->suivant;
                continue;
            }

            if (obj->type == BLOCK_ID || obj->type == TRAP_ID) {
                canMoveX = false;
                canMoveY = false;
            } else if (obj->type == GATE_ID) {
                switch (obj->gate.numgate) {
                    case 0:
                        if (!player->key1) {
                            // Laisse le joueur passer
                            canMoveX = false;
                        }
                        break;
                    case 1:
                        if (!player->key2) {
                            // Laisse le joueur passer
                            canMoveX = false;
                        }
                        break;
                    case 2:
                        if (!player->key3) {
                            // Laisse le joueur passer
                            canMoveX = false;
                        }
                        break;
                    case 3:
                        if (!player->key4) {
                            // Laisse le joueur passer
                            canMoveX = false;
                        }
                        break;
                }
            } else if (obj->type == LADDER_ID) {
                // Check if the player is on the top of the ladder
                hasLadder = true;
            } else if (obj->type == KEY_ID) {
                switch (obj->key.numkey) {
                    case 0:
                        player->key1 = true;
                        break;
                    case 1:
                        player->key2 = true;
                        break;
                    case 2:
                        player->key3 = true;
                        break;
                    case 3:
                        player->key4 = true;
                        break;
                }
            } else if (obj->type == HEART_ID) {
                if (player->life < 5) {
                    player->life++;
                    obj->isActive = false;
                    obj->nbSecBeforeRespawn = 5;
                }
            } else if (obj->type == BOMB_ID) {
                if (player->nbBombs < 3) {
                    player->nbBombs++;
                    obj->isActive = false;
                    obj->nbSecBeforeRespawn = 5;
                }
            }
            elt = elt->suivant;
        }
    }

    // Check if there is a block under the new position
    Liste underPlr = objectInHitBox(level, newX, newY+1, 3, 1);
    // Parcourir la liste
    if (underPlr.tete != NULL) {
        EltListe* elt = underPlr.tete;
        while (elt != NULL) {
            Objet* obj = (Objet*)elt->elmt;
            if (obj->type == BLOCK_ID || obj->type == TRAP_ID || obj->type == LADDER_ID) {
                hasBlockUnderFeet = true;
                break;
            }
            elt = elt->suivant;
        }
    }
    liste_free(&underPlr, false);

    // Check if the player can move
    if (canMoveX && hasBlockUnderFeet) {
        player->obj->x = newX;
    }
    if (canMoveY && hasLadder) {
        player->obj->y = newY;
    }
}

/*
	// Check if the player is on a ladder
	bool canMoveY = false;
	bool canMoveX = false;
	Liste oldCollision = objectInHitBox(lvl, player->obj->x, player->obj->y, 3, 4);
	if (oldCollision.tete != NULL) {
		EltListe* elt = oldCollision.tete;
		while (elt != NULL) {
			Objet* obj = (Objet*)elt->elmt;
			if (obj->type == LADDER_ID) {
				canMoveY = true;
				printf("In ladder\n");
				break;
			}
			elt = elt->suivant;
		}
	}

	switch (input) {

		case KEY_RIGHT:
		case KEY_LEFT:
		case KEY_UP:
		case KEY_DOWN:

			// Get future position
			switch (input) {
				case KEY_RIGHT: x++; player->obj->player.orientation = RIGHT_ORIENTATION; break;
				case KEY_LEFT: x--; player->obj->player.orientation = LEFT_ORIENTATION; break;
				case KEY_UP: y--; break;
				case KEY_DOWN: y++; break;
			}

			// Check if there is a collision
			Liste newCollision = objectInHitBox(lvl, x, y, 3, 4);
			// printf type of collision
			EltListe* elt = newCollision.tete;
			if (elt == NULL) {
				// Check si il y a un block en dessous de la nouvelle position
				Liste underPlr = objectInHitBox(lvl, x, y+1, 3, 4);
				if (underPlr.tete != NULL) { // TODO Vérifier l'ensemble des objets pour trouver un block
					canMoveX = true;
				} else {
					canMoveX = false;
				}
				liste_free(&underPlr,0);
				printf("PartieManager | inputPartieTCP | No collision\n");
			}

			// Else, conditions on type
			else {
				printf("PartieManager | inputPartieTCP | Collision, %d items\n", newCollision.taille);
				canMoveX = true;
				while (elt != NULL) {
					Objet* obj = (Objet*)elt->elmt;
					printf("PartieManager | inputPartieTCP | Collision with %d at '%d %d'\n", obj->type, obj->x, obj->y);
					switch (obj->type) {
						case TRAP_ID: // TODO : See to Check if the player is on the trap not in it too
						case BLOCK_ID:
							// Do nothing
							printf("PartieManager | inputPartieTCP | Collision with block\n");
							canMoveX = false;
							canMoveY = false;
							break;

						case GATE_ID:
							switch (obj->gate.numgate) {
								case 0:
									if (!player->key1) {
										// Laisse le joueur passer
										canMoveX = false;
									}
									break;
								case 1:
									if (player->key2) {
										// Laisse le joueur passer
										canMoveX = false;
									}
									break;
								case 2:
									if (player->key3) {
										// Laisse le joueur passer
										canMoveX = false;
									}
									break;
								case 3:
									if (player->key4) {
										// Laisse le joueur passer
										canMoveX = false;
									}
									break;
							}
							break;

						case HEART_ID:
							// Give the heart to the player
							player->life++;
							if (player->life > 5)
								player->life = 5;
							// TODO : Remove the heart from the level and respawn after a while
							break;
						
						case BOMB_ID:
							// Give the bomb to the player
							player->nbBombs++;
							if (player->nbBombs > 3)
								player->nbBombs = 3;
							// TODO : Remove the bomb from the level and respawn after a while
							break;

						case LADDER_ID:
							// Axe where the player can move
							canMoveY = true;
							break;
						case START_ID:
						case PLAYER_ID:
						case ROBOT_ID:	// ROBOT thread will attack the player
						case PROBE_ID:	// PROBE thread will attack the player
							break;
						
						case KEY_ID:
							// Give the key to the player
							switch (obj->key.numkey) {
								case 0: player->key1 = true; break;
								case 1: player->key2 = true; break;
								case 2: player->key3 = true; break;
								case 3: player->key4 = true; break;
							}
							break;
						
						case DOOR_ID:
							// Teleport the player to the other door
							// TODO Find the other door
							break;

						case EXIT_ID:
							// Win the game
							// TODO
							break;
					}
					elt = elt->suivant;
				}
			}

			liste_free(&newCollision, 0);

			// Move the player
			if (canMoveY) {
				player->obj->y = y;
			}
			if (canMoveX) {
				player->obj->x = x;
			}
			break;
		
		case KEY_VALIDATE:
			// Drop a bomb at the player position
			if (player->nbBombs > 0) {
				// TODO
			}
			break;
	}
}
*/