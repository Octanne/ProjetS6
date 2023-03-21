
#include "menu_gui.h"

#include <locale.h>
#include <ncurses.h>
#include <string.h>

#include "utils.h"
#include "constants.h"

#include "client_gui.h"
#include "client_network.h"

void menu_init_gui(GameInterface *gameI){
    strcpy(gameI->menuInfo.player_name, "User 1");
    gameI->menuInfo.tabPartieMenu.numPage = 1;
    gameI->menuInfo.createPartieMenu.numPage = 1;
    gameI->menuInfo.newPartie = false;
    gameI->menuInfo.tabPartieMenu.selPartie = 0;
    gameI->menuInfo.createPartieMenu.selMap = 0;
    gameI->menuInfo.createPartieMenu.maxPlayers = 2;

    menu_gen_main_window(gameI);
    menu_gen_right_menu(gameI);
}

void menu_stop_gui(GameInterface *gameI){
    if (gameI->menuInfo.newPartie) {
        // Destroy TabMap window 1 to 6
        int i = 0;
        for (i = 0; i < 6; i++) {
            delwin(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB);
            logs(L_INFO, "Menu | TabPartie %d deleted!", i);
        }
    } else {
        // Destroy TabPartie window 1 to 4
        int i = 0;
        for (i = 0; i < 4; i++) {
            delwin(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
            logs(L_INFO, "Menu | TabPartie %d deleted!", i);
        }
    }
}

void menu_gen_main_window(GameInterface *gameI) {
    int i = 0;
    if (gameI->menuInfo.newPartie) {
        // Draw maps disponible on title
        wattron(gameI->gui.cwinMAIN, COLOR_PAIR(LBLUE_COLOR));
        mvwprintw(gameI->gui.cwinMAIN, 0, 0, " Map(s) disponible(s) ");
        wrefresh(gameI->gui.cwinINFOS);

        // Create MapPartie window 1 to 6
        for (i = 0; i < 6; i++) {
            // Mock data
            strcpy(gameI->menuInfo.createPartieMenu.tabMap[i].name, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            gameI->menuInfo.createPartieMenu.tabMap[i].set = true;

            gameI->menuInfo.createPartieMenu.tabMap[i].winTAB = derwin(gameI->gui.winMAIN, 3, MATRICE_LEVEL_X, 1+3*i, 0);
            logs(L_INFO, "Menu | TabMap %d created!", i);
            wrefresh(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB);
        }
        gameI->menuInfo.createPartieMenu.nbMaps = 6;
    } else {
        // Draw parties disponible on title
        wattron(gameI->gui.cwinMAIN, COLOR_PAIR(LBLUE_COLOR));
        mvwprintw(gameI->gui.cwinMAIN, 0, 0, " Partie(s) disponible(s) ");
        wrefresh(gameI->gui.cwinINFOS);

        // Create TabPartie window 1 to 4
        for (i = 0; i < 4; i++) {
            // Mock data
            strcpy(gameI->menuInfo.tabPartieMenu.tabPartie[i].name, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
            gameI->menuInfo.tabPartieMenu.tabPartie[i].nbPlayers = 2;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].maxPlayers = 4;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].status = 0;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].set = true;

            gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB = derwin(gameI->gui.winMAIN, 5, MATRICE_LEVEL_X, 5*i, 0);
            logs(L_INFO, "Menu | TabPartie %d created!", i);
            wrefresh(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
        }
        gameI->menuInfo.tabPartieMenu.nbParties = 4;
    }


    menu_refresh_main_window(gameI);
}

void menu_gen_right_menu(GameInterface *gameI) {
    menu_refresh_right_menu(gameI);
}

void changerPagePartieMenu(GameInterface *gameI, int page){
    // TODO Network call
    NetMessage messageReq;
    messageReq.type = NET_REQ_PARTIE_LIST;
    messageReq.partieListMessage.numPage = page;
    NetMessage messageUDP = send_udp_message(&gameI->netSocket->udpSocket, &messageReq);

    if (messageUDP.type == NET_REQ_PARTIE_LIST) {
        // Si pas de partie sur la page on change pas de page
        if (messageUDP.partieListMessage.nbParties == 0) {
            set_text_info_gui(gameI, "Vous êtes sur la dernire page!", 0, RED_COLOR);
            return;
        }

        gameI->menuInfo.tabPartieMenu.numPage = messageUDP.partieListMessage.numPage;
        gameI->menuInfo.tabPartieMenu.nbParties = messageUDP.partieListMessage.nbParties;
        int i = 0;
        for (i = 0; i < 4; i++) {
            gameI->menuInfo.tabPartieMenu.tabPartie[i].set = false;
        }
        for (i = 0; i < messageUDP.partieListMessage.nbParties; i++) {
            strcpy(gameI->menuInfo.tabPartieMenu.tabPartie[i].name, messageUDP.partieListMessage.partieInfo[i].name);
            gameI->menuInfo.tabPartieMenu.tabPartie[i].nbPlayers = messageUDP.partieListMessage.partieInfo[i].nbPlayers;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].maxPlayers = messageUDP.partieListMessage.partieInfo[i].maxPlayers;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].status = messageUDP.partieListMessage.partieInfo[i].status;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].set = true;
        }
    } else {
        set_text_info_gui(gameI, "Erreur lors du changement de page!", 0, RED_COLOR);
    }

    menu_refresh_main_window(gameI);
}

