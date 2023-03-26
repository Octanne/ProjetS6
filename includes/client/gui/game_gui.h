
#ifndef __GAME_GUI_H__
#define __GAME_GUI_H__

#include "gui_struct.h"

/**
 * @brief Initialize le GUI du jeu
 * 
 * @param gameI 
 */
void game_init_gui(GameInterface *gameI);
/**
 * @brief Stop le GUI du jeu
 * 
 * @param gameI 
 */
void game_stop_gui(GameInterface *gameI);

/**
 * @brief Génère le niveau dans le GUI
 * 
 * @param gameI 
 */
void game_gen_level(GameInterface *gameI);
/**
 * @brief Génère le menu du joueur dans le GUI
 * 
 * @param gameI 
 */
void game_gen_player_menu(GameInterface *gameI);

/**
 * @brief Gère les évènements de la souris dans le GUI du jeu
 * 
 * @param gameI 
 * @param x 
 * @param y 
 */
void game_mouse_level_window(GameInterface *gameI, int x, int y);
/**
 * @brief Gère les évènements de la souris dans le GUI du menu du joueur
 * 
 * @param gameI 
 * @param x 
 * @param y 
 */
void game_mouse_player_menu(GameInterface *gameI, int x, int y);
/**
 * @brief Gère les évènements du clavier dans le GUI du jeu
 * 
 * @param gameI 
 * @param key 
 */
void game_keyboard_handler(GameInterface *gameI, int key);

/**
 * @brief Rafraichit le GUI du jeu, le Niveau
 * 
 * @param gameI 
 */
void load_level(GameInterface *gameI, Level * newLevel);
/**
 * @brief Rafraichit le GUI du jeu, le Niveau
 * 
 * @param gameI 
 */
void refresh_level(GameInterface *gameI);
/**
 * @brief Rafraichit le GUI du jeu, le menu du joueur
 * 
 * @param gameI 
 */
void refresh_player_menu(GameInterface *gameI);

#endif

