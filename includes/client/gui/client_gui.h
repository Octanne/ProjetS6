
#ifndef __CLIENT_GUI_H__
#define __CLIENT_GUI_H__

#include "gui_struct.h"

void stop_gui(GameInterface *gameI);
void init_gui(GameInterface *gameI);

void gen_main_gui(GameInterface *gameI);
void gen_right_menu_gui(GameInterface *gameI);

void refresh_main_gui(GameInterface *gameI);
void refresh_right_menu_gui(GameInterface *gameI);

void set_text_info_gui(GameInterface *gameI, const char *text, int line, int color);

void switch_gui(GameInterface *gameI);

#endif