void changerPageCreatePartie(GameInterface *gameI, int page){
    // TODO Network call
    menu_refresh_main_window(gameI);
    set_text_info_gui(gameI, "Page changee avec succes!", 0, YELLOW_COLOR);
}

void rejoindrePartie(GameInterface *gameI){
    // TODO Network call

    set_text_info_gui(gameI, "Partie rejointe avec succes!", 0, YELLOW_COLOR);
}

void createPartie(GameInterface *gameI){
    // TODO Network call
    set_text_info_gui(gameI, "Partie creee avec succes!", 0, YELLOW_COLOR);
}

void switchBetweenCreateAndChoose(GameInterface *gameI){
    menu_stop_gui(gameI);

    gameI->menuInfo.newPartie = !gameI->menuInfo.newPartie;

    menu_gen_main_window(gameI);
    menu_gen_right_menu(gameI);
}

void menu_mouse_main_window(GameInterface *gameI, int x, int y){
    int i = 0;
    if (x >= 48 && x <= 58) {
        for (i = 0; i < 4; i++) {
            if (y == 4 + i*5) {
                gameI->menuInfo.tabPartieMenu.selPartie = i;
                menu_refresh_main_window(gameI);
                rejoindrePartie(gameI);
                break;
            }
        }
    }
}

void menu_mouse_right_menu(GameInterface *gameI, int x, int y){
    if (gameI->menuInfo.newPartie) {
        // Control Create Partie

        // Minus Players
        if (y == 6 && x >= 63 && x <= 66) {
            if (gameI->menuInfo.createPartieMenu.maxPlayers > 1) {
                gameI->menuInfo.createPartieMenu.maxPlayers--;
                refresh_right_menu_gui(gameI);
            }
        }
        // Plus Players
        if (y == 6 && x >= 70 && x <= 73) {
            if (gameI->menuInfo.createPartieMenu.maxPlayers < 100) {
                gameI->menuInfo.createPartieMenu.maxPlayers++;
                refresh_right_menu_gui(gameI);
            }
        }

        // Valider
        if (y == 13 && x >= 64 && x <= 72) {
            createPartie(gameI);
        }

        // Annuler
        if (y == 15 && x >= 64 && x <= 72) {
            switchBetweenCreateAndChoose(gameI);
        }

        // Page Right
        if (y == 19 && x >= 70 && x <= 73) {
            changerPageCreatePartie(gameI, gameI->menuInfo.createPartieMenu.numPage+1);
        }
        // Page Left
        if (y == 19 && x >= 63 && x <= 66) {
            changerPageCreatePartie(gameI, gameI->menuInfo.createPartieMenu.numPage-1);
        }
    } else {
        // Control Join Partie

        // Nouvelle Partie
        if (y == 2 && x >= 63 && x <= 73) {
            switchBetweenCreateAndChoose(gameI);
        }

        // Page Right
        if (y == 19 && x >= 70 && x <= 73) {
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage+1);
        }
        // Page Left
        if (y == 19 && x >= 63 && x <= 66) {
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage-1);
        }
    }
}

