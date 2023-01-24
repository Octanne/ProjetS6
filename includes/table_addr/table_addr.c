#include "table_addr.h"

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <error.h>
#include <errno.h>

#include "../utils/utils.h"

void optimize() {
    // Faire la contatenation des empties adjacents
    // Faire la defragmentation des données (déplacement des données vers le début du fichier)
}

void free_file(file_t* file) {
    free(file);
}
void init_table(off_t* table) {
    for (int i = 0; i < TAILLE_TABLE; i++) {
        table[i] = ADDR_UNUSED;
    }
}

int get_data_info(int fd, off_t addr, data_info_t* dataInfo) {
    lseek(fd, addr, SEEK_SET);
    if (read(fd, dataInfo, SIZE_DATA_INFO) == -1) {
        logs(1, "get_data_info: read error: %s", strerror(errno));
        return -1;
    }
    logs(1, "get_data_info: type = %d, size = %d", dataInfo->type, dataInfo->size);
    return 1;
}
int get_table(int fd, off_t addrTable, off_t* tableToFill) {
    logs(1, "get_table: addrTable = %ld", addrTable);
    lseek(fd, addrTable, SEEK_SET);
    
    if (addrTable != ADDR_FIRST_TABLE) {
        // get data_info
        data_info_t dataInfo;
        if (get_data_info(fd, addrTable, &dataInfo) == -1) return -1;

        // check if it's a table
        if (dataInfo.type != TABLE_TYPE_TABLE) return -1;
    }

    // get table
    if (read(fd, tableToFill, SIZE_TABLE) == -1) {
        logs(1, "get_table: read error: %s", strerror(errno));
        return -1;
    }

    logs(1, "get_table: success! firstAddress = %ld", (off_t)tableToFill[0]);
    return 1;
}

int write_data_info(int fd, off_t addr, data_info_t *dataInfo) {
    lseek(fd, addr, SEEK_SET);
    if (write(fd, dataInfo, SIZE_DATA_INFO) == -1) {
        logs(1, "write_data_info: write error: %s", strerror(errno));
        return -1;
    }

    logs(1, "write_data_info: success! type = %d, size = %d", dataInfo->type, dataInfo->size);
    return 1;
}
int write_table(int fd, off_t addrTable, off_t* table) {
    if (addrTable == ADDR_FIRST_TABLE) {
        logs(1, "write_table: first table (addr = %ld)", addrTable);
        lseek(fd, addrTable, SEEK_SET);
        if (write(fd, table, SIZE_TABLE) == -1) {
            logs(1, "write_table: write error: %s", strerror(errno));
            return -1;
        }
        logs(1, "write_table: success!");
        return 1;
    } else {
        logs(1, "write_table: not first table (addr = %ld)", addrTable);
        lseek(fd, addrTable + SIZE_DATA_INFO, SEEK_SET);
        if (write(fd, table, SIZE_TABLE) == -1) {
            logs(1, "write_table: write error: %s", strerror(errno));
            return -1;
        }
        logs(1, "write_table: success!");
        return 1;
    }
}

int update_empty(int fd, empty_data_t emptyData, size_t sizeNeeded) {
    logs(1, "update_empty: addrEmpty = %ld, sizeNeeded = %d", emptyData.addr_empty, sizeNeeded);
    // get table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, emptyData.addr_table, table) == -1) return -1;

    // if size not fully used
    if (emptyData.size != sizeNeeded && (emptyData.size - sizeNeeded - SIZE_DATA_INFO) > 0) {
        // update data_info
        data_info_t dataInfo;
        dataInfo.type = TABLE_TYPE_EMPTY;
        dataInfo.size = emptyData.size - sizeNeeded;
        if (write_data_info(fd, emptyData.addr_empty + SIZE_DATA_INFO + sizeNeeded, &dataInfo) == -1) return -1;

        // update addr in empty table
        table[emptyData.found] = emptyData.addr_empty + SIZE_DATA_INFO + sizeNeeded;
    } else {
        // remove addrEmpty from table
        table[emptyData.found] = ADDR_UNUSED;
    }

    // update table
    if(write_table(fd, emptyData.addr_table, table) == -1) return -1;

    logs(1, "update_empty: success!");
    return 1;
}

