
#ifndef __MENU_GUI_H__
#define __MENU_GUI_H__

#include "gui_struct.h"

/**
 * @brief Methode pour initialiser le gui du menu
 * 
 * @param gameI 
 */
void menu_init_gui(GameInterface *gameI);
/**
 * @brief Methode pour couper le gui du menu
 * 
 * @param gameI 
 */
void menu_stop_gui(GameInterface *gameI);

/**
 * @brief Methode pour generer le gui du menu
 * 
 * @param gameI 
 */
void menu_gen_main_window(GameInterface *gameI);
/**
 * @brief Methode pour generer le gui du menu (Partie droite)
 * 
 * @param gameI 
 */
void menu_gen_right_menu(GameInterface *gameI);

/**
 * @brief Methode pour gerer les evenements de la souris
 * 
 * @param gameI 
 * @param x 
 * @param y 
 */
void menu_mouse_main_window(GameInterface *gameI, int x, int y);
/** 
 * @brief Methode pour gerer les evenements de la souris (Partie droite)
 * 
 * @param gameI
 * @param x
 * @param y
*/
void menu_mouse_right_menu(GameInterface *gameI, int x, int y);

/**
 * @brief Methode pour gerer les evenements du clavier
 * 
 * @param gameI 
 * @param key 
 */
void menu_keyboard_handler(GameInterface *gameI, int key);

/**
 * @brief Methode pour rafraichir le gui du menu
 * 
 * @param gameI 
 */
void menu_refresh_main_window(GameInterface *gameI);
/**
 * @brief Methode pour rafraichir le gui du menu (Partie droite)
 * 
 * @param gameI 
 */
void menu_refresh_right_menu(GameInterface *gameI);

/** 
 * @brief Permets de changer de page dans le menu de partie
 * 
 * @param gameI
 * @param page La page a afficher
*/
void changerPagePartieMenu(GameInterface *gameI, int page);
/** 
 * @brief Permets de changer de page dans le menu de creation de partie
 * 
 * @param gameI
 * @param page La page a afficher
*/
void changerPageCreatePartie(GameInterface *gameI, int page);
/**
 * @brief Permet de passer de la page de creation de partie a la page de selection de partie
 * 
 * @param gameI 
 */
void switchBetweenCreateAndChoose(GameInterface *gameI);
/**
 * @brief Permet de se mettre en attente de la partie selectionnee
 * 
 * @param gameI 
 */
void waitForPartie(GameInterface *gameI);

#endif

