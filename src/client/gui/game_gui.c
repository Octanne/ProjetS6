
#include "game_gui.h"

#include <locale.h>
#include <stdlib.h>
#include <ncurses.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"
#include "client_network.h"

/**
 * @brief Function to clear the level and generate a new one
 */
void clear_level(GameInterface *gameI) {
	// Free level and generate a new one
    level_free(gameI->gameInfo.level);
    *(gameI->gameInfo.level) = levelCreer();

	// Logs and refresh level
    logs(L_INFO, "Main | Level cleared");
    logs(L_INFO, "Main | Level value : %X", gameI->gameInfo.level);
    refresh_level(gameI);
}

/**
 * @brief Function to load a level.
 */
void load_level(GameInterface *gameI, Level *newLevel) {
    level_free(gameI->gameInfo.level);
    gameI->gameInfo.level = newLevel;

    // Logs and refresh level
    logs(L_INFO, "Main | New level load : %d", newLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, gameI->gameInfo.level->listeObjet.taille);
    set_text_info_gui(gameI, "Level loaded", 1, GREEN_COLOR);

    refresh_level(gameI);
}

/**
 * @brief Refresh the level window from scratch.
 */
void refresh_level(GameInterface *gameI) {
    // Clear window
    werase(gameI->gui.winMAIN);
    // Update level matrice
    levelUpdateMatriceSprite(gameI->gameInfo.level);
	// Draw level from matrice
    short y, x;
    for (y = 0; y < MATRICE_LEVEL_Y; y++) {
        for (x = 0; x < MATRICE_LEVEL_X; x++) {
			// Get sprite data
            SpriteData spriteD = gameI->gameInfo.level->matriceSprite[y + x * MATRICE_LEVEL_Y];

			// Move cursor to sprite position
            wmove(gameI->gui.winMAIN, y, x);

			// Set color and draw sprite
            wattron(gameI->gui.winMAIN, COLOR_PAIR(spriteD.color));
            if (spriteD.specialChar)
				waddch(gameI->gui.winMAIN, spriteD.spSprite);
            else
				waddch(gameI->gui.winMAIN, spriteD.sprite);
            wattroff(gameI->gui.winMAIN, COLOR_PAIR(spriteD.color));
        }
    }

    // Refresh window
    wrefresh(gameI->gui.winMAIN);
}

/**
 * @brief Refresh the tools menu from scratch.
 */
void refresh_player_menu(GameInterface *gameI) {
    // Clear window
    werase(gameI->gui.winTOOLS);
	// Draw player infos
    // Draw Keys
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameI->gui.winTOOLS, 2, 2, "Keys");
    if (gameI->gameInfo.player.key1 == 1) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
        mvwaddch(gameI->gui.winTOOLS, 4, 2, ' ');
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(PURPLE_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 5, 2, ACS_LLCORNER);
    }
    if (gameI->gameInfo.player.key2 == 1) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwaddch(gameI->gui.winTOOLS, 4, 4, ' ');
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(GREEN_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 5, 4, ACS_LLCORNER);
    }
    if (gameI->gameInfo.player.key3 == 1) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
        mvwaddch(gameI->gui.winTOOLS, 4, 6, ' ');
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(YELLOW_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 5, 6, ACS_LLCORNER);
    }
    if (gameI->gameInfo.player.key4 == 1) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(LBLUE_BLOCK));
        mvwaddch(gameI->gui.winTOOLS, 4, 8, ' ');
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(LBLUE_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 5, 8, ACS_LLCORNER);
    }

    // Draw Lives
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameI->gui.winTOOLS, 7, 2, "Lives");
    int i;
    for (i = 0; i < gameI->gameInfo.player.life; i++) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 9, 2 + 2*i, 'V');
    }

    // Draw Bombs
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameI->gui.winTOOLS, 11, 2, "Bombs");
    for (i = 0; i < gameI->gameInfo.player.nbBombs; i++) {
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(RED_COLOR));
        mvwaddch(gameI->gui.winTOOLS, 13, 2 + 2*i, 'O');
    }

    // Draw Level
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(WHITE_COLOR));
    mvwprintw(gameI->gui.winTOOLS, 15, 2, "Level");
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
    mvwprintw(gameI->gui.winTOOLS, 17, 2, " %03i ", gameI->gameInfo.player.level);

	// Refresh window
    wattroff(gameI->gui.winTOOLS, COLOR_PAIR(RED_COLOR));
    wrefresh(gameI->gui.winTOOLS);
}

