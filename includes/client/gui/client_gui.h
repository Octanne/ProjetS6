
#ifndef __CLIENT_GUI_H__
#define __CLIENT_GUI_H__

#include "gui_struct.h"
/**
 * @brief Methode pour couper le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void stop_gui(GameInterface *gameI);
/**
 * @brief Methode pour initialiser le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void init_gui(GameInterface *gameI);

/**
 * @brief Methode pour generer le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void gen_main_gui(GameInterface *gameI);
/**
 * @brief Methode pour generer le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void gen_right_menu_gui(GameInterface *gameI);

/**
 * @brief Methode pour rafraichir le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void refresh_main_gui(GameInterface *gameI);
/**
 * @brief Methode pour rafraichir le gui (Parti commun a Menu et Jeu)
 * 
 * @param gameI 
 */
void refresh_right_menu_gui(GameInterface *gameI);

/**
 * @brief Methode pour afficher un message dans INFO
 * 
 * @param gameI 
 * @param text Le texte a afficher
 * @param line La ligne ou afficher le texte
 * @param color La couleur du texte
 */
void set_text_info_gui(GameInterface *gameI, const char *text, int line, int color);

/**
 * @brief Pour changer de gui (Menu vers Jeu ou Jeu vers Menu)
 * 
 * @param gameI 
 */
void switch_gui(GameInterface *gameI);

#endif

