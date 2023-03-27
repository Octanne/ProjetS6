
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
    gameI->menuInfo.inCreateMenu = false;
    gameI->menuInfo.tabPartieMenu.selPartie = 0;
    gameI->menuInfo.createPartieMenu.selMap = 0;
    gameI->menuInfo.createPartieMenu.maxPlayers = 2;
    gameI->menuInfo.waitToJoin = false;
    gameI->menuInfo.selPartieID = -1;

    menu_gen_main_window(gameI);
    menu_gen_right_menu(gameI);
}

void menu_stop_gui(GameInterface *gameI){
    if (gameI->menuInfo.inCreateMenu) {
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
    if (gameI->menuInfo.inCreateMenu) {
        // Draw maps disponible on title
        wattron(gameI->gui.cwinMAIN, COLOR_PAIR(DBLUE_COLOR));
        mvwprintw(gameI->gui.cwinMAIN, 0, 0, " Map(s) disponible(s) ");
        wattroff(gameI->gui.cwinMAIN, COLOR_PAIR(DBLUE_COLOR));
        wrefresh(gameI->gui.cwinINFOS);

        // Create MapPartie window 1 to 6
        for (i = 0; i < 6; i++) {
            gameI->menuInfo.createPartieMenu.tabMap[i].winTAB = derwin(gameI->gui.winMAIN, 3, MATRICE_LEVEL_X, 1+3*i, 0);
            // Disable all map
            gameI->menuInfo.createPartieMenu.tabMap[i].info.set = false;
            logs(L_INFO, "Menu | TabMap %d created!", i);
        }
        gameI->menuInfo.createPartieMenu.nbMaps = 6;

        // Get map list
        changerPageCreatePartie(gameI, 1);
    } else {
        // Draw parties disponible on title
        wattron(gameI->gui.cwinMAIN, COLOR_PAIR(DBLUE_COLOR));
        mvwprintw(gameI->gui.cwinMAIN, 0, 0, " Partie(s) disponible(s) ");
        wattroff(gameI->gui.cwinMAIN, COLOR_PAIR(DBLUE_COLOR));
        wrefresh(gameI->gui.cwinINFOS);

        // Create TabPartie window 1 to 4
        for (i = 0; i < 4; i++) {
            gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB = derwin(gameI->gui.winMAIN, 5, MATRICE_LEVEL_X, 5*i, 0);
            // Disable all partie
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.set = false;
            logs(L_INFO, "Menu | TabPartie %d created!", i);
        }

        // Get partie list
        changerPagePartieMenu(gameI, 1);
    }

    menu_refresh_main_window(gameI);
}

void menu_gen_right_menu(GameInterface *gameI) {
    menu_refresh_right_menu(gameI);
}

void changerPagePartieMenu(GameInterface *gameI, int page){
    // Remise sur 0 de la selection
    gameI->menuInfo.tabPartieMenu.selPartie = 0;
    
    NetMessage messageReq;
    messageReq.type = UDP_REQ_PARTIE_LIST;
    messageReq.partieListeMessage.numPage = page;
    NetMessage messageUDP = send_udp_message(&gameI->netSocket.udpSocket, &messageReq);

    if (messageUDP.type == UDP_REQ_PARTIE_LIST) {
        // Si pas de partie sur la page on change pas de page
        if (messageUDP.partieListeMessage.nbParties == 0 && page < 1) {
            set_text_info_gui(gameI, "Vous êtes déjà sur la première page !", 1, RED_COLOR);
            return;
        } else if (messageUDP.partieListeMessage.nbParties == 0 && page != 1) {
            set_text_info_gui(gameI, "Vous êtes déjà sur la dernière page !", 1, RED_COLOR);
            return;
        } else if (messageUDP.partieListeMessage.nbParties == 0 && page == 1){
            set_text_info_gui(gameI, "Aucune partie disponible !", 1, RED_COLOR);
        } else {
            set_text_info_gui(gameI, "Page chargée avec succès !", 1, GREEN_COLOR);
        }

        gameI->menuInfo.tabPartieMenu.numPage = messageUDP.partieListeMessage.numPage;
        gameI->menuInfo.tabPartieMenu.nbParties = messageUDP.partieListeMessage.nbParties;
        int i = 0;
        for (i = 0; i < 4; i++) {
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.set = false;
        }
        for (i = 0; i < messageUDP.partieListeMessage.nbParties; i++) {
            strcpy(gameI->menuInfo.tabPartieMenu.tabPartie[i].info.name, messageUDP.partieListeMessage.partieInfo[i].name);
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.nbPlayers = messageUDP.partieListeMessage.partieInfo[i].nbPlayers;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.maxPlayers = messageUDP.partieListeMessage.partieInfo[i].maxPlayers;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.status = messageUDP.partieListeMessage.partieInfo[i].status;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.numPartie = messageUDP.partieListeMessage.partieInfo[i].numPartie;
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.set = true;
        }
    } else {
        set_text_info_gui(gameI, "Erreur lors du changement de page!", 1, RED_COLOR);
    }

    menu_refresh_main_window(gameI);
    menu_refresh_right_menu(gameI);
}

void changerPageCreatePartie(GameInterface *gameI, int page){
    // Remise sur 0 de la selection
    gameI->menuInfo.createPartieMenu.selMap = 0;

    NetMessage messageReq;
    messageReq.type = UDP_REQ_MAP_LIST;
    messageReq.mapListMessage.numPage = page;
    NetMessage messageUDP = send_udp_message(&gameI->netSocket.udpSocket, &messageReq);

    if (messageUDP.type == UDP_REQ_MAP_LIST) {
        // Si pas de map sur la page on change pas de page
        if (messageUDP.partieListeMessage.nbParties == 0 && page < 1) {
            set_text_info_gui(gameI, "Vous êtes déjà sur la première page !", 1, RED_COLOR);
            return;
        } else if (messageUDP.mapListMessage.nbMaps == 0 && page != 1) {
			set_text_info_gui(gameI, "Vous êtes déjà sur la dernière page !", 1, RED_COLOR);
			return;
        } else if (messageUDP.mapListMessage.nbMaps == 0 && page == 1) {
            set_text_info_gui(gameI, "Aucune map disponible !", 1, RED_COLOR);
            return;
        }

        gameI->menuInfo.createPartieMenu.numPage = messageUDP.mapListMessage.numPage;
        gameI->menuInfo.createPartieMenu.nbMaps = messageUDP.mapListMessage.nbMaps;
        int i = 0;
        for (i = 0; i < 6; i++) {
            gameI->menuInfo.createPartieMenu.tabMap[i].info.set = false;
        }
        for (i = 0; i < messageUDP.mapListMessage.nbMaps; i++) {
            strcpy(gameI->menuInfo.createPartieMenu.tabMap[i].info.name, messageUDP.mapListMessage.mapInfo[i].name);
            gameI->menuInfo.createPartieMenu.tabMap[i].info.numMap = messageUDP.mapListMessage.mapInfo[i].numMap;
            gameI->menuInfo.createPartieMenu.tabMap[i].info.set = true;
        }
    } else {
        set_text_info_gui(gameI, "Erreur lors du changement de page!", 1, RED_COLOR);
    }

    menu_refresh_main_window(gameI);
    menu_refresh_right_menu(gameI);
}

void waitForPartie(GameInterface *gameI){
    if (gameI->menuInfo.waitToJoin && gameI->menuInfo.selPartieID == gameI->menuInfo.tabPartieMenu
            .tabPartie[gameI->menuInfo.tabPartieMenu.selPartie].info.numPartie) { // Si on attend deja la partie
        // Network call
        NetMessage messageReq;
        messageReq.type = UDP_REQ_WAITLIST_PARTIE;
        messageReq.partieWaitListMessage.numPartie = gameI->menuInfo.selPartieID;
        messageReq.partieWaitListMessage.waitState = false;
        NetMessage messageUDP = send_udp_message(&gameI->netSocket.udpSocket, &messageReq);
        
        if (messageUDP.partieWaitListMessage.takeInAccount) {
            stop_wait_tcp_connection(gameI);

            // Si bonne reponse
            logs(L_INFO, "Stop waiting for partie %d", gameI->menuInfo.selPartieID);
            gameI->menuInfo.waitToJoin = false;
            gameI->menuInfo.selPartieID = -1;
            int saveSel = gameI->menuInfo.tabPartieMenu.selPartie;
            // Refresh menu
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage);
            gameI->menuInfo.tabPartieMenu.selPartie = saveSel;
            refresh_main_gui(gameI);
            set_text_info_gui(gameI, "Vous avez quitté la liste d'attente", 1, GREEN_COLOR);
        } else {
            // Erreur lors de la tentative de quitter la liste d'attente
            logs(L_INFO, "Erreur lors de la tentative de quitter la liste d'attente");
            set_text_info_gui(gameI, "Impossible de quitter la liste d'attente", 1, RED_COLOR);
        }
    } else if (!gameI->menuInfo.waitToJoin) { // Si on attend pas deja la partie
        // Network call
        NetMessage messageReq;
        messageReq.type = UDP_REQ_WAITLIST_PARTIE;
        messageReq.partieWaitListMessage.numPartie = gameI->menuInfo.tabPartieMenu.tabPartie[gameI->menuInfo.tabPartieMenu.selPartie].info.numPartie;
        messageReq.partieWaitListMessage.waitState = true;
        NetMessage messageUDP = send_udp_message(&gameI->netSocket.udpSocket, &messageReq);
        
        if (messageUDP.partieWaitListMessage.takeInAccount) {
            // Start TCP connection waiting
            wait_tcp_connection(gameI, messageUDP.partieWaitListMessage.portTCP);

            int saveSel = gameI->menuInfo.tabPartieMenu.selPartie;
            gameI->menuInfo.waitToJoin = true;
            gameI->menuInfo.selPartieID = gameI->menuInfo.tabPartieMenu.tabPartie[saveSel].info.numPartie;

            // Refresh menu
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage);
            gameI->menuInfo.tabPartieMenu.selPartie = saveSel;
            refresh_main_gui(gameI);

            logs(L_INFO, "Start waiting for partie %d", gameI->menuInfo.selPartieID);
            set_text_info_gui(gameI, "Vous venez de rejoindre la liste d'attente", 1, GREEN_COLOR);
        } else {
            // Erreur lors de la tentative de rejoindre la liste d'attente
            logs(L_INFO, "Erreur lors de la tentative de rejoindre la liste d'attente");
            set_text_info_gui(gameI, "Impossible de rejoindre la liste d'attente", 1, RED_COLOR);
        }
    }
}

