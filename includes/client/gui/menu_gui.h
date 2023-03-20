
#ifndef __MENU_GUI_H__
#define __MENU_GUI_H__

#include "gui_struct.h"

void menu_init_gui(GameInterface *gameI);
void menu_stop_gui(GameInterface *gameI);

void menu_gen_main_window(GameInterface *gameI);
void menu_gen_right_menu(GameInterface *gameI);

void menu_mouse_main_window(GameInterface *gameI, int x, int y);
void menu_mouse_right_menu(GameInterface *gameI, int x, int y);
void menu_keyboard_handler(GameInterface *gameI, int key);

void menu_refresh_main_window(GameInterface *gameI);
void menu_refresh_right_menu(GameInterface *gameI);

#endif

