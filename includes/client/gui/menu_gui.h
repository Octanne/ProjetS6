
#ifndef __MENU_GUI_H__
#define __MENU_GUI_H__

void menu_init_gui();
void menu_stop_gui();

void menu_gen_main_window();
void menu_gen_right_menu();

void menu_mouse_main_window(int x, int y);
void menu_mouse_right_menu(int x, int y);
void menu_keyboard_handler(int key);

void menu_refresh_main_window();
void menu_refresh_right_menu();

#endif

