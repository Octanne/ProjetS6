
#include "menu_gui.h"

#include <locale.h>
#include <ncurses.h>
#include <string.h>

#include "utils.h"
#include "constants.h"
#include "gui_struct.h"

void menu_init_gui(){
    menu_gen_main_window();
    menu_gen_right_menu();
}

void menu_stop_gui(){

}

void menu_gen_main_window() {

}

void menu_gen_right_menu() {
    strcpy(gameInterface.menuInfo.player_name, "User 1");

    menu_refresh_right_menu();
}

void menu_mouse_main_window(int x, int y){

}

void menu_mouse_right_menu(int x, int y){

}

void menu_keyboard_handler(int key){

}

void menu_refresh_main_window(){

}

void menu_refresh_right_menu(){
    // Draw player infos
    // Draw Keys
    wattron(gameInterface.gui.winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
    char centered_name[11];
    center_string(gameInterface.menuInfo.player_name, 11, centered_name);
    mvwprintw(gameInterface.gui.winTOOLS, 1, 1, "%11s", centered_name);
    
    wrefresh(gameInterface.gui.winTOOLS);
}

