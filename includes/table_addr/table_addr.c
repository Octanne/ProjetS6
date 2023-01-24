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
    logs(L_DEBUG, "free_file | file = %s", file->filename);
    free(file);
}
void init_table(off_t* table) {
    logs(L_DEBUG, "init_table | Initialisation à ADDR_UNUSED des entrées de la table.");
    for (int i = 0; i < TAILLE_TABLE; i++) {
        table[i] = ADDR_UNUSED;
    }
}

int get_data_info(int fd, off_t addr, data_info_t* dataInfo) {
    logs(L_DEBUG, "get_data_info | addr = %ld", addr);
    if (lseek(fd, addr, SEEK_SET) != addr) {
        logs(L_DEBUG, "get_data_info | Déplacement impossible à l'adresse %ld", addr);
        return -1;
    }
    if (read(fd, dataInfo, SIZE_DATA_INFO) == -1) {
        logs(L_DEBUG, "get_data_info | Read error : %s", strerror(errno));
        return -1;
    }
    logs(1, "get_data_info | retour : type = %d, size = %d", dataInfo->type, dataInfo->size);
    return 1;
}
int get_table(int fd, off_t addrTable, off_t* tableToFill) {
    logs(L_DEBUG, "get_table | addrTable = %ld", addrTable);
    if (lseek(fd, addrTable, SEEK_SET) != addrTable) {
        logs(L_DEBUG, "get_table | Déplacement impossible à l'adresse %ld", addrTable);
        return -1;
    }
    
    if (addrTable != ADDR_FIRST_TABLE) {
        // get data_info
        data_info_t dataInfo;
        if (get_data_info(fd, addrTable, &dataInfo) == -1) return -1;

        // check if it's a table
        if (dataInfo.type != TABLE_TYPE_TABLE) {
            logs(L_DEBUG, "get_table | L'adresse %ld ne pointe pas vers une table.", addrTable);
            return -1;
        }
    }

    // get table
    if (read(fd, tableToFill, SIZE_TABLE) == -1) {
        logs(L_DEBUG, "get_table | Read error: %s", strerror(errno));
        return -1;
    }

    logs(L_DEBUG, "get_table | retour : firstAddress = %ld, lastAddress = %ld", (off_t)tableToFill[0], (off_t)tableToFill[TAILLE_TABLE - 1]);
    return 1;
}

int write_data_info(int fd, off_t addr, data_info_t *dataInfo) {
    logs(L_DEBUG, "write_data_info | addr = %ld", addr);
    if (lseek(fd, addr, SEEK_SET) != addr) {
        logs(L_DEBUG, "write_data_info | Déplacement impossible à l'adresse %ld", addr);
        return -1;
    }
    if (write(fd, dataInfo, SIZE_DATA_INFO) == -1) {
        logs(1, "write_data_info | Write error: %s", strerror(errno));
        return -1;
    }

    logs(1, "write_data_info | retour : type = %d, size = %d", dataInfo->type, dataInfo->size);
    return 1;
}
int write_table(int fd, off_t addrTable, off_t* table) {
    logs(L_DEBUG, "write_table | addrTable = %ld", addrTable);
    if (addrTable == ADDR_FIRST_TABLE) {
        logs(L_DEBUG, "write_table | first table");
        if (lseek(fd, addrTable, SEEK_SET) != addrTable) {
            logs(L_DEBUG, "write_table | Déplacement impossible à l'adresse %ld", addrTable);
            return -1;
        }
        if (write(fd, table, SIZE_TABLE) == -1) {
            logs(L_DEBUG, "write_table | Write error: %s", strerror(errno));
            return -1;
        }
        logs(L_DEBUG, "write_table | retour : success!");
        return 1;
    } else {
        logs(L_DEBUG, "write_table | not first table");
        if (lseek(fd, addrTable + SIZE_DATA_INFO, SEEK_SET) != addrTable + SIZE_DATA_INFO) {
            logs(L_DEBUG, "write_table | Déplacement impossible à l'adresse %ld", addrTable + SIZE_DATA_INFO);
            return -1;
        }
        if (write(fd, table, SIZE_TABLE) == -1) {
            logs(L_DEBUG, "write_table | Write error: %s", strerror(errno));
            return -1;
        }
        logs(L_DEBUG, "write_table | retour : success!");
        return 1;
    }
}

