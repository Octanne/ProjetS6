
#ifndef __GUI_PROCESS_H__
#define __GUI_PROCESS_H__

#include "net_struct.h"
#include "gui_struct.h"

void init_gui_process(GameInterface *gameI);

void update_game_gui(GameInterface *gameI, DataUpdateGame *data);

void write_text_info_bar(GameInterface *gameI, DataTextInfoGUI *data);

#endif // __GUI_PROCESS_H__