void createPartie(GameInterface *gameI){
    NetMessage messageReq;
    messageReq.type = UDP_REQ_CREATE_PARTIE;
    PartieCreateMessage partieCreateMessage;
    partieCreateMessage.maxPlayers = gameI->menuInfo.createPartieMenu.maxPlayers;
    partieCreateMessage.numMap = gameI->menuInfo.createPartieMenu.tabMap[gameI->menuInfo.createPartieMenu.selMap].info.numMap;
    messageReq.partieCreateMessage = partieCreateMessage;

    // send message
    NetMessage responseUDP = send_udp_message(&gameI->netSocket.udpSocket, &messageReq);

    if (responseUDP.partieCreateMessage.success) {
        int numPage = responseUDP.partieCreateMessage.numPartie / 4 + 1;
        switchBetweenCreateAndChoose(gameI);
        // Refresh menu
        changerPagePartieMenu(gameI, numPage);
        gameI->menuInfo.waitToJoin = true;
        gameI->menuInfo.selPartieID = responseUDP.partieCreateMessage.numPartie;
        // Check if partie is in the current page
        if (gameI->menuInfo.tabPartieMenu.tabPartie[gameI->menuInfo.tabPartieMenu.selPartie].info.numPartie != gameI->menuInfo.selPartieID) {
            for (int i = 0; i < 4; i++) {
                if (gameI->menuInfo.tabPartieMenu.tabPartie[i].info.numPartie == gameI->menuInfo.selPartieID) {
                    gameI->menuInfo.tabPartieMenu.selPartie = i;
                    break;
                }
            }
        }
        refresh_main_gui(gameI);
        refresh_right_menu_gui(gameI);
        logs(L_INFO, "Start waiting for partie %d", gameI->menuInfo.selPartieID);
                    
        wait_tcp_connection(gameI, responseUDP.partieCreateMessage.serverPortTCP);
        set_text_info_gui(gameI, "Partie créée avec succès!", 1, GREEN_COLOR);
    } else {
        set_text_info_gui(gameI, "Erreur lors de la creation de la partie!", 1, RED_COLOR);
    }

}

