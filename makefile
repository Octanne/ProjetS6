
#TODO voir pour auto la génération des dépendances check l'autre script.

INCLUDES = includes/ Client/includes/ Editeur/includes/ Serveur/includes/
include_dirs = $(wildcard $(addsuffix */, $(INCLUDES)))
INCLUDE_STD = $(addprefix -iquote ,$(INCLUDES) $(include_dirs))

CC = @gcc -Wall $(INCLUDE_STD) -c
LD = @gcc -Wall -o
LIBS = -lncurses -lpthread

all: prepare client editeur serveur end
	@echo "Process done."

end:
	@echo "Compilation done."
	@echo "Moving files to zbin/..."
	@mv *.o zbin/
	@echo "Moving exec to zrun/..."
	@mv client zrun/
	@mv editeur zrun/
	@mv serveur zrun/

prepare:
	@echo "Preparing folder if needed..."
	@mkdir -p zbin
	@mkdir -p zrun

clean:
	@rm -f *.o client editeur serveur
	@rm -f zbin/*.o
	@echo "Clean done."

# Compilation
client: client.o level_update.o client_gui.o level.o liste.o objet.o utils.o
	$(LD) client client.o level_update.o client_gui.o level.o liste.o \
 objet.o utils.o $(LIBS)

editeur: editeur.o level_edit.o editor_gui.o system_save.o level.o liste.o \
 objet.o utils.o
	$(LD) editeur editeur.o level_edit.o editor_gui.o system_save.o \
 level.o liste.o objet.o utils.o $(LIBS)

serveur: serveur.o player_manager.o player.o
	$(LD) serveur serveur.o player_manager.o player.o $(LIBS)

# Dépendances
serveur.o: Serveur/serveur.c
	$(CC) Serveur/serveur.c

client.o: Client/client.c includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h \
 Client/includes/level_update/level_update.h \
 Client/includes/client_gui/client_gui.h
	$(CC) Client/client.c

editeur.o: Editeur/editeur.c includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h \
 Editeur/includes/level_edit/level_edit.h \
 Editeur/includes/system_save/system_save.h \
 Editeur/includes/editor_gui/editor_gui.h
	$(CC) Editeur/editeur.c

level_edit.o: Editeur/includes/level_edit/level_edit.c \
 Editeur/includes/level_edit/level_edit.h includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) Editeur/includes/level_edit/level_edit.c

editor_gui.o: Editeur/includes/editor_gui/editor_gui.c \
 Editeur/includes/editor_gui/editor_gui.h includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) Editeur/includes/editor_gui/editor_gui.c

system_save.o: Editeur/includes/system_save/system_save.c \
 Editeur/includes/system_save/system_save.h includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) Editeur/includes/system_save/system_save.c

level_update.o: Client/includes/level_update/level_update.c \
 Client/includes/level_update/level_update.h includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) Client/includes/level_update/level_update.c

client_gui.o: Client/includes/client_gui/client_gui.c \
 Client/includes/client_gui/client_gui.h includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) Client/includes/client_gui/client_gui.c

level.o: includes/level/level.c includes/level/level.h \
 includes/liste/liste.h includes/objet/objet.h includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) includes/level/level.c

liste.o: includes/liste/liste.c includes/liste/liste.h \
 includes/objet/objet.h includes/utils/utils.h includes/utils/constants.h \
 includes/utils/st_benchmark.h
	$(CC) includes/liste/liste.c

objet.o: includes/objet/objet.c includes/objet/objet.h \
 includes/utils/utils.h includes/utils/constants.h \
 includes/utils/st_benchmark.h
	$(CC) includes/objet/objet.c

utils.o: includes/utils/utils.c includes/utils/utils.h \
 includes/utils/constants.h includes/utils/st_benchmark.h
	$(CC) includes/utils/utils.c

player.o: includes/player/player.c includes/player/player.h
	$(CC) includes/player/player.c

player_manager.o: Serveur/includes/player_manager/player_manager.c Serveur/includes/player/player_manager.h \
 includes/player/player.h
	$(CC) Serveur/includes/player_manager/player_manager.c