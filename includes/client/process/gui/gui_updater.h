
#ifndef __GUI_UPDATER_H__
#define __GUI_UPDATER_H__

#include "net_message.h"

int init_updater_gui();

void update_menu_gui(DataUpdateMenu *data);
void update_game_gui(DataUpdateGame *data);
void write_text_info_bar(DataTextInfoGUI *data);

#endif