void menu_new_partie(GameInterface *gameI){
     // Draw the list of available games
    int i = 0;
    for (i = 0; i < 6; i++) {
        if (!gameI->menuInfo.createPartieMenu.tabMap[i].set) continue;
        wclear(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB);
        wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(LBLUE_COLOR));

        // Selection box
        if(gameI->menuInfo.createPartieMenu.selMap == i) wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(GREEN_COLOR));
        box(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, 0, 0);
        
        wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(LBLUE_COLOR));
        // Information partie
        mvwprintw(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, 1, 2, 
            " Nom de la map: %38s ", 
            gameI->menuInfo.createPartieMenu.tabMap[i].name);

        wrefresh(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB);
    }
}

void menu_keyboard_new_partie(GameInterface *gameI, int key){
    switch (key) {
        case KEY_DOWN:
            // Change selection 
            if (gameI->menuInfo.createPartieMenu.selMap < gameI->menuInfo.createPartieMenu.nbMaps-1) {
                gameI->menuInfo.createPartieMenu.selMap++;
            } else gameI->menuInfo.createPartieMenu.selMap = 0;
            menu_refresh_main_window(gameI);
        break;

        case KEY_UP:
            // Change selection 
            if (gameI->menuInfo.createPartieMenu.selMap > 0) {
                gameI->menuInfo.createPartieMenu.selMap--;
            } else gameI->menuInfo.createPartieMenu.selMap = gameI->menuInfo.createPartieMenu.nbMaps - 1;

            menu_refresh_main_window(gameI);
        break;

        case KEY_LEFT:
            // Write down the action
            changerPageCreatePartie(gameI, gameI->menuInfo.createPartieMenu.numPage - 1);
        break;

        case KEY_RIGHT:
            // Write down the action
            changerPageCreatePartie(gameI, gameI->menuInfo.createPartieMenu.numPage + 1);
        break;

        case KEY_VALIDATE:
            // Write down the action
            createPartie(gameI);
        break;

        default:
            // Write down the action
            set_text_info_gui(gameI, "Action: UNKNOWN", 1, RED_COLOR);
        break;
    }
}

void menu_keyboard_choose_partie(GameInterface *gameI, int key){
    switch (key) {
        case KEY_DOWN:
            // Change selection 
            if (gameI->menuInfo.tabPartieMenu.selPartie < gameI->menuInfo.tabPartieMenu.nbParties-1) {
                gameI->menuInfo.tabPartieMenu.selPartie++;
            } else gameI->menuInfo.tabPartieMenu.selPartie = 0;
            menu_refresh_main_window(gameI);
        break;

        case KEY_UP:
            // Change selection 
            if (gameI->menuInfo.tabPartieMenu.selPartie > 0) {
                gameI->menuInfo.tabPartieMenu.selPartie--;
            } else gameI->menuInfo.tabPartieMenu.selPartie = gameI->menuInfo.tabPartieMenu.nbParties - 1;

            menu_refresh_main_window(gameI);
        break;

        case KEY_LEFT:
            // Write down the action
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage - 1);
        break;

        case KEY_RIGHT:
            // Write down the action
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage + 1);
        break;

        case KEY_VALIDATE:
            // Write down the action
            rejoindrePartie(gameI);
        break;

        default:
            // Write down the action
            set_text_info_gui(gameI, "Action: UNKNOWN", 1, RED_COLOR);
        break;
    }
}