empty_data_t get_empty(int fd, size_t sizeNeeded, off_t addrTable);
empty_data_t get_empty(int fd, size_t sizeNeeded, off_t addrTable) {
    // error empty data
    empty_data_t errReturn;
    errReturn.found = -2; // set to -2 to know if it's an error

    // get actual table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addrTable, table) == -1) return errReturn;

    logs(1, "get_empty: checking table at addr = %ld", addrTable);

    // check table if there is an empty space
    int i;
    for (i = 0; i < TAILLE_TABLE - 1; i++) {
        // cellule is used
        if (table[i] != ADDR_UNUSED) {
            logs(1, "get_empty: cellule %d direct to %ld", i, table[i]);
            // get data_info
            data_info_t dataInfo;
            if(get_data_info(fd, table[i], &dataInfo) == -1) return errReturn;

            // check if it's an empty
            if (dataInfo.type == TABLE_TYPE_EMPTY) {
                logs(1, "get_empty: empty found at %ld", table[i]);
                // check if it's big enough
                if (dataInfo.size >= sizeNeeded) {
                    // return addr
                    empty_data_t emptyData;
                    emptyData.addr_empty = table[i];
                    emptyData.addr_table = addrTable;
                    emptyData.size = dataInfo.size;
                    emptyData.found = i;
                    logs(1, "get_empty: size enoughly big, size: %ld", emptyData.size);
                    return emptyData;
                }
            }
        }
    }

    // check if there is an other empty table
    if (table[TAILLE_TABLE - 1] != ADDR_UNUSED) {
        logs(1, "get_empty: next table at %ld", table[TAILLE_TABLE - 1]);
        // get table
        off_t next_table_addr = table[TAILLE_TABLE - 1];
        return get_empty(fd, sizeNeeded, next_table_addr);
    } else {
        // no empty space
        logs(1, "get_empty: no empty space");
        errReturn.found = -1; // set to -1 to know it's have not found
        return errReturn;
    }
}

int find_av_tableEntry(int fd, off_t addrTable, table_entry_t* result);
int find_av_tableEntry(int fd, off_t addrTable, table_entry_t* result) {
    // get actual table
    off_t table[TAILLE_TABLE];
    if(get_table(fd, addrTable, table) == -1) return -1;

    // check table if there is an empty space
    int i;
    for (i = 0; i < TAILLE_TABLE - 1; i++) {
        logs(1, "find_av_tableEntry: table[%d] = %ld", i, table[i]);
        if (table[i] == ADDR_UNUSED) {
            result->addr_table = addrTable;
            result->idx_table = i;
            result->addr_cell = ADDR_UNUSED;
            result->type_data = TABLE_TYPE_NONE;
            return 1;
        }
    }

    // check if there is an other empty table
    if (table[TAILLE_TABLE - 1] != ADDR_UNUSED) {
        logs(1, "find_av_tableEntry: search in next table");
        // get table
        off_t next_table_addr = table[TAILLE_TABLE - 1];
        return find_av_tableEntry(fd, next_table_addr, result);
    } else {
        logs(1, "find_av_tableEntry: create new table");
        // create a new table
        // if empty on file
        empty_data_t emptyData = get_empty(fd, SIZE_TABLE, ADDR_EMTPY_TABLE);
        if (emptyData.found > -1) {
            // set cursor in the empty space found
            lseek(fd, emptyData.addr_empty, SEEK_SET);
            update_empty(fd, emptyData, SIZE_TABLE);

            // update table
            table[TAILLE_TABLE - 1] = emptyData.addr_empty;
        } else if (emptyData.found == -1){
            // set cursor at the end of file
            // update table
            table[TAILLE_TABLE - 1] = lseek(fd, 0, SEEK_END);
        } else return -1;

        // create new table
        off_t new_table[TAILLE_TABLE]; 
        init_table(new_table);

        // write the data info
        data_info_t dataInfo;
        dataInfo.type = TABLE_TYPE_TABLE;
        dataInfo.size = SIZE_TABLE;
        if (write(fd, &dataInfo, SIZE_DATA_INFO) == -1) return -1;

        // write the new table
        if(write(fd, new_table, SIZE_TABLE) == -1) return -1;

        // update table
        write_table(fd, addrTable, table);

        // return the table entry
        result->addr_table = table[TAILLE_TABLE - 1];
        result->idx_table = 0;
        result->addr_cell = ADDR_UNUSED;
        result->type_data = TABLE_TYPE_NONE;
        return 1;
    }
}

