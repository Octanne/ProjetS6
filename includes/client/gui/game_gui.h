
#ifndef __GAME_GUI_H__
#define __GAME_GUI_H__

#include "gui_struct.h"

void game_init_gui(GameInterface *gameI);
void game_stop_gui(GameInterface *gameI);

void game_gen_level(GameInterface *gameI);
void game_gen_player_menu(GameInterface *gameI);

void game_mouse_level_window(GameInterface *gameI, int x, int y);
void game_mouse_player_menu(GameInterface *gameI, int x, int y);
void game_keyboard_handler(GameInterface *gameI, int key);

void load_level(GameInterface *gameI, Level * newLevel);
void refresh_level(GameInterface *gameI);
void refresh_player_menu(GameInterface *gameI);

#endif

