
#ifndef __GUI_PROCESS_H__
#define __GUI_PROCESS_H__

#include "net_struct.h"
#include "gui_struct.h"

/**
 * @brief Initialise le processus GUI attente d'entrée utilisateur
 * 
 * @param networkSocket 
 */
void init_gui_process(GameInterface *gameI);

/**
 * @brief Fonction de traitement des données reçues pour mettre à jour l'interface graphique du jeu
 * 
 * @param data 
 */
void update_game_gui(GameInterface *gameI, DataUpdateGame *data);
/**
 * @brief Fonction de traitement des données reçues pour écrire dans la barre d'information
 * 
 * @param gameI 
 * @param data 
 */
void write_text_info_bar(GameInterface *gameI, DataTextInfoGUI *data);

#endif // __GUI_PROCESS_H__

