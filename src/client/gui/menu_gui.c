
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
    // Destroy TabPartie window 1 to 4
    int i = 0;
    for (i = 0; i < 4; i++) {
        delwin(gameI->menuInfo.tabPartie[i].winTAB);
        logs(L_INFO, "Menu | TabPartie %d deleted!", i);
    }
}

void menu_gen_main_window(GameInterface *gameI) {
    // Create TabPartie window 1 to 4
    int i = 0;
    for (i = 0; i < 4; i++) {
        // Mock data
        strcpy(gameI->menuInfo.tabPartie[i].name, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
        gameI->menuInfo.tabPartie[i].nbPlayers = 2;
        gameI->menuInfo.tabPartie[i].maxPlayers = 4;
        gameI->menuInfo.tabPartie[i].status = 0;


        gameI->menuInfo.tabPartie[i].winTAB = 
            derwin(gameI->gui.winMAIN, 5, MATRICE_LEVEL_X, 5*i, 0);
        wattron(gameI->menuInfo.tabPartie[i].winTAB, COLOR_PAIR(LBLUE_COLOR));
        box(gameI->menuInfo.tabPartie[i].winTAB, 0, 0);
        wattron(gameI->menuInfo.tabPartie[i].winTAB, COLOR_PAIR(LBLUE_BLOCK));
        // Information partie
        mvwprintw(gameI->menuInfo.tabPartie[i].winTAB, 1, 2, 
            " Nom de la partie: %36s ", 
            gameI->menuInfo.tabPartie[i].name);
        // Status partie
        if (gameI->menuInfo.tabPartie[i].status == 0) {
            mvwprintw(gameI->menuInfo.tabPartie[i].winTAB, 2, 2, 
            " Status: En attente                                     ");
        } else {
            mvwprintw(gameI->menuInfo.tabPartie[i].winTAB, 2, 2, 
            " Status: En cours                                       ");
        }
        mvwprintw(gameI->menuInfo.tabPartie[i].winTAB, 2, MATRICE_LEVEL_X-20, 
            " Joueur(s): %02i/%02i",
            gameI->menuInfo.tabPartie[i].nbPlayers, gameI->menuInfo.tabPartie[i].maxPlayers);

        // Bouton rejoindre
        wattron(gameI->menuInfo.tabPartie[i].winTAB, COLOR_PAIR(YELLOW_BLOCK));
        mvwprintw(gameI->menuInfo.tabPartie[i].winTAB, 4, MATRICE_LEVEL_X-12, " Rejoindre ");
        
        logs(L_INFO, "Menu | TabPartie %d created!", i);
        wrefresh(gameI->menuInfo.tabPartie[i].winTAB);
    }

    menu_refresh_main_window(gameI);
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
    // Draw partie disponible on title
    wattron(gameI->gui.cwinMAIN, COLOR_PAIR(LBLUE_COLOR));
    mvwprintw(gameI->gui.cwinMAIN, 0, 0, " Partie disponible ");

    // Draw the list of available games
    wattron(gameI->gui.cwinMAIN, COLOR_PAIR(COLOR_WHITE));

    wrefresh(gameI->gui.cwinMAIN);
    wrefresh(gameI->gui.winMAIN);
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

