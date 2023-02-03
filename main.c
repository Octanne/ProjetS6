#include <string.h>
#include <stdlib.h>
#include <ncurses.h>

#include "includes/level/level.h"
#include "includes/utils/utils.h"
#include "includes/level_edit/level_edit.h"
#include "includes/system_save/system_save.h"
#include "includes/system_gui/system_gui.h"

// https://gitlab-mi.univ-reims.fr/rabat01/info0601/-/blob/main/Cours/01_curses/CM_01.pdf

Level* level = NULL;

/**
 * Function to clear the level and generate a new one
 */
void clear_level() {

	// Free level and generate a new one
    level_free(level);
    level = levelCreer();

	// Logs and refresh level
    logs(L_INFO, "Main | Level cleared");
    logs(L_INFO, "Main | Level value : %X", *level);
    refresh_level(level);
}

/**
 * Function to load a level and save the old one
 */
void load_level(int newLevel, int oldLevel) {

	// Loading file
    file_t* file = load_file(FILENAME);

    // Levels offset
    oldLevel--;
    newLevel--;

	// Save old level
    if (save_level(file, oldLevel, level) == -1)
		logs(L_INFO, "Main | Error while saving level %d", oldLevel);
	
	// Load new level and generate empty level if no level found
    if (get_level(file, newLevel, &level) == -1) {
        clear_level(level);
        logs(L_INFO, "Main | Level %d not found, empty level generated", newLevel);
    }

    // Show tables
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================", show_table(file));

	// Free file
    free_file(file);

	// Logs and refresh level
    logs(L_INFO, "Main | New level load : %d, Old level save : %d", newLevel, oldLevel);
    logs(L_INFO, "Main | Level %d : %d items loaded", newLevel, level->listeObjet->taille);
    set_text_info("Level loaded", 1, GREEN_COLOR);
    refresh_level(level);
}

/**
 * Function to load the first level from the file
 * Runned once at the start of the program
 */
void load_level_file() {

	// Loading file
    logs(L_INFO, "Main | Loading first level from file...");
    file_t* file = load_file(FILENAME);
    logs(L_INFO, "Main | File loaded");

	// Generate empty level if no level found
    if (get_level(file, 0, &level) == -1) {
        clear_level(level);
        logs(L_INFO, "Main | First level not found, empty level generated.");
        logs(L_INFO, "Main | Level value : %X", level);
    }
	// Else, load the first level
	else {
        logs(L_INFO, "Main | First level found and loaded.");
        logs(L_INFO, "Main | First level : %d items loaded", level->listeObjet->taille);
        refresh_level(level);
    }

    // Show tables
    logs(L_INFO, "Main | Level value : %X", level);
    logs(L_INFO, "\n======================Affichage Tables======================\n\n%s======================Affichage Tables======================", show_table(file));

	// Free file
    free_file(file);
}

/**
 * Function runned when the game is stopped
 */
void stop_game() {

	// Logs
	logs(L_INFO, "Main | Stopping game...");

	// Free the game interface
    int actualLevel = gameInterface->toolsMenu->levelNumberSelected;
    stop_gui();

	// Save level
    file_t* file = load_file(FILENAME);
    logs(L_INFO, "Main | Saving level %d...", actualLevel);
    save_level(file, actualLevel, level);
    logs(L_INFO, "Main | Level %d saved", actualLevel);

    // Show tables
    logs(L_INFO, "\n===================== Affichage Tables =====================\n\n%s===================== Affichage Tables =====================", show_table(file));
    free_file(file);

	// Free the level
    level_free(level);

	// Close logs
    closeLogs();
}

/**
 * Function managing the mouse event on the tools window
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_toolsWindow(short posX, short posY) {

	// Check if the mouse is inside the tools window
    if (posX >= 62 && posX < 77 && posY >= 0 && posY < 20) {

        // Increase/Decrease level number and load level
		if (posY == 16 && (posX == 65 || posX == 71)) {

			// Variables
			int oldLevel = gameInterface->toolsMenu->levelNumberSelected;
			char text[100];

			// Increase/Decrease level number
			if (posX == 71 && oldLevel < 999)
				gameInterface->toolsMenu->levelNumberSelected++;
			else if (posX == 65 && oldLevel > 1)
				gameInterface->toolsMenu->levelNumberSelected--;
			
			// Do nothing if the level number is the same
			if (oldLevel == gameInterface->toolsMenu->levelNumberSelected)
				return;

			// Load new level
			load_level(gameInterface->toolsMenu->levelNumberSelected, oldLevel);

			// Visual updates
			sprintf(text, "Chargement du niveau '%03i'.", gameInterface->toolsMenu->levelNumberSelected);
			set_text_info(text, 1, WHITE_COLOR);
			refresh_tools_menu();
		}

		// Delete level
		else if (posY == 18 && posX >= 65 && posX <= 70) {
			clear_level(level);
			set_text_info("Le niveau a été supprimé.", 1, WHITE_COLOR);
			logs(L_INFO, "Main | Remise à zéro du niveau %d", gameInterface->toolsMenu->levelNumberSelected);
		} 
    }
}

/**
 * Function managing the mouse event on the level window
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_levelWindow(short posX, short posY) {
	// Logs
    logs(L_INFO, "Main | Level value : %X", level);

	// Check if the mouse is inside the level window
    if (posX >= 0 && posX < 60 && posY >= 0 && posY < 20) {

        // Apply the selected tool on the level
        int success = 0;
        switch(gameInterface->toolsMenu->toolsSelected) {
            case 0:
                // Delete
                success = supprimerObjet(level, posX, posY);
                break;
            case 1:
                // Block
                success = poserBlock(level, posX, posY);
                break;
            case 2:
                // Ladder
                success = poserLadder(level, posX, posY);
                break;
            case 3:
                // Trap
                success = poserTrap(level, posX, posY);
                break;
            case 4:
                // Gate
                success = poserGate(level, posX, posY, gameInterface->toolsMenu->gateColorSelected);
                break;
            case 5:
                // Key
                success = poserKey(level, posX, posY, gameInterface->toolsMenu->gateColorSelected);
                break;
            case 6:
                // Door
                success = poserDoor(level, posX, posY, gameInterface->toolsMenu->doorNumberSelected);
                break;
            case 7:
                // Exit
                success = poserExit(level, posX, posY);
                break;
            case 8:
                // Start
                success = poserStart(level, posX, posY);
                break;
            case 9:
                // Robot
                success = poserRobot(level, posX, posY);
                break;
            case 10:
                // Probe
                success = poserProbe(level, posX, posY);
                break;
            case 11:
                // Life
                success = poserVie(level, posX, posY);
                break;
            case 12:
                // Bomb
                success = poserBomb(level, posX, posY);
                break;
        }

		// Display the result of the action and refresh the level window
        if (success == 1) {
			refresh_level(level);
            set_text_info("Action d'édition effectuée avec succès.", 1, GREEN_COLOR);
        }
		else
			set_text_info("Vous ne pouvez pas effectuer cette action ici.", 1, YELLOW_COLOR);
    }
}

/**
 * Function managing the mouse events
 * @param posX : mouse position on X axis
 * @param posY : mouse position on Y axis
 */