void switchBetweenCreateAndChoose(GameInterface *gameI){
    menu_stop_gui(gameI);

    gameI->menuInfo.inCreateMenu = !gameI->menuInfo.inCreateMenu;

    menu_gen_main_window(gameI);
    menu_gen_right_menu(gameI);
}

void menu_mouse_main_window(GameInterface *gameI, int x, int y){
    int i = 0;
    if (x >= 48 && x <= 58 && !gameI->menuInfo.inCreateMenu) {
        for (i = 0; i < 4; i++) {
            if (y == 4 + i*5 && gameI->menuInfo.tabPartieMenu.tabPartie[i].info.set && (!gameI->menuInfo.waitToJoin ||
                    gameI->menuInfo.selPartieID == gameI->menuInfo.tabPartieMenu.tabPartie[i].info.numPartie)) {
                gameI->menuInfo.tabPartieMenu.selPartie = i;
                menu_refresh_main_window(gameI);
                waitForPartie(gameI);
                break;
            }
        }
    }
}

void menu_mouse_right_menu(GameInterface *gameI, int x, int y){
    if (gameI->menuInfo.inCreateMenu) {
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
        if (y == 2 && x >= 63 && x <= 73 && !gameI->menuInfo.waitToJoin) {
            switchBetweenCreateAndChoose(gameI);
        }

        // Rafrachir
        if (y == 15 && x >= 63 && x <= 73) {
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage);
        }

        // Page Right
        if (y == 19 && x >= 70 && x <= 73 && !gameI->menuInfo.waitToJoin) {
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage+1);
        }
        // Page Left
        if (y == 19 && x >= 63 && x <= 66 && !gameI->menuInfo.waitToJoin) {
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage-1);
        }
    }
}

