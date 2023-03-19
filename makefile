#
# MAIN CONFIGURATION (DONT MOVE THE LIGN BEFORE OBJECTS)
#

EXEC = client editeur serveur
OBJECTS = client/client_network.o client/gui/client_gui.o client/gui/game_gui.o client/gui/gui_struct.o client/gui/menu_gui.o client/level_update.o editeur/editor_gui.o editeur/level_edit.o editeur/system_save.o global/level.o global/liste.o global/objet.o global/player.o global/utils.o serveur/console_manager.o serveur/partie_manager.o serveur/serveur_network.o
PROJECT_NAME = ProjetS6

SRC_DIR = src
OBJECTS_DIR = obj
INCLUDES = includes/
INCLUDE_DIR = $(addprefix -iquote ,$(shell find $(INCLUDES) -type d))
BIN_DIR = bin

#
# SUFFIXES (must not change it)
#

.SUFFIXES: .c .o

#
# OBJECTS (must not change it)
#

EXEC_O = $(EXEC:=.o)
OBJECTS_O = $(OBJECTS) $(EXEC_O)

#
# ARGUMENTS AND COMPILER (to configure)
#

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
	# We create the directory if it doesn't exist \
	mkdir -p $(dir $(addprefix $(BIN_DIR)/,$$i)); \
	echo "Generating $(addprefix $(BIN_DIR)/,$$i)"; \
	# We get the objects needed for the executable \
	# We add in $$objFilter only the objects that contains the name of the executable in their name or if it contains 'global/' \
	objsFilter=""; \
	for obj in $(OBJECTS); do \
	echo $$obj | grep -q "$$i/" && objsFilter="$$objsFilter $(addprefix $(OBJECTS_DIR)/,$$obj)"; \
	echo $$obj | grep -q "global/" && objsFilter="$$objsFilter $(addprefix $(OBJECTS_DIR)/,$$obj)"; \
	done; \
	# We remove the last and the first space \
	objsFilter=`echo $$objsFilter | sed "s/^ //"`; \
	$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $$objsFilter $(CCLIBS) $(INCLUDE_DIR); \
	#echo "$(CC) -o $(addprefix $(BIN_DIR)/,$$i) $(addprefix $(OBJECTS_DIR)/,$$i.o) $$objsFilter $(CCLIBS) $(INCLUDE_DIR);"; \
	done
	@echo "Done."

msg:
	@echo "Create objects..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

runClient: 
	@# create folder run if it doesn't exist
	@mkdir -p run/client
	@cd run/client && ../../bin/client -p 25565 -h 127.0.0.1

runEditeur:
	@# create folder run if it doesn't exist
	@mkdir -p run/editeur
	@cd run/editeur && ../../bin/editeur

runServeur:
	@# create folder run if it doesn't exist
	@mkdir -p run/serveur
	@cd run/serveur && ../../bin/serveur -p 25565

#
# DEFAULT RULES (must not change it)
#

$(addprefix $(OBJECTS_DIR)/,%.o) : $(addprefix $(SRC_DIR)/,%.c)
	@echo "Generating $@"
	@# We create the directory if it doesn't exist
	@mkdir -p $(dir $@)
	@${CC} ${CCFLAGS} -c $< -o $@ $(INCLUDE_DIR)

#
# MAIN RULES (must not change it)
#

