
#ifndef __GUI_PROCESS_H__
#define __GUI_PROCESS_H__

void init_gui_process(NetworkSocket *networkSocket);

void update_game_gui(DataUpdateGame *data);
void update_menu_gui(DataUpdateMenu *data);
void write_text_info_bar(DataTextInfoGUI *data);

#endif // __GUI_PROCESS_H__