int transform_to_empty(int fd, int globalIndexEntry, off_t addrTable, int numTable);
int transform_to_empty(int fd, int globalIndexEntry, off_t addrTable, int numTable) {
    // get the table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addrTable, table) == -1) return -1;

    // check index presence
    if (globalIndexEntry > (TAILLE_TABLE-1) * (numTable+1)) {
        if (table[TAILLE_TABLE - 1] == ADDR_UNUSED) {
            // the table is empty
            return -1;
        } else {
            // go to the next table
            return transform_to_empty(fd, globalIndexEntry, table[TAILLE_TABLE - 1], numTable + 1);
        }
    }

    // calculate the index in the table
    int localIndex = globalIndexEntry - ((TAILLE_TABLE-1) * numTable);

    // get the data info
    data_info_t dataInfo;
    if (get_data_info(fd, table[localIndex], &dataInfo) == -1) return -1;

    // check if the data is a table and if it's not empty
    if (dataInfo.type == TABLE_TYPE_TABLE) {
        off_t tableToDelete[TAILLE_TABLE];
        if (get_table(fd, table[localIndex], tableToDelete) == -1) return -1;

        // check if the table is empty
        int i;
        for (i = 0; i < TAILLE_TABLE; i++) {
            if (tableToDelete[i] != ADDR_UNUSED) {
                // the table is not empty
                return -1;
            }
        }
    }

    //
    // Erase the data
    //
    // set cursor at the beginning of the empty space
    lseek(fd, table[localIndex], SEEK_SET);

    // generate a buffer with a copy of datainfo and 0 * size
    char buffer[dataInfo.size + SIZE_DATA_INFO];
    memset(buffer, 0, dataInfo.size + SIZE_DATA_INFO);
    memcpy(buffer, &dataInfo, SIZE_DATA_INFO);

    // write the buffer
    if (write(fd, buffer, dataInfo.size + SIZE_DATA_INFO) == -1) return -1;
    logs(1, "transform_to_empty: erase data at %d", table[localIndex]);

    // 
    // Register the empty space to the empty table
    //
    table_entry_t tableEntry;
    if (find_av_tableEntry(fd, ADDR_EMTPY_TABLE, &tableEntry) == -1) return -1;

    logs(1, "transform_to_empty: find space on empty table at %d", tableEntry.addr_table);

    // get the table
    off_t tableEmpty[TAILLE_TABLE];
    if (get_table(fd, tableEntry.addr_table, tableEmpty) == -1) return -1;

    logs(1, "transform_to_empty: get the empty table at %d", tableEntry.addr_table);

    // set the table entry
    tableEmpty[tableEntry.idx_table] = table[localIndex];

    // update the table
    if (write_table(fd, tableEntry.addr_table, tableEmpty) == -1) return -1;
    // 
    // End of add the empty space to the empty table
    //

    // update the data_info
    dataInfo.type = TABLE_TYPE_EMPTY;
    if (write_data_info(fd, table[localIndex], &dataInfo) == -1) return -1;

    // unregister the old data address
    table[localIndex] = ADDR_UNUSED;

    // update the table
    if (write_table(fd, addrTable, table) == -1) return -1;

    logs(1, "transform_to_empty: success");
    return 1;
}