int update_empty(int fd, empty_data_t emptyData, size_t sizeNeeded) {
    logs(L_DEBUG, "update_empty | addrEmpty = %ld, sizeNeeded = %ld", emptyData.addr_empty, sizeNeeded);
    // get table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, emptyData.addr_table, table) == -1) return -1;

    // if size not fully used
    size_t sizeLeft = emptyData.size - sizeNeeded - SIZE_DATA_INFO;
    if (sizeLeft > 0) {
        // update data_info
        data_info_t dataInfo;
        dataInfo.type = TABLE_TYPE_EMPTY;
        dataInfo.size = sizeLeft;
        off_t newAddrEmpty = emptyData.addr_empty + SIZE_DATA_INFO + sizeNeeded;
        if (write_data_info(fd, newAddrEmpty, &dataInfo) == -1) return -1;

        // update addr in empty table
        table[emptyData.found] = newAddrEmpty;
        logs(L_DEBUG, "update_empty | addrEmpty updated in table");
    } else {
        // remove addrEmpty from table
        table[emptyData.found] = ADDR_UNUSED;
        logs(L_DEBUG, "update_empty | addrEmpty removed from table");
    }

    // update table
    if(write_table(fd, emptyData.addr_table, table) == -1) return -1;

    logs(L_DEBUG, "update_empty | retour : success!");
    return 1;
}
empty_data_t get_empty(int fd, size_t sizeNeeded, off_t addrTable) {
    logs(L_DEBUG, "get_empty | sizeNeeded = %ld, addrTable = %ld", sizeNeeded, addrTable);
    // error empty data
    empty_data_t errReturn;
    errReturn.found = -2; // set to -2 to know if it's an error

    // get actual table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addrTable, table) == -1) return errReturn;

    logs(L_DEBUG, "get_empty | Check table for an empty space...");

    // check table if there is an empty space
    int i;
    for (i = 0; i < TAILLE_TABLE - 1; i++) {
        // cellule is used
        if (table[i] != ADDR_UNUSED) {
            logs(L_DEBUG, "get_empty | Empty found at index = %d pointing to addr = %ld", i, table[i]);
            // get data_info
            data_info_t dataInfo;
            if(get_data_info(fd, table[i], &dataInfo) == -1) return errReturn;

            // check if it's really an empty
            if (dataInfo.type == TABLE_TYPE_EMPTY) {
                logs(L_DEBUG, "get_empty | This empty had a size of %ld", dataInfo.size);
                // check if it's big enough
                if (dataInfo.size >= sizeNeeded) {
                    // return addr
                    empty_data_t emptyData;
                    emptyData.addr_empty = table[i];
                    emptyData.addr_table = addrTable;
                    emptyData.size = dataInfo.size;
                    emptyData.found = i;
                    logs(L_DEBUG, "get_empty | The size is big enough compare to sizeNeeded");
                    return emptyData;
                }
            } else {
                logs(L_DEBUG, "get_empty | This empty is not an empty");
                return errReturn;
            }
        }
    }

    // check if there is an other empty table
    if (table[TAILLE_TABLE - 1] != ADDR_UNUSED) {
        off_t next_table_addr = table[TAILLE_TABLE - 1];
        logs(L_DEBUG, "get_empty | Check the next table readable at addr = %ld", next_table_addr);
        // get table
        return get_empty(fd, sizeNeeded, next_table_addr);
    } else {
        // no empty space
        logs(L_DEBUG, "get_empty | no more table to check, no empty space found");
        errReturn.found = -1; // set to -1 to know it's have not found
        return errReturn;
    }
}

