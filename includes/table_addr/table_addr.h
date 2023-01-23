#ifndef TABLEADDR_H
#define TABLEADDR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "../level/level.h"

#define TAG_FILE "GAMEOFTABLES"
#define TAILLE_TABLE 10

#define TABLE_TYPE_NONE 0
#define TABLE_TYPE_EMPTY 1
#define TABLE_TYPE_TABLE 2
#define TABLE_TYPE_LEVEL 3

typedef struct file {
    char* filename;
    off_t table[TAILLE_TABLE];
    off_t tableOfEmpty[TAILLE_TABLE];
} file_t;

typedef struct data_info {
    char type;
    size_t size;
} data_info_t;

typedef struct data_read {
    data_info_t info;
    int valid;
    char * buffer;
} data_read_t;

typedef struct empty_data {
    off_t addr_table;
    off_t addr_empty;
    size_t size;
    int found;
} empty_data_t;

typedef struct table_entry {
    off_t addr_table;
    off_t addr_cell;
    long idx_table;
    char type_data;
} table_entry_t;

#define SIZE_TABLE sizeof(off_t)*TAILLE_TABLE
#define SIZE_DATA_INFO sizeof(data_info_t)
#define SIZE_TAG (strlen(TAG_FILE)+1)

#define ADDR_EMTPY_TABLE SIZE_TAG + SIZE_TABLE
#define ADDR_FIRST_TABLE SIZE_TAG
#define ADDR_UNUSED (off_t)-1

file_t* load_file(char* filename);
void free_file(file_t* file);

data_read_t read_data(file_t* file, int index);
int add_data(file_t* file, char* data, size_t size, char data_type);
int remove_entry(file_t* file, int index); // Retourne -1 si l'index n'est pas set dans les tables.

int save_level(file_t* file, int numLevel, Level* level); // Retourne -1 en cas d'erreur. 0 sinon.
int remove_level(file_t* file, int numLevel); // Retourne -1 si level n'est pas dans les tables. 0 sinon.
int get_level(file_t* file, int numLevel, Level** level); // Retourne -1 si le level n'est pas dans les tables. 0 sinon.

char* show_table(file_t* file);

#endif /* TABLEADDR_H */