table_entry_t find_tableEntryOfIdx(int fd, int globalIndexEntry, off_t addrTable, int numTable);
table_entry_t find_tableEntryOfIdx(int fd, int globalIndexEntry, off_t addrTable, int numTable) {
    // get the table
    off_t table[TAILLE_TABLE];
    get_table(fd, addrTable, table);

    // check index presence
    if (globalIndexEntry >= (TAILLE_TABLE-1) * (numTable+1)) {
        if (table[TAILLE_TABLE - 1] == ADDR_UNUSED) {
            // the table is empty
            table_entry_t entry;
            entry.addr_cell = ADDR_UNUSED;
            entry.addr_table = ADDR_UNUSED;
            entry.idx_table = -1;
            entry.type_data = TABLE_TYPE_NONE;
            return entry;
        } else {
            // go to the next table
            return find_tableEntryOfIdx(fd, globalIndexEntry, table[TAILLE_TABLE - 1], numTable + 1);
        }
    }

    // calculate the index in the table
    int localIndex = globalIndexEntry - ((TAILLE_TABLE-1) * numTable);

    // get the data info
    data_info_t dataInfo;
    if (get_data_info(fd, table[localIndex], &dataInfo) == -1) {
        table_entry_t entry;
        entry.addr_cell = ADDR_UNUSED;
        entry.addr_table = ADDR_UNUSED;
        entry.idx_table = -1;
        entry.type_data = TABLE_TYPE_NONE;
        return entry;
    }

    // check if the data is a table
    if (dataInfo.type == TABLE_TYPE_TABLE) {
        // Devrais pas arrivée
        table_entry_t entry;
        entry.addr_cell = ADDR_UNUSED;
        entry.addr_table = ADDR_UNUSED;
        entry.idx_table = -1;
        entry.type_data = TABLE_TYPE_NONE;
        return entry;
    }

    // Return cell info
    table_entry_t entry;
    entry.addr_cell = table[localIndex];
    entry.addr_table = addrTable;
    entry.idx_table = localIndex;
    entry.type_data = dataInfo.type;

    return entry;
}

file_t* create_file(char* filename) {
    // open file to create only if it doesn't exist
    int fd = open(filename, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {

        return (file_t*)NULL;
    }

    // alocate file struct
    file_t* file = malloc(sizeof(file_t));
    file->filename = filename;
    init_table(file->table);
    init_table(file->tableOfEmpty);

    // write tag
    if (write(fd, TAG_FILE, SIZE_TAG) == -1) {
        close(fd);
        return (file_t*)NULL;
    }

    logs(1, "Tag written: %s, size_tag : %ld", TAG_FILE, SIZE_TAG);

    // write table
    if (write(fd, file->table, SIZE_TABLE) == -1) {
        close(fd);
        return (file_t*)NULL;
    }

    // data info 
    data_info_t dataInfo;
    dataInfo.type = TABLE_TYPE_TABLE;
    dataInfo.size = SIZE_TABLE;
    if (write(fd, &dataInfo, SIZE_DATA_INFO) == -1) {
        close(fd);
        return (file_t*)NULL;
    }
    logs(1, "Cursor at : %ld", lseek(fd, 0, SEEK_CUR));
    // write tableOfEmpty
    if (write(fd, file->tableOfEmpty, SIZE_TABLE) == -1) {
        close(fd);
        return (file_t*)NULL;
    }

    // close file
    close(fd);

    logs(1, "File created: %s, %d", filename, fd);

    return file;
}
file_t* load_file(char* filename) {
    // open file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            // file doesn't exist so create it
            file_t* file = create_file(filename);
            logs(1, "load_file: file %s has been create.", filename);
            return file;
        } else {
            // other error
            logs(1, "load_file: error opening file %s, %s", filename, strerror(errno));
            return (file_t*)NULL;
        }
    }

    // alocate file struct
    file_t* file = malloc(sizeof(file_t));
    file->filename = filename;

    size_t readRes;

    // check if it's a supported file
    char tag[SIZE_TAG];
    readRes = read(fd, tag, SIZE_TAG);
    if (readRes == -1 || strcmp(tag, TAG_FILE) != 0) {
        logs(1, "Tag unreconized! Tag: %s", tag);
        free(file);
        close(fd);
        return (file_t*)NULL;
    }

    // load table
    readRes = read(fd, file->table, SIZE_TABLE);
    if (readRes == -1) {
        logs(1, "Error read table");
        close(fd);
        free(file);
        return (file_t*)NULL;
    }

    lseek(fd, ADDR_EMTPY_TABLE, SEEK_CUR);
    // load tableOfEmpty
    readRes = read(fd, file->tableOfEmpty, SIZE_TABLE);
    if (readRes == -1) {
        logs(1, "Error read tableOfEmpty");
        close(fd);
        free(file);
        return (file_t*)NULL;
    }

    logs(1, "File loaded: %s, %d", filename, fd);

    // close file
    close(fd);

    return file;
}