# You can add your own commands
clean:
	@echo "Delete objects, temporary files..."
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(OBJECTS_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,$(EXEC_O))
	@rm -f $(addprefix $(OBJECTS_DIR)/,*~) $(addprefix $(OBJECTS_DIR)/,*#)
	@#rm -f $(addprefix $(INCLUDE_DIR)/,*~) $(addprefix $(INCLUDE_DIR)/,*#)
	@rm -f $(addprefix $(BIN_DIR)/,$(EXEC))
	@rm -f dependancies
	@echo "Done."

depend:
	@echo "Create dependancies..."
	@sed -e "/^# DEPENDANCIES/,$$ d" makefile > dependancies
	@echo "# DEPENDANCIES" >> dependancies
	@echo "OBJECTS =" >> objs.temp
	@echo "INCLUDE_DIR = $(INCLUDE_DIR)";
	@files_c=`find $(SRC_DIR) -mindepth 1 -type f`; \
	for i in $$files_c; do \
	i=`echo $$i | sed "s/$(SRC_DIR)\///"`; # We remove the 'includes/' part of the path \
	o_name=$(OBJECTS_DIR)/`echo $$i | sed "s/\(.*\)\\.c$$/\1.o/"`; \
	c_name=$(SRC_DIR)/`echo $$i`; \
	echo "dependancies for $$c_name..."; \
	$(CC) -MM -MT $$o_name $(CCFLAGS) $$c_name $(INCLUDE_DIR) >> dependancies; \
	# if the o_name equals $(OBJECTS_DIR)/$(EXEC).o we don't add it \
	itsGood=1; \
	for exec in $(EXEC); do \
	comp=$(OBJECTS_DIR)/$$exec.o; \
	if [ "$$o_name" = "$$comp" ]; then \
	itsGood=0; \
	fi; \
	done; \
	if [ $$itsGood -eq 1 ]; then \
	echo $$i | sed "s/\(.*\)\\.c$$/\1.o/" >> objs.temp; \
	fi; \
	done;
	
	@# replace \n by space on objs.temp
	@tr '\n' ' ' < objs.temp > objs.temp.new && mv objs.temp.new objs.temp
	
	@# remove the last space on objs.temp
	@sed -i 's/ *$$//' objs.temp

	@# We add a \n at the end of objs.temp
	@echo "" >> objs.temp

	@# We replace the 6th line of dependancies by the content of objs.temp's file
	@# (the 6th line is the line where we have the OBJECTS = ... part) \
	sed '6d; 5r objs.temp' dependancies > dependancies.tmp && mv dependancies.tmp dependancies

	@cat dependancies > makefile
	@rm dependancies
	@rm objs.temp
	@echo "Done."

#
# CREATE ARCHIVE (must not modify)
#

ARCHIVE_FILES = *

archive: clean
	@echo "Create archive $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Done."

# DEPENDANCIES
obj/client.o: src/client.c includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h includes/client/gui/client_gui.h \
 includes/client/client_network.h
obj/client/client_network.o: src/client/client_network.c \
 includes/client/client_network.h includes/global/utils.h \
 includes/global/constants.h includes/global/net_message.h
obj/client/gui/client_gui.o: src/client/gui/client_gui.c \
 includes/client/gui/client_gui.h includes/global/utils.h \
 includes/global/constants.h includes/client/gui/gui_struct.h \
 includes/global/level.h includes/global/liste.h includes/global/objet.h \
 includes/client/gui/menu_gui.h includes/client/gui/game_gui.h
obj/client/gui/game_gui.o: src/client/gui/game_gui.c \
 includes/client/gui/game_gui.h includes/global/utils.h \
 includes/global/constants.h includes/client/gui/gui_struct.h \
 includes/global/level.h includes/global/liste.h includes/global/objet.h \
 includes/client/level_update.h
obj/client/gui/gui_struct.o: src/client/gui/gui_struct.c \
 includes/client/gui/gui_struct.h includes/global/level.h \
 includes/global/liste.h includes/global/objet.h \
 includes/global/constants.h
obj/client/gui/menu_gui.o: src/client/gui/menu_gui.c \
 includes/client/gui/menu_gui.h
obj/client/level_update.o: src/client/level_update.c \
 includes/client/level_update.h includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h
obj/editeur.o: src/editeur.c includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h includes/editeur/level_edit.h \
 includes/editeur/system_save.h includes/editeur/editor_gui.h
obj/editeur/editor_gui.o: src/editeur/editor_gui.c \
 includes/editeur/editor_gui.h includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h
obj/editeur/level_edit.o: src/editeur/level_edit.c \
 includes/editeur/level_edit.h includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h
obj/editeur/system_save.o: src/editeur/system_save.c \
 includes/editeur/system_save.h includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h
obj/global/level.o: src/global/level.c includes/global/level.h \
 includes/global/liste.h includes/global/objet.h includes/global/utils.h \
 includes/global/constants.h
obj/global/liste.o: src/global/liste.c includes/global/liste.h \
 includes/global/utils.h includes/global/constants.h \
 includes/global/player.h includes/global/objet.h
obj/global/objet.o: src/global/objet.c includes/global/objet.h \
 includes/global/utils.h includes/global/constants.h
obj/global/player.o: src/global/player.c includes/global/player.h
obj/global/utils.o: src/global/utils.c includes/global/utils.h \
 includes/global/constants.h
obj/serveur.o: src/serveur.c includes/serveur/serveur_network.h \
 includes/serveur/console_manager.h includes/global/utils.h \
 includes/global/constants.h
obj/serveur/console_manager.o: src/serveur/console_manager.c \
 includes/serveur/console_manager.h includes/global/utils.h \
 includes/global/constants.h
obj/serveur/partie_manager.o: src/serveur/partie_manager.c \
 includes/serveur/partie_manager.h includes/global/liste.h
obj/serveur/serveur_network.o: src/serveur/serveur_network.c \
 includes/serveur/serveur_network.h includes/global/utils.h \
 includes/global/constants.h includes/global/net_message.h
