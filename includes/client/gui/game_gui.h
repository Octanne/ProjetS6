
#ifndef __GAME_GUI_H__
#define __GAME_GUI_H__

void game_init_gui();
void game_stop_gui();

void game_mouse_level_window(int x, int y);
void game_mouse_player_menu(int x, int y);
void game_keyboard_handler(int key);

void refresh_level();
void refresh_player_menu();

#endif

