
#TODO voir pour auto la génération des dépendances check l'autre script.

EXEC = Client/client Editeur/editeur # EDIT THIS LINE

INCLUDES = includes/ Client/includes/ Editeur/includes/ Serveur/includes/ # EDIT THIS LINE
include_dirs = $(wildcard $(addsuffix */, $(INCLUDES))) # DONT TOUCH
INCLUDE_STD = $(addprefix -iquote ,$(INCLUDES) $(include_dirs)) # DONT TOUCH

OBJECTS = $(OBJECTS_GEN) $(OBJECTS_CLI) $(OBJECTS_EDIT)

CC = gcc
CCFLAGS_STD = -Wall -O3 -Werror
CCFLAGS_DEBUG = -D _DEBUG_
CCFLAGS = $(CCFLAGS_STD)
CCLIBS = -lncurses -lpthread

#
# RULES (must not change it)
#

all: msg $(addprefix $(OBJECTS_DIR)/,$(OBJECTS)) $(addprefix $(OBJECTS_DIR)/,$(EXEC_O))
	@echo "Create executables..."
	@for i in $(EXEC); do \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $(addprefix $(OBJECTS_DIR)/,$(OBJECTS)) $(CCLIBS) -I../$(INCLUDE_DIR)/; \
	done
	@echo "Done."

msg:
	@echo "Create objects..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

#
# DEFAULT RULES (must not change it)
#

$(addprefix $(OBJECTS_DIR)/,%.o) : $(addprefix $(SRC_DIR)/,%.c)
	@${CC} ${CCFLAGS} -c $< -o $@ -I$(INCLUDE_DIR)/

#
# MAIN RULES (must not change it)
#

# You can add your own commands
clean:
	@echo "Delete objects, temporary files..."
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(EXEC_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,*~) $(addprefix $(OBJECTS_DIR)/,*#)
	@rm -f $(addprefix $(INCLUDE_DIR)/,*~) $(addprefix $(INCLUDE_DIR)/,*#)
	@rm -f $(addprefix $(BIN_DIR)/,$(EXEC))
	@rm -f dependancies
	@echo "Done."

depend:
	@echo "Create dependancies..."
	@sed -e "/^# DEPENDANCIES/,$$ d" makefile > dependancies
	@echo "# DEPENDANCIES" >> dependancies
	
	@for FOLDER in $(INCLUDES); do \
	echo "Includes folder : $$FOLDER"; \
	file_c=`find Client -mindepth 1 -maxdepth 1 -type d`; \
	echo "  Files to generate : $$file_c"; \
	for i in $$file_c; do \
	echo "    File to generate : $$i"; \
	done; \
	done
	
	@cat dependancies > makefile
	@rm dependancies
	@echo "Done

# | sed ':a;N;$!ba;s/\n/ /g'
# #$(CC) -MM -MT $(addprefix $(OBJECTS_DIR)/,$$i) $(CCFLAGS) $(SRC_DIR)/`echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` -I$(INCLUDE_DIR) >> dependancies; \
# DEPENDANCIES
