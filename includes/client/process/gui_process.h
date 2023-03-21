
#ifndef __GUI_PROCESS_H__
#define __GUI_PROCESS_H__

#include "net_struct.h"
#include "gui_struct.h"

void init_gui_process(NetworkSocket *networkSocket);

void update_game_gui(GameInterface *gameI, DataUpdateGame *data);
void update_menu_gui(GameInterface *gameI, DataUpdateMenu *data);
void write_text_info_bar(GameInterface *gameI, DataTextInfoGUI *data);

#endif // __GUI_PROCESS_H__