int find_av_tableEntry(int fd, off_t addrTable, table_entry_t* result) {
    logs(L_DEBUG, "find_av_tableEntry | addrTable = %ld", addrTable);
    // get actual table
    off_t table[TAILLE_TABLE];
    if(get_table(fd, addrTable, table) == -1) return -1;

    // check table if there an unused cell
    int i;
    for (i = 0; i < TAILLE_TABLE - 1; i++) {
        if (table[i] == ADDR_UNUSED) {
            logs(L_DEBUG, "find_av_tableEntry | entry not used found at index = %d of table = %d", i, addrTable);
            result->addr_table = addrTable;
            result->idx_table = i;
            result->addr_cell = ADDR_UNUSED;
            result->type_data = TABLE_TYPE_NONE;
            return 1;
        }
    }

    // check if there is an other empty table
    if (table[TAILLE_TABLE - 1] != ADDR_UNUSED) {
        // get table
        off_t next_table_addr = table[TAILLE_TABLE - 1];
        logs(L_DEBUG, "find_av_tableEntry | Check the next table readable at addr = %ld", next_table_addr);
        return find_av_tableEntry(fd, next_table_addr, result);
    } else {
        // create a new table
        logs(L_DEBUG, "find_av_tableEntry | no more table to check, create a new one");
        // search for an empty space
        empty_data_t emptyData = get_empty(fd, SIZE_TABLE, ADDR_EMTPY_TABLE);
        if (emptyData.found >= 0) {
            // update empty space to be used
            if (update_empty(fd, emptyData, SIZE_TABLE) == -1) {
                logs(L_DEBUG, "find_av_tableEntry | Impossible to update the empty space found");
                return -1;
            }
            // add addr of the new table in the previous table
            table[TAILLE_TABLE - 1] = emptyData.addr_empty;
        } else if (emptyData.found == -1){ 
            // no empty found => set cursor at the end of file
            // update table with the addr of the new table
            table[TAILLE_TABLE - 1] = lseek(fd, 0, SEEK_END);
        } else return -1;

        // declare new table
        off_t new_table[TAILLE_TABLE]; 
        init_table(new_table);

        // write the dataInfo of the new table
        data_info_t dataInfo;
        dataInfo.type = TABLE_TYPE_TABLE;
        dataInfo.size = SIZE_TABLE;
        if (write_data_info(fd,table[TAILLE_TABLE-1], &dataInfo) == -1) return -1;

        // write the new table data
        if (write_table(fd, table[TAILLE_TABLE-1], new_table) == -1) return -1;

        // update table with the addr of the new table
        if (write_table(fd, addrTable, table) == -1) return -1;

        // return the table entry
        result->addr_table = table[TAILLE_TABLE - 1];
        result->idx_table = 0;
        result->addr_cell = ADDR_UNUSED;
        result->type_data = TABLE_TYPE_NONE;
        logs(L_DEBUG, "find_av_tableEntry | new table created at addr = %ld", table[TAILLE_TABLE - 1]);
        return 1;
    }
}

