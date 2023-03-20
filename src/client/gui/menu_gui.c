
#include "menu_gui.h"

#include <locale.h>
#include <ncurses.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"

void menu_init_gui(GameInterface *gameI){
    menu_gen_main_window(gameI);
    menu_gen_right_menu(gameI);
}

void menu_stop_gui(GameInterface *gameI){

}

void menu_gen_main_window(GameInterface *gameI) {

}

void menu_gen_right_menu(GameInterface *gameI) {
    strcpy(gameI->menuInfo.player_name, "User 1");

    menu_refresh_right_menu(gameI);
}

void menu_mouse_main_window(GameInterface *gameI, int x, int y){

}

void menu_mouse_right_menu(GameInterface *gameI, int x, int y){

}

void menu_keyboard_handler(GameInterface *gameI, int key){
    switch (key) {
        case KEY_UP:
            // Write down the action
            set_text_info_gui(gameI, "Action: UP", 1, GREEN_COLOR);
        break;
        
        case KEY_DOWN:
            // Write down the action
            set_text_info_gui(gameI, "Action: DOWN", 1, GREEN_COLOR);
        break;

        case KEY_LEFT:
            // Write down the action
            set_text_info_gui(gameI, "Action: LEFT", 1, GREEN_COLOR);
        break;

        case KEY_RIGHT:
            // Write down the action
            set_text_info_gui(gameI, "Action: RIGHT", 1, GREEN_COLOR);
        break;

        case KEY_VALIDATE:
            // Write down the action
            set_text_info_gui(gameI, "Action: VALIDATE", 1, GREEN_COLOR);
        break;

        default:
            // Write down the action
            set_text_info_gui(gameI, "Action: UNKNOWN", 1, RED_COLOR);
        break;
    }
}

void menu_refresh_main_window(GameInterface *gameI){

}

void menu_refresh_right_menu(GameInterface *gameI){
    // Draw player infos
    // Draw Keys
    wattron(gameI->gui.winTOOLS, COLOR_PAIR(PURPLE_BLOCK));
    char centered_name[11];
    center_string(gameI->menuInfo.player_name, 11, centered_name);
    mvwprintw(gameI->gui.winTOOLS, 1, 1, "%11s", centered_name);
    
    wrefresh(gameI->gui.winTOOLS);
}

