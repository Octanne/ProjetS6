#include "player.h"

#include <stdlib.h>

void player_free(Player* player) {
    free(player);
}