void menu_new_partie(GameInterface *gameI){
     // Draw the list of available games
    int i = 0;
    for (i = 0; i < 6; i++) {
        if (!gameI->menuInfo.createPartieMenu.tabMap[i].info.set) continue;
        wclear(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB);
        wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(DBLUE_COLOR));

        // Selection box
        if (gameI->menuInfo.createPartieMenu.selMap == i) wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(GREEN_COLOR));
        box(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, 0, 0);
        
        wattron(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, COLOR_PAIR(DBLUE_COLOR));
        // Information partie
        mvwprintw(gameI->menuInfo.createPartieMenu.tabMap[i].winTAB, 1, 2, 
            " Nom de la map: %38s ", 
            gameI->menuInfo.createPartieMenu.tabMap[i].info.name);

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
    }
}

void menu_keyboard_choose_partie(GameInterface *gameI, int key){
    switch (key) {
        case KEY_DOWN:
            if (gameI->menuInfo.waitToJoin) break;
            // Change selection 
            if (gameI->menuInfo.tabPartieMenu.selPartie < gameI->menuInfo.tabPartieMenu.nbParties-1) {
                gameI->menuInfo.tabPartieMenu.selPartie++;
            } else gameI->menuInfo.tabPartieMenu.selPartie = 0;
            menu_refresh_main_window(gameI);
        break;

        case KEY_UP:
            if (gameI->menuInfo.waitToJoin) break;
            // Change selection 
            if (gameI->menuInfo.tabPartieMenu.selPartie > 0) {
                gameI->menuInfo.tabPartieMenu.selPartie--;
            } else gameI->menuInfo.tabPartieMenu.selPartie = gameI->menuInfo.tabPartieMenu.nbParties - 1;

            menu_refresh_main_window(gameI);
        break;

        case KEY_LEFT:
            if (gameI->menuInfo.waitToJoin) break;
            // Write down the action
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage - 1);
        break;

        case KEY_RIGHT:
            if (gameI->menuInfo.waitToJoin) break;
            // Write down the action
            changerPagePartieMenu(gameI, gameI->menuInfo.tabPartieMenu.numPage + 1);
        break;

        case KEY_VALIDATE:
            if (gameI->menuInfo.waitToJoin) break;
            // Write down the action
            waitForPartie(gameI);
        break;
    }
}

