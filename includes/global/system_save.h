
#ifndef TABLEADDR_H
#define TABLEADDR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "level.h"

#define TAG_FILE "GAMEOFTABLES"
#define TAILLE_TABLE 10

#define TABLE_TYPE_NONE 0 // Jamais écris une valeur de retour.
#define TABLE_TYPE_EMPTY 1
#define TABLE_TYPE_TABLE 2
#define TABLE_TYPE_LEVEL 3

typedef struct file {
    char* filename;
} file_t;

typedef struct data_info {
    int8_t type;
    size_t size;
} data_info_t;

typedef struct empty_data {
    off_t addr_table;
    off_t addr_empty;
    size_t size;
    int index;
} empty_data_t;

typedef struct table_entry {
    off_t addr_table;
    off_t addr_cell;
    long idx_table;
    int8_t type_data;
} table_entry_t;

#define SIZE_TABLE			sizeof(off_t) * TAILLE_TABLE
#define SIZE_DATA_INFO		sizeof(data_info_t)
#define SIZE_TAG			sizeof(TAG_FILE)

#define ADDR_EMPTY_TABLE	SIZE_TAG + SIZE_TABLE
#define ADDR_FIRST_TABLE	SIZE_TAG
#define ADDR_UNUSED			(off_t)-1

file_t load_file(char* filename);

int save_level(file_t file, int numLevel, Level level);		// Retourne -1 cas d'erreur. 0 sinon.
int remove_level(file_t file, int numLevel);				// Retourne -1 si level n'est pas dans les tables. 0 sinon.
int get_level(file_t file, int numLevel, Level* level);		// Retourne -1 si le level n'est pas dans les tables. 0 sinon.

char* show_table(file_t file);

#endif /* TABLEADDR_H */