void menu_choose_partie(GameInterface *gameI){
    // Draw the list of available games
    int i = 0;
    for (i = 0; i < 4; i++) {
        if (!gameI->menuInfo.tabPartieMenu.tabPartie[i].set) continue;
        wclear(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(LBLUE_COLOR));

        // Selection box
        if(gameI->menuInfo.tabPartieMenu.selPartie == i) wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(GREEN_COLOR));
        box(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 0, 0);
        
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(LBLUE_COLOR));
        // Information partie
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 1, 2, 
            " Nom de la partie: %36s ", 
            gameI->menuInfo.tabPartieMenu.tabPartie[i].name);
        // Status partie
        if (gameI->menuInfo.tabPartieMenu.tabPartie[i].status == 0) {
            mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, 2, 
            " Status: En attente                                     ");
        } else {
            mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, 2, 
            " Status: En cours                                       ");
        }
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, MATRICE_LEVEL_X-20, 
            " Joueur(s): %02i/%02i",
            gameI->menuInfo.tabPartieMenu.tabPartie[i].nbPlayers, gameI->menuInfo.tabPartieMenu.tabPartie[i].maxPlayers);

        // Bande du bas
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(LBLUE_BLOCK));
        
        // Bouton rejoindre
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(YELLOW_BLOCK));
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 4, MATRICE_LEVEL_X-12, " Rejoindre ");

        wrefresh(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
    }
}

void menu_keyboard_handler(GameInterface *gameI, int key){
    if (gameI->menuInfo.newPartie) {
        menu_keyboard_new_partie(gameI, key);
    } else {
        menu_keyboard_choose_partie(gameI, key);
    }
}

void menu_refresh_main_window(GameInterface *gameI){
    wclear(gameI->gui.winMAIN); // Clear the main window

    if (gameI->menuInfo.newPartie) {
        menu_new_partie(gameI);
    } else {
        menu_choose_partie(gameI);
    }

    wrefresh(gameI->gui.cwinMAIN);
    wrefresh(gameI->gui.winMAIN);
}

void menu_refresh_right_menu(GameInterface *gameI){
    wclear(gameI->gui.winTOOLS); // Clear the right menu

    wattron(gameI->gui.winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
    char centered_name[11];
    center_string(gameI->menuInfo.player_name, 11, centered_name);
    mvwprintw(gameI->gui.winTOOLS, 0, 1, "%11s", centered_name);

    if (!gameI->menuInfo.newPartie) {
        // Créer partie
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 2, 1, " N. Partie ");

        // Supprimer partie
        /*wattron(gameI->gui.winTOOLS, COLOR_PAIR(RED_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 4, 1, " Supprimer ");*/

        wattron(gameI->gui.winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
        // Pagination
        mvwprintw(gameI->gui.winTOOLS, 17, 1, "  Page %02i  ", gameI->menuInfo.tabPartieMenu.numPage);

        // Bouton page gauche
        mvwprintw(gameI->gui.winTOOLS, 19, 1, " <- ");

        // Bouton page droite
        mvwprintw(gameI->gui.winTOOLS, 19, 8, " -> ");
    } else {
        // Créer partie
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(YELLOW_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 2, 0, " MAX JOUEURS ");

        // Pagination
        mvwprintw(gameI->gui.winTOOLS, 4, 3, "  %03i  ", gameI->menuInfo.createPartieMenu.maxPlayers);

        // Bouton remove
        mvwprintw(gameI->gui.winTOOLS, 6, 1, " <- ");

        // Bouton add
        mvwprintw(gameI->gui.winTOOLS, 6, 8, " -> ");

                // Retour
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 13, 2, " Valider ");

        // Retour
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(RED_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 15, 2, " Annuler ");

        wattron(gameI->gui.winTOOLS, COLOR_PAIR(DBLUE_BLOCK));
        // Pagination
        mvwprintw(gameI->gui.winTOOLS, 17, 1, "  Page %02i  ", gameI->menuInfo.createPartieMenu.numPage);

        // Bouton page gauche
        mvwprintw(gameI->gui.winTOOLS, 19, 1, " <- ");

        // Bouton page droite
        mvwprintw(gameI->gui.winTOOLS, 19, 8, " -> ");
    }
    
    wrefresh(gameI->gui.winTOOLS);
}