void menu_choose_partie(GameInterface *gameI){
    // Draw the list of available games
    int i = 0;
    for (i = 0; i < 4; i++) {
        if (!gameI->menuInfo.tabPartieMenu.tabPartie[i].info.set) continue;
        wclear(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(DBLUE_COLOR));

        // Selection box
        if (gameI->menuInfo.tabPartieMenu.selPartie == i) wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(GREEN_COLOR));
        box(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 0, 0);
        
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(DBLUE_COLOR));
        // Information partie
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 1, 2, 
            " Nom de la partie: %36s ", 
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.name);
        // Status partie
        if (gameI->menuInfo.tabPartieMenu.tabPartie[i].info.status == 0) {
            mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, 2, 
            " Status: En attente                                     ");
        } else {
            mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, 2, 
            " Status: En cours                                       ");
        }
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 2, MATRICE_LEVEL_X-20, 
            " Joueur(s): %02i/%02i",
            gameI->menuInfo.tabPartieMenu.tabPartie[i].info.nbPlayers, gameI->menuInfo.tabPartieMenu.tabPartie[i].info.maxPlayers);

        // Bande du bas écrire le numero
        wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(DBLUE_COLOR));
        mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 4, 2, " %02i ", gameI->menuInfo.tabPartieMenu.tabPartie[i].info.numPartie);
        
        // Bouton rejoindre/annuler
        if (gameI->menuInfo.waitToJoin) {
            if (gameI->menuInfo.selPartieID == gameI->menuInfo.tabPartieMenu.tabPartie[i].info.numPartie) {
                wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(RED_BLOCK));
                mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 4, MATRICE_LEVEL_X-12, "  Annuler  ");
            } else {
                wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(GRAY_BLOCK));
                mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 4, MATRICE_LEVEL_X-12, " Rejoindre ");
            }
        } else {
            wattron(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, COLOR_PAIR(YELLOW_BLOCK));
            mvwprintw(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB, 4, MATRICE_LEVEL_X-12, " Rejoindre ");
        }

        wrefresh(gameI->menuInfo.tabPartieMenu.tabPartie[i].winTAB);
    }
}

void menu_keyboard_handler(GameInterface *gameI, int key){
    if (gameI->menuInfo.inCreateMenu) {
        menu_keyboard_new_partie(gameI, key);
    } else {
        menu_keyboard_choose_partie(gameI, key);
    }
}

void menu_refresh_main_window(GameInterface *gameI){
    wclear(gameI->gui.winMAIN); // Clear the main window

    if (gameI->menuInfo.inCreateMenu) {
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

    if (!gameI->menuInfo.inCreateMenu) {
        // Créer partie
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(GREEN_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 2, 1, " N. Partie ");

        // Actualiser partie
        wattron(gameI->gui.winTOOLS, COLOR_PAIR(LBLUE_BLOCK));
        mvwprintw(gameI->gui.winTOOLS, 15, 1, "  Refresh  ");

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