int transform_to_empty(int fd, int globalIndexEntry, off_t addrTable, int numTable) {
    logs(L_DEBUG, "transform_to_empty | globalIndexEntry = %d, addrTable = %ld, numTable = %d", globalIndexEntry, addrTable, numTable);
    // get the table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addrTable, table) == -1) return -1;

    // check index presence
    if (globalIndexEntry > (TAILLE_TABLE-1) * (numTable+1)) {
        logs(L_DEBUG, "transform_to_empty | L'indice est dans une table plus loin");
        if (table[TAILLE_TABLE - 1] == ADDR_UNUSED) {
            // the table is empty
            logs(L_DEBUG, "transform_to_empty | no more table to check, the index is not present");
            return -1;
        } else {
            // go to the next table
            logs(L_DEBUG, "transform_to_empty | go to the next table readable at addr = %ld", table[TAILLE_TABLE - 1]);
            return transform_to_empty(fd, globalIndexEntry, table[TAILLE_TABLE - 1], numTable + 1);
        }
    }

    // calculate the index in the table
    int localIndex = globalIndexEntry - numTable*(TAILLE_TABLE-1);
    logs(L_DEBUG, "transform_to_empty | entry find ! localIndex = %d", localIndex);

    // check if the data is already empty
    if (table[localIndex] == ADDR_UNUSED) {
        logs(L_DEBUG, "transform_to_empty | this index is not present");
        return -1;
    }

    // get the data info
    data_info_t dataInfo;
    if (get_data_info(fd, table[localIndex], &dataInfo) == -1) return -1;

    // check if the data is a table and if it's not empty
    if (dataInfo.type == TABLE_TYPE_TABLE) {
        logs(L_DEBUG, "transform_to_empty | the data is a table, check if it's empty");
        off_t tableToDelete[TAILLE_TABLE];
        if (get_table(fd, table[localIndex], tableToDelete) == -1) return -1;

        // check if the table is empty
        int i;
        for (i = 0; i < TAILLE_TABLE; i++) {
            if (tableToDelete[i] != ADDR_UNUSED) {
                // the table is not empty
                logs(L_DEBUG, "transform_to_empty | the table is not empty, impossible to delete it");
                return -1;
            }
        }
        logs(L_DEBUG, "transform_to_empty | the table is empty, delete it");
    }

    //
    // Erase the data
    //
    // set cursor at the beginning of the empty space
    if (lseek(fd, table[localIndex], SEEK_SET) != table[localIndex]) {
        logs(L_DEBUG, "transform_to_empty | Impossible to set cursor at the beginning of the empty space");   
        return -1;
    }

    // generate a buffer with a copy of datainfo and 0 * size
    char buffer[dataInfo.size + SIZE_DATA_INFO];
    memcpy(buffer, &dataInfo, SIZE_DATA_INFO);
    memset(buffer + SIZE_DATA_INFO, 0, dataInfo.size);

    // write the buffer
    if (write(fd, buffer, dataInfo.size + SIZE_DATA_INFO) == -1) return -1;
    logs(L_DEBUG, "transform_to_empty | Erase data at %d", table[localIndex]);

    // 
    // Register the empty space to the empty table
    //
    table_entry_t tableEntry;
    if (find_av_tableEntry(fd, ADDR_EMTPY_TABLE, &tableEntry) == -1) return -1;

    logs(L_DEBUG, "transform_to_empty: Find space on empty table at %d on index %d", tableEntry.addr_table, tableEntry.idx_table);

    // get the table
    off_t tableEmpty[TAILLE_TABLE];
    if (get_table(fd, tableEntry.addr_table, tableEmpty) == -1) return -1;

    // set the table entry
    tableEmpty[tableEntry.idx_table] = table[localIndex];

    // update the table of empty
    if (write_table(fd, tableEntry.addr_table, tableEmpty) == -1) return -1;
    logs(L_DEBUG, "transform_to_empty: Register the empty space successfully");
    // 
    // End of add the empty space to the empty table
    //

    // update the data_info
    dataInfo.type = TABLE_TYPE_EMPTY;
    if (write_data_info(fd, table[localIndex], &dataInfo) == -1) return -1;

    logs(L_DEBUG, "transform_to_empty | Update the data_info");

    // unregister the old data address
    table[localIndex] = ADDR_UNUSED;

    logs(L_DEBUG, "transform_to_empty | Unregister the old data address");

    // update the table
    if (write_table(fd, addrTable, table) == -1) return -1;

    logs(L_DEBUG, "transform_to_empty | End of transformation");
    return 1;
}