void mouse_event(short posX, short posY) {
    // convert to window level coordinates
    posX -= 1;
    posY -= 1;

    // Apply the mouse event on the level window
    mouse_levelWindow(posX, posY);

    // Apply the mouse event on the tools window
    mouse_toolsWindow(posX, posY);

    // Write down mouse position
    char text[100];
    sprintf(text, "Position : (Y,X) -> (%i,%i)", posY, posX);
    set_text_info(text, 2, LBLUE_COLOR);
}

/**
 * Function managing the keyboard events
 * Running indefinitely until the user press the key to quit the game
 */
void control_handler() {
    int ch;
    while((ch = getch()) != KEY_QUIT_GAME) {
        switch(ch) {

            case KEY_UP:
				// Write down the action
                set_text_info("Action: UP", 1, GREEN_COLOR);

				// Move the cursor up in the tools menu if we are in edit mode
                if (gameInterface->toolsMenu->inEdit) {
					gameInterface->toolsMenu->toolsSelected--;
                    if (gameInterface->toolsMenu->toolsSelected == 0)
                        gameInterface->toolsMenu->toolsSelected = TOTAL_TOOLS - 1;
                    refresh_tools_menu();
                }
            break;
			
            case KEY_DOWN:
				// Write down the action
                set_text_info("Action: DOWN", 1, GREEN_COLOR);

				// Move the cursor down in the tools menu if we are in edit mode
                if (gameInterface->toolsMenu->inEdit) {
					gameInterface->toolsMenu->toolsSelected++;
                    if (gameInterface->toolsMenu->toolsSelected == TOTAL_TOOLS)
						gameInterface->toolsMenu->toolsSelected = 1;
                    refresh_tools_menu();
                }
            break;

            case KEY_LEFT:
				// Write down the action
                set_text_info("Action: LEFT", 1, GREEN_COLOR);

				// If we are in edit mode
                if (gameInterface->toolsMenu->inEdit) {

					// If the tools selected is the gate, we can change the selected color
                    if (gameInterface->toolsMenu->toolsSelected == 4)
                        if (gameInterface->toolsMenu->gateColorSelected > 0)
                            gameInterface->toolsMenu->gateColorSelected--;

					// If the tools selected is the door, we can change the selected door number
                    if (gameInterface->toolsMenu->toolsSelected == 6)
                        if (gameInterface->toolsMenu->doorNumberSelected > 1)
                            gameInterface->toolsMenu->doorNumberSelected--;

                    refresh_tools_menu();
                }
            break;

            case KEY_RIGHT:
				// Write down the action
                set_text_info("Action: RIGHT", 1, GREEN_COLOR);

				// If we are in edit mode
                if (gameInterface->toolsMenu->inEdit) {

					// If the tools selected is the gate, we can change the selected color
                    if (gameInterface->toolsMenu->toolsSelected == 4)
                        if (gameInterface->toolsMenu->gateColorSelected < 3)
                            gameInterface->toolsMenu->gateColorSelected++;
					
					// If the tools selected is the door, we can change the selected door number
                    if (gameInterface->toolsMenu->toolsSelected == 6)
                        if (gameInterface->toolsMenu->doorNumberSelected < 99)
                            gameInterface->toolsMenu->doorNumberSelected++;

                    refresh_tools_menu();
                }
            break;

            case KEY_VALIDATE:
				// Write down the action
                set_text_info("Action: VALIDATE", 1, GREEN_COLOR);
            break;

            case KEY_MOUSE:
				// Mouse event handler
                int posX, posY;
                if (mouse_getpos(&posX, &posY) == OK)
                    mouse_event((short)posX, (short)posY);
            break;
        }
    }
}

/**
 * Exit function called at the exit of the program
 */
void main_exit() {
    stop_game();
}

/**
 * Main function
 * @return EXIT_SUCCESS
 */
int main(void) {
    // Register exit function
    atexit(main_exit);

    // Init gui
    init_gui();

    // Load level
    load_level_file();

    // Launch control handler
    control_handler();

	return EXIT_SUCCESS;
}

