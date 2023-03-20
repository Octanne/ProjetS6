
#include "menu_gui.h"

#include <locale.h>
#include <ncurses.h>
#include <string.h>

#include "utils.h"
#include "constants.h"
#include "gui_updater.h"

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
    switch (key) {
        case KEY_UP:
            // Write down the action
            set_text_info_gui("Action: UP", 1, GREEN_COLOR);
        break;
        
        case KEY_DOWN:
            // Write down the action
            set_text_info_gui("Action: DOWN", 1, GREEN_COLOR);
        break;

        case KEY_LEFT:
            // Write down the action
            set_text_info_gui("Action: LEFT", 1, GREEN_COLOR);
        break;

        case KEY_RIGHT:
            // Write down the action
            set_text_info_gui("Action: RIGHT", 1, GREEN_COLOR);
        break;

        case KEY_VALIDATE:
            // Write down the action
            set_text_info_gui("Action: VALIDATE", 1, GREEN_COLOR);
        break;

        default:
            // Write down the action
            set_text_info_gui("Action: UNKNOWN", 1, RED_COLOR);
        break;
    }
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