table_entry_t find_tableEntryOfIdx(int fd, int globalIndexEntry, off_t addrTable, int numTable) {
    logs(L_DEBUG, "find_tableEntryOfIdx | globalIndexEntry = %d, addrTable = %ld, numTable = %d", globalIndexEntry, addrTable, numTable);
    // get the table
    off_t table[TAILLE_TABLE];
    get_table(fd, addrTable, table);

    // check index presence
    if (globalIndexEntry > (TAILLE_TABLE-1) * (numTable+1)) {
        logs(L_DEBUG, "find_tableEntryOfIdx | globalIndexEntry > (TAILLE_TABLE-1) * (numTable+1)");
        if (table[TAILLE_TABLE - 1] == ADDR_UNUSED) {
            logs(L_DEBUG, "find_tableEntryOfIdx | no more table to check, the index is not present");
            // the table is empty
            table_entry_t entry;
            entry.addr_cell = ADDR_UNUSED;
            entry.addr_table = ADDR_UNUSED;
            entry.idx_table = -1;
            entry.type_data = TABLE_TYPE_NONE;
            return entry;
        } else {
            // go to the next table
            logs(L_DEBUG, "find_tableEntryOfIdx | go to the next table readable at addr = %ld", table[TAILLE_TABLE - 1]);
            return find_tableEntryOfIdx(fd, globalIndexEntry, table[TAILLE_TABLE - 1], numTable + 1);
        }
    }

    // calculate the index in the table
    int localIndex = globalIndexEntry - (TAILLE_TABLE-1) * numTable;
    logs(L_DEBUG, "find_tableEntryOfIdx | entry find ! localIndex = %d", localIndex);

    // check if the index is present
    if (table[localIndex] == ADDR_UNUSED) {
        logs(L_DEBUG, "find_tableEntryOfIdx | the index is not present");
        // the table is empty
        table_entry_t entry;
        entry.addr_cell = ADDR_UNUSED;
        entry.addr_table = ADDR_UNUSED;
        entry.idx_table = -1;
        entry.type_data = TABLE_TYPE_NONE;
        return entry;
    }

    // get the data info
    data_info_t dataInfo;
    if (get_data_info(fd, table[localIndex], &dataInfo) == -1) {
        table_entry_t entry;
        entry.addr_cell = ADDR_UNUSED;
        entry.addr_table = ADDR_UNUSED;
        entry.idx_table = -1;
        entry.type_data = TABLE_TYPE_NONE;
        logs(L_DEBUG, "find_tableEntryOfIdx | ERROR get_data_info");
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
        logs(L_DEBUG, "find_tableEntryOfIdx | ERROR the data is a table ! idx = %d, addr = %d", localIndex, table[localIndex]);
        return entry;
    }

    // Return cell info
    table_entry_t entry;
    entry.addr_cell = table[localIndex];
    entry.addr_table = addrTable;
    entry.idx_table = localIndex;
    entry.type_data = dataInfo.type;
    logs(L_DEBUG, "find_tableEntryOfIdx | return the cell info, addr_cell = %ld, addr_table = %ld, idx_table = %d, type_data = %d", entry.addr_cell, entry.addr_table, entry.idx_table, entry.type_data);
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
    off_t table[TAILLE_TABLE];
    init_table(table);

    // write tag
    if (write(fd, TAG_FILE, SIZE_TAG) == -1) {
        close(fd);
        return (file_t*)NULL;
    }

    logs(L_DEBUG, "Tag written: %s, size_tag : %ld", TAG_FILE, SIZE_TAG);

    // write table
    if (write(fd, table, SIZE_TABLE) == -1) {
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
    logs(L_DEBUG, "Cursor at : %ld", lseek(fd, 0, SEEK_CUR));
    // write tableOfEmpty
    if (write(fd, table, SIZE_TABLE) == -1) {
        close(fd);
        return (file_t*)NULL;
    }

    // close file
    close(fd);

    logs(L_DEBUG, "File created: %s, %d", filename, fd);

    return file;
}
file_t* load_file(char* filename) {
    // open file
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            // file doesn't exist so create it
            file_t* file = create_file(filename);
            logs(L_DEBUG, "load_file: file %s has been create.", filename);
            return file;
        } else {
            // other error
            logs(L_DEBUG, "load_file: error opening file %s, %s", filename, strerror(errno));
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
        logs(L_DEBUG, "Tag unreconized! Tag: %s", tag);
        free(file);
        close(fd);
        return (file_t*)NULL;
    }

    logs(L_DEBUG, "File loaded: %s, %d", filename, fd);

    // close file
    close(fd);

    return file;
}

