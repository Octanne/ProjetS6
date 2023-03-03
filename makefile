#
# MAIN CONFIGURATION
#

EXEC = client editeur
MAIN = Client/client.c Editeur/editeur.c
INCLUDES = includes/ Client/includes/ Editeur/includes/ Serveur/includes/
OBJECTS_CLI = level_update.o client_gui.o
OBJECTS_EDIT = level_edit.o editor_gui.o system_save.o
OBJECTS = level.o liste.o objet.o utils.o $(OBJECTS_CLI) $(OBJECTS_EDIT)
PROJECT_NAME = game_projets6

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJECTS
#

EXEC_O = $(EXEC:=.o)
OBJECTS_O = $(OBJECTS) $(EXEC_O)

#
# ARGUMENTS AND COMPILER
#

CC = gcc
CCFLAGS_STD = -Wall -Werror
CCFLAGS_DEBUG = -D _DEBUG_

include_dirs = $(wildcard $(addsuffix */, $(INCLUDES)))
INCLUDE_STD = $(addprefix -iquote ,$(INCLUDES) $(include_dirs))

CCFLAGS = $(CCFLAGS_STD) $(INCLUDE_STD)
CCLIBS = -lncursesw

#
# RULES
#

all: msg $(OBJECTS) $(EXEC_O)
	@echo "Create executables..."
	@for i in $(MAIN); do \
	$(CC) -o $$i $$i.o $(OBJECTS) $(CCLIBS); \
	done
	@echo "Done."

msg:
	@echo "Create objects..."

debug: CCFLAGS = $(CCFLAGS_STD) $(CCFLAGS_DEBUG)
debug: all

#
# DEFAULT RULES
#

%.o : %.c
	@echo "Create object $@..."
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# GENERAL RULES
#

clean:
	@echo "Delete objects, temporary files..."
	@rm -f $(OBJECTS) $(EXEC_O)
	@rm -f *~ *#
	@rm -f $(EXEC)
	@rm -f dependancies
	@echo "Done."

depend:
	@echo "Create dependancies..."
	@sed -e "/^# DEPENDANCIES/,$$ d" makefile > dependancies
	@echo "# DEPENDANCIES" >> dependancies
	@echo "OBJ : $(OBJECTS_O)"
	@for i in $(OBJECTS_O); do \
	echo "Create dependancies for $$i..."; \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependancies; \
	done
	@cat dependancies > makefile
	@rm dependancies
	@echo "Done."

#
# CREATE ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Create archive $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename "$$PWD"`; cd .. && tar zcf $(PROJECT_NAME)_$(shell date '+%y%m%d.tar.gz') $(addprefix "$$REP"/,$(ARCHIVE_FILES))
	@echo "Done."

# DEPENDANCIES
Client/client.o: Client/client.c includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h \
 Client/includes/level_update/level_update.h \
 Client/includes/client_gui/client_gui.h
Editeur/editeur.o: Editeur/editeur.c includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h \
 Editeur/includes/level_edit/level_edit.h \
 Editeur/includes/system_save/system_save.h \
 Editeur/includes/editor_gui/editor_gui.h