int add_data(file_t* file, char* data, size_t size, char data_type) {
    logs(1, "Add data: %X, %ld, %d", data, size, data_type);

    int fd = open(file->filename, O_RDWR);
    if (fd == -1) {
        logs(1, "Error open file: %s", file->filename);
        return -1;
    }

    // get the table entry
    table_entry_t tableEntry;
    if (find_av_tableEntry(fd, ADDR_FIRST_TABLE, &tableEntry) == -1) {
        close(fd);
        logs(1, "Error find_av_tableEntry");
        return -1;
    }
    
    logs(1, "Table entry found: %ld, %ld, %d, %d", 
    tableEntry.addr_cell, tableEntry.addr_table, tableEntry.idx_table, tableEntry.type_data);

    // get the table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, tableEntry.addr_table, table) == -1) {
        close(fd);
        return -1;
    }
    
    empty_data_t emptyData = get_empty(fd, size, ADDR_EMTPY_TABLE);
    if (emptyData.found > -1) {
        logs(1, "Empty data found: addr: %ld, size : %ld", emptyData.addr_empty, emptyData.size);
        // set cursor in the empty space found
        lseek(fd, emptyData.addr_empty, SEEK_SET);
        update_empty(fd, emptyData, size);

        // update table
        table[tableEntry.idx_table] = emptyData.addr_empty;
        lseek(fd, emptyData.addr_empty, SEEK_SET);
    } else if (emptyData.found == -1){
        // set cursor at the end of file
        logs(1, "Empty data not found write at the end of file");
        // update table
        table[tableEntry.idx_table] = lseek(fd, 0, SEEK_END);
    } else {
        // error
        logs(1, "Error get_empty");
        close(fd);
        return -1;
    }

    // write data info
    data_info_t dataInfo;
    dataInfo.type = data_type;
    dataInfo.size = size;
    if (write(fd, &dataInfo, SIZE_DATA_INFO) == -1) {
        close(fd);
        return -1;
    }

    // write data
    if (write(fd, data, size) == -1) {
        close(fd);
        return -1;
    }

    // update table
    if (write_table(fd, tableEntry.addr_table, table) == -1) {
        close(fd);
        return -1;
    }

    // close file
    close(fd);

    logs(1, "Data added: %X, %d", data, fd);

    return 1;
}

int remove_entry(file_t* file, int index) {
    int fd = open(file->filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        logs(1, "Error open file: %s", file->filename);
        return -1;
    }

    int res = transform_to_empty(fd, index, ADDR_FIRST_TABLE, 0);

    // close file
    close(fd);

    return res;
}