int add_data(file_t* file, char* data, size_t size, char data_type) {
    logs(L_DEBUG, "add_data | data: %X, size: %ld, data_type: %d", data, size, data_type);

    int fd = open(file->filename, O_RDWR);
    if (fd == -1) {
        logs(L_DEBUG, "add_data | ERROR open file, %s", file->filename);
        return -1;
    }

    // search place to index the data
    table_entry_t tableEntry;
    if (find_av_tableEntry(fd, ADDR_FIRST_TABLE, &tableEntry) == -1) {
        close(fd);
        logs(L_DEBUG, "add_data | ERROR find_av_tableEntry");
        return -1;
    }
    
    logs(L_DEBUG, "add_data | tableEntry.addr_cell: %ld, tableEntry.addr_table: %ld, tableEntry.idx_table: %d, tableEntry.type_data: %d", tableEntry.addr_cell, tableEntry.addr_table, tableEntry.idx_table, tableEntry.type_data);

    // get the table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, tableEntry.addr_table, table) == -1) {
        close(fd);
        return -1;
    }

    // search space for writing data    
    empty_data_t emptyData = get_empty(fd, size, ADDR_EMTPY_TABLE);
    if (emptyData.found >= 0) {
        logs(L_DEBUG, "add_data | Empty space found, addr: %ld, size : %ld", emptyData.addr_empty, emptyData.size);
        // update empty space to be used
        if (update_empty(fd, emptyData, size) == -1) {
            logs(L_DEBUG, "add_data | Impossible to update the empty space found");
            return -1;
        }
        // update table with the address of the empty space
        table[tableEntry.idx_table] = emptyData.addr_empty;
    } else if (emptyData.found == -1){
        // set cursor at the end of file
        logs(L_DEBUG, "add_data | No empty space found, set cursor at the end of file");
        // update table with the address of the end of file
        table[tableEntry.idx_table] = lseek(fd, 0, SEEK_END);
    } else {
        // error
        logs(L_DEBUG, "add_data | Error while searching empty space");
        close(fd);
        return -1;
    }

    // write data info
    data_info_t dataInfo;
    dataInfo.type = data_type;
    dataInfo.size = size;
    if (write_data_info(fd,table[tableEntry.idx_table],&dataInfo) == -1) {
        logs(L_DEBUG, "add_data | Error while writing data info");
        close(fd);
        return -1;
    }

    // write data
    if (write(fd, data, size) == -1) {
        logs(L_DEBUG, "add_data | Error while writing data");
        close(fd);
        return -1;
    }

    // update table with the address of the data
    if (write_table(fd, tableEntry.addr_table, table) == -1) {
        close(fd);
        return -1;
    }

    // close file
    close(fd);

    logs(L_DEBUG, "add_data | Data added at addr = %ld, idx = %d, tableAddr = %ld", 
    table[tableEntry.idx_table], tableEntry.idx_table, tableEntry.addr_table);

    return 1;
}

int remove_entry(file_t* file, int index) {
    logs(L_DEBUG, "remove_entry | index: %d", index);
    int fd = open(file->filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1) {
        logs(L_DEBUG, "remove_entry | ERROR open file, %s", file->filename);
        return -1;
    }

    int res = transform_to_empty(fd, index, ADDR_FIRST_TABLE, 0);

    // close file
    close(fd);
    logs(L_DEBUG, "remove_entry | Entry removed, result = %d", res);

    return res;
}