/**
 * @brief Generate the game editor window
 */
void gen_game_window(GameInterface *gameI) {
    // Clear window
    werase(gameI->gui.cwinMAIN);
    // Draw border
    box(gameI->gui.cwinMAIN, 0, 0);
    // Afficher le titre de la main window
    wmove(gameI->gui.cwinMAIN, 0, 0);
    wprintw(gameI->gui.cwinMAIN, " Level ");
    wrefresh(gameI->gui.cwinMAIN);
}

/**
 * @brief Generate the players menu
 */
void gen_player_menu(GameInterface *gameI) {
    gameI->gameInfo.player.life = 5;
    gameI->gameInfo.player.nbBombs = 5;

    gameI->gameInfo.player.key1 = 1;
    gameI->gameInfo.player.key2 = 1;
    gameI->gameInfo.player.key3 = 1;
    gameI->gameInfo.player.key4 = 1;

    gameI->gameInfo.player.level = 1;

    refresh_player_menu(gameI);
}

void game_init_gui(GameInterface *gameI) {
    gen_game_window(gameI);
    logs(L_INFO, "Main | Game Window created!");
    gen_player_menu(gameI);
    logs(L_INFO, "Main | Player info menu created!");

    // Init level
    gameI->gameInfo.level = malloc(sizeof(Level));
    *gameI->gameInfo.level = levelCreer();
    logs(L_INFO, "Main | Empty Level created!");
    refresh_level(gameI);
}

void game_stop_gui(GameInterface *gameI) {
	// Free the level
    level_free(gameI->gameInfo.level);
    free(gameI->gameInfo.level);
}

void game_mouse_level_window(GameInterface *gameI, int x, int y) {
    // if we want to use the mouse
}

void game_mouse_player_menu(GameInterface *gameI, int x, int y) {
    // if we want to use the mouse
}

void send_input_to_server(GameInterface *gameI, int key) {
    // Send the key to the server
    NetMessage msg;
    msg.type = TCP_REQ_INPUT_PLAYER;
    msg.dataInputPlayer.keyPress = key;

    send_tcp_message(&gameI->netSocket.tcpSocket, msg);
}

void game_keyboard_handler(GameInterface *gameI, int key) {
    switch (key) {
        case KEY_UP:
            set_text_info_gui(gameI, "Action: UP", 1, GREEN_COLOR);
            send_input_to_server(gameI, key);
        break;
        
        case KEY_DOWN:
            set_text_info_gui(gameI, "Action: DOWN", 1, GREEN_COLOR);
            send_input_to_server(gameI, key);
        break;

        case KEY_LEFT:
            set_text_info_gui(gameI, "Action: LEFT", 1, GREEN_COLOR);
            send_input_to_server(gameI, key);
        break;

        case KEY_RIGHT:
            set_text_info_gui(gameI, "Action: RIGHT", 1, GREEN_COLOR);
            send_input_to_server(gameI, key);
        break;

        case KEY_VALIDATE:
            set_text_info_gui(gameI, "Action: VALIDATE", 1, GREEN_COLOR);
            send_input_to_server(gameI, key);
        break;

        default:
            set_text_info_gui(gameI, "Action: UNKNOWN", 1, RED_COLOR);
            send_input_to_server(gameI, key);
        break;
    }
}