int get_data(int fd, off_t addr_data, data_info_t* dataInfo, char** data) {
    // set cursor
    lseek(fd, addr_data, SEEK_SET);
    logs(1, "Get data: cursor set at %ld", lseek(fd, 0, SEEK_CUR));

    // read data info
    if (read(fd, dataInfo, SIZE_DATA_INFO) == -1) {
        logs(1, "get_data: error read data info");
        return -1;
    }

    logs(1, "get_data: data info read: type: %d, size: %ld", dataInfo->type, dataInfo->size);

    // allocate data
    char *dataS = malloc(dataInfo->size);
    // read data
    if (read(fd, dataS, dataInfo->size) == -1) {
        logs(1, "get_data: error read data");
        return -1;
    }
    *data = dataS;

    logs(1, "get_data: Success! data : %X", *data);
    return 1;
}

int remove_level(file_t* file, int numLevel) {
    return remove_entry(file, numLevel-1);
}

char* convert_level_to_bytes(Level* level, int* size) {
    logs(1, "Convert level to bytes: %d items.", level->listeObjet->taille);

    int num_obj = level->listeObjet->taille;
    *size = num_obj * sizeof(Objet);
    char* buffer = malloc(*size);
    int i = 0;
    
    // Parcourir la liste des objets
    EltListe_o* obj = level->listeObjet->tete;
    while(obj != NULL) {
        //logs(1, "Convert level to bytes: x: %d, y: %d, type: %d", obj->objet->x, obj->objet->y, obj->objet->type);
        memcpy(buffer + (i * sizeof(Objet)), obj->objet, sizeof(Objet));
        obj = obj->suivant;
        i++;
    }

    logs(1, "Convert level to bytes: Success! %d bytes.", *size);
    
    return buffer;
}

Level* convert_bytes_to_level(char* bytes, int size) {
    logs(1, "Convert bytes to level: %d bytes.", size);

    Level* level = levelEmpty();
    int i;
    int num_obj = size / sizeof(Objet);
    for (i = 0; i < num_obj; i++) {
        Objet* obj = malloc(sizeof(Objet));
        memcpy(obj, bytes + (i * sizeof(Objet)), sizeof(Objet));
        //logs(1, "Convert bytes to obj: x: %d, y: %d, type: %d", obj->x, obj->y, obj->type);
        levelAjouterObjet(level, obj);
    }

    logs(1, "Convert bytes to level: Success! %d items.", level->listeObjet->taille);

    return level;
}

int get_level(file_t* file, int numLevel, Level** level) {
    logs(1, "get_level: start");
    int fd = open(file->filename, O_RDONLY);
    if (fd == -1) {
        logs(1, "Error open file: %s", file->filename);
        return -1;
    }

    logs(1, "get_level: file opened: %s", file->filename);

    // get table entry
    table_entry_t entry = find_tableEntryOfIdx(fd,numLevel-1,ADDR_FIRST_TABLE,0);

    logs(1, "get_level: table entry read: type: %d, addr: %ld", entry.type_data, entry.addr_cell);

    if (entry.type_data == TABLE_TYPE_NONE) {
        // Level not found
        logs(1, "get_level: Level not found");
        close(fd);
        return -1;
    } else if (entry.type_data == TABLE_TYPE_LEVEL) {
        // get data info
        data_info_t dataInfo;
        char* data;
        if (get_data(fd, entry.addr_cell, &dataInfo, &data) == -1) {
            close(fd);
            return -1;
        }

        logs(1, "get_level: data read: %X", data);

        // convert bytes to level
        *level = convert_bytes_to_level(data, dataInfo.size);
        if (level == NULL) {
            close(fd);
            return -1;
        }

        free(data);

        // close file
        close(fd);

        logs(1, "get_level: Level load success : %d items loaded!", (*level)->listeObjet->taille);

        return 1;
    }

    // close file
    close(fd);

    logs(1, "get_level: Level not found");

    return -1;
}