int get_data(int fd, off_t addr_data, data_info_t* dataInfo, char** data) {
    // set cursor
    if (lseek(fd, addr_data, SEEK_SET) != addr_data) {
        logs(L_DEBUG, "Get_data | Impossible to set cursor at addr = %ld", addr_data);
        return -1;
    }
    logs(L_DEBUG, "Get_data | Set cursor at addr = %ld", lseek(fd, 0, SEEK_CUR));

    // read data info
    if (get_data_info(fd,addr_data,dataInfo) == -1) {
        logs(L_DEBUG, "Get_data | error read data info");
        return -1;
    }

    logs(L_DEBUG, "Get_data | dataInfo.type = %d, dataInfo.size = %ld", dataInfo->type, dataInfo->size);

    // allocate data
    char *dataS = malloc(dataInfo->size);
    // read data
    if (read(fd, dataS, dataInfo->size) == -1) {
        logs(L_DEBUG, "Get_data | error read data");
        return -1;
    }
    *data = dataS;

    logs(L_DEBUG, "Get_data | Success! data : %X", *data);
    return 1;
}

int remove_level(file_t* file, int numLevel) {
    logs(L_DEBUG, "Remove_level | level : %d", numLevel);
    int i = remove_entry(file, numLevel);
    logs(L_DEBUG, "Remove_level | level : %d, result : %d", numLevel, i);
    return i;
}

char* convert_level_to_bytes(Level* level, int* size) {
    logs(L_INFO, "Level Converter | Convert level to bytes: %d items.", level->listeObjet->taille);

    int num_obj = level->listeObjet->taille;
    *size = num_obj * sizeof(Objet);
    char* buffer = malloc(*size);
    int i = 0;
    
    // Parcourir la liste des objets
    EltListe_o* obj = level->listeObjet->tete;
    while(obj != NULL) {
        //logs(L_DEBUG, "Level Converter | Convert level to bytes: x: %d, y: %d, type: %d", obj->objet->x, obj->objet->y, obj->objet->type);
        memcpy(buffer + (i * sizeof(Objet)), obj->objet, sizeof(Objet));
        obj = obj->suivant;
        i++;
    }

    logs(L_INFO, "Level Converter | Convert level to bytes: Success! %d bytes.", *size);
    
    return buffer;
}

Level* convert_bytes_to_level(char* bytes, int size) {
    logs(L_INFO, "Level Converter | Convert bytes to level: %d bytes.", size);

    Level* level = levelEmpty();
    int i;
    int num_obj = size / sizeof(Objet);
    for (i = 0; i < num_obj; i++) {
        Objet* obj = malloc(sizeof(Objet));
        memcpy(obj, bytes + (i * sizeof(Objet)), sizeof(Objet));
        //logs(L_DEBUG, "Level Converter | Convert bytes to obj: x: %d, y: %d, type: %d", obj->x, obj->y, obj->type);
        levelAjouterObjet(level, obj);
    }

    logs(L_INFO, "Level Converter | Convert bytes to level: Success! %d items.", level->listeObjet->taille);

    return level;
}

int get_level(file_t* file, int numLevel, Level** level) {
    numLevel--;
    logs(L_DEBUG, "Get_level | level : %d", numLevel);
    int fd = open(file->filename, O_RDONLY);
    if (fd == -1) {
        logs(L_DEBUG, "Get_level | Error open file: %s", file->filename);
        return -1;
    }

    logs(L_DEBUG, "Get_level | File opened: %s", file->filename);

    // get table entry
    table_entry_t entry = find_tableEntryOfIdx(fd,numLevel,ADDR_FIRST_TABLE,0);
    logs(L_DEBUG, "Get_level | table entry read: type = %d, addr = %ld", entry.type_data, entry.addr_cell);

    if (entry.type_data == TABLE_TYPE_NONE) {
        // Level not found
        logs(L_DEBUG, "Get_level | Level not found");
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

        logs(L_DEBUG, "Get_level | dataInfo.type = %d, dataInfo.size = %ld", dataInfo.type, dataInfo.size);
        logs(L_DEBUG, "Get_level | data = %X", data);

        // convert bytes to level
        *level = convert_bytes_to_level(data, dataInfo.size);
        if (level == NULL) {
            close(fd);
            return -1;
        }

        free(data);

        // close file
        close(fd);
        logs(L_DEBUG, "Get_level | level : %d, success! %d items loaded!", numLevel, (*level)->listeObjet->taille);

        return 1;
    }

    // close file
    close(fd);

    logs(L_DEBUG, "Get_level | level : %d, not found, unexpected type : %d", numLevel, entry.type_data);

    return -1;
}

int save_level(file_t* file, int numLevel, Level* level) {
    numLevel--;
    logs(L_DEBUG, "Save_level | level : %d", numLevel);
    // open 
    int fd = open(file->filename, O_RDWR);
    if (fd == -1) {
        logs(L_DEBUG, "Save_level | Error open file: %s", file->filename);
        return -1;
    }
    table_entry_t result = find_tableEntryOfIdx(fd, numLevel, ADDR_FIRST_TABLE, 0);
    
    logs(L_DEBUG, "Save_level | table entry found: type: %d, addr: %ld", result.type_data, result.addr_cell);

    int size;
    char* bytes = convert_level_to_bytes(level, &size);

    logs(L_DEBUG, "Save_level | Success level converted to bytes: %X", bytes);
    if (result.type_data == TABLE_TYPE_NONE) {
        close(fd);
        // create new level
        int i = add_data(file, bytes, size, TABLE_TYPE_LEVEL);
        logs(L_DEBUG, "Save_level | Success level created: res = %d", i);
        free(bytes);
        return i;
    } else if (result.type_data == TABLE_TYPE_LEVEL){
        close(fd);
        // update level
        if (remove_level(file, numLevel) == -1) {
            free(bytes);
            logs(L_DEBUG, "Save_level | Error remove entry");
            return -1;
        }
        // create new level
        int i = add_data(file, bytes, size, TABLE_TYPE_LEVEL);
        logs(L_DEBUG, "Save_level | Success level updated: res = %d", i);
        free(bytes);
        return i;
    }

    free(bytes);

    close(fd);

    logs(L_DEBUG, "Save_level | Error, unexpected type: %d", result.type_data);

    return -1;
}

int show_table_c(int fd, off_t addr_table, int level, char* output) {
    //logs(L_DEBUG, "Show table | addrTable = %ld, table_level = %d", addr_table, level);
    char text[300];

    // get table
    off_t table[TAILLE_TABLE];
    if (get_table(fd, addr_table, table) == -1) return -1;

    //logs(L_DEBUG, "Show table | begin show table %d", level);
    sprintf(text, "Table %d :\n", level);
    strcat(output, text);

    // show table
    int i;
    for (i = 0; i < TAILLE_TABLE-1; i++) {
        if (table[i] != ADDR_UNUSED) {
            //logs(L_DEBUG, "Show table | cell %d is not empty", i);
            sprintf(text, " CELL[%d] : %ld\n", i, table[i]);
            strcat(output, text);
            // get data info
            data_info_t dataInfo;
            char* dataBuffer;
            if (get_data(fd, table[i], &dataInfo, &dataBuffer) == -1) {
                strcat(output, "   Data : ERROR\n");
            } else {
                //logs(L_DEBUG, "Show table | data info read: type: %d, size: %ld", dataInfo.type, dataInfo.size);
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
        logs(L_DEBUG, "Show_table | Error open file: %s", file->filename);
        return "ERROR\n";
    }

    strcat(result, "Tables :\n");
    if (show_table_c(fd, ADDR_FIRST_TABLE, 0, result) == -1) strcat(result,"ERROR\n");
    strcat(result, "Table de vides :\n");
    if (show_table_c(fd, ADDR_EMTPY_TABLE, 0, result) == -1) strcat(result,"ERROR\n");

    close(fd);
    return result;
}