int save_level(file_t* file, int numLevel, Level* level) {
    // open 
    int fd = open(file->filename, O_RDWR);
    if (fd == -1) {
        logs(1, "Error open file: %s", file->filename);
        return -1;
    }
    table_entry_t result = find_tableEntryOfIdx(fd, numLevel-1, ADDR_FIRST_TABLE, 0);
    
    logs(1, "save_level: table entry found: type: %d, addr: %ld", result.type_data, result.addr_cell);

    int size;
    char* bytes = convert_level_to_bytes(level, &size);

    logs(1, "save_level: level converted to bytes: %X", bytes);
    if (result.type_data == TABLE_TYPE_NONE) {
        close(fd);
        // create new level
        int i = add_data(file, bytes, size, TABLE_TYPE_LEVEL);
        free(bytes);
        return i;
    } else if (result.type_data == TABLE_TYPE_LEVEL){
        close(fd);
        // update level
        remove_entry(file, numLevel-1);
        // create new level
        int i = add_data(file, bytes, size, TABLE_TYPE_LEVEL);
        free(bytes);
        return i;
    }

    free(bytes);

    close(fd);

    logs(1, "save_level: Error, unexpected type: %d", result.type_data);

    return -1;
}

int show_table_c(int fd, off_t addr_table, int level, char* output) {
    logs(1, "Show table: %ld, %d", addr_table, level);
    char text[300];

    // get table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addr_table, table) == -1) return -1;

    logs(1, "show_table: begin show table %d", level);
    sprintf(text, "Table %d :\n", level);
    strcat(output, text);

    // show table
    int i;
    for (i = 0; i < TAILLE_TABLE-1; i++) {
        if (table[i] != ADDR_UNUSED) {
            logs(1, "show_table: cell %d is not empty", i);
            sprintf(text, " CELL[%d] : %ld\n", i, table[i]);
            strcat(output, text);
            // get data info
            data_info_t dataInfo;
            char* dataBuffer;
            if (get_data(fd, table[i], &dataInfo, &dataBuffer) == -1) {
                strcat(output, "   Data : ERROR\n");
            } else {
                logs(1, "show_table: data info read: type: %d, size: %ld", dataInfo.type, dataInfo.size);
                // show data size
                sprintf(text, "   Data size : %ld\n", dataInfo.size);
                strcat(output, text);
                // type
                sprintf(text, "   Data type : %d\n", dataInfo.type);
                strcat(output, text);
                strcat(output, "   Data : ");
                if (dataInfo.type == TABLE_TYPE_EMPTY) {
                    strcat(output, "EMPTY\n");
                } else if (dataInfo.type == TABLE_TYPE_LEVEL) {
                    int num_obj = dataInfo.size / sizeof(Objet);
                    sprintf(text, "%d map's items\n", num_obj);
                    strcat(output, text);
                } else {
                    int j;
                    for (j = 0; j < dataInfo.size; j++) {
                        sprintf(text, "%X ", dataBuffer[j]);
                        strcat(output, text);
                    }
                    strcat(output, "\n");
                }
            }
        } else {
            sprintf(text, " CELL[%d] : EMPTY\n", i);
            strcat(output, text);
        }
    }

    // check if there is a next table
    if (table[TAILLE_TABLE-1] != ADDR_UNUSED) {
        strcat(output, " Next table : YES\n\n");
        show_table_c(fd, table[TAILLE_TABLE-1], level+1, output);
    } else {
        strcat(output, " Next table : NO\n\n");
    }

    return 1;
}

char* show_table(file_t* file){
    char * result = malloc(8192);
    int fd = open(file->filename, O_RDONLY);
    if (fd == -1) {
        logs(1, "Error open file: %s", file->filename);
        return "ERROR\n";
    }

    strcat(result, "Tables :\n");
    if (show_table_c(fd, ADDR_FIRST_TABLE, 0, result) == -1) strcat(result,"ERROR\n");
    strcat(result, "Table de vides :\n");
    if (show_table_c(fd, ADDR_EMTPY_TABLE, 0, result) == -1) strcat(result,"ERROR\n");

    close(fd);
    return result;
}