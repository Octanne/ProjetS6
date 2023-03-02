CC = gcc -Wall
OBJETS = main.o utils.o objet.o liste.o level.o level_edit.o system_save.o system_gui.o
INCL = ../includes
main : $(OBJETS)
	$(CC) -o main $(OBJETS) -lm -lncurses

main.o : ../main.c $(INCL)/level/level.h $(INCL)/level_edit/level_edit.h $(INCL)/utils/utils.h $(INCL)/system_save/system_save.h $(INCL)/system_gui/system_gui.h
	$(CC) -c ../main.c

system_save.o : $(INCL)/system_save/system_save.c $(INCL)/system_save/system_save.h $(INCL)/level/level.h $(INCL)/utils/utils.h
	$(CC) -c $(INCL)/system_save/system_save.c

system_gui.o : $(INCL)/system_gui/system_gui.c $(INCL)/system_gui/system_gui.h $(INCL)/level/level.h $(INCL)/utils/utils.h
	$(CC) -c $(INCL)/system_gui/system_gui.c -lm -lncurses

utils.o : $(INCL)/utils/utils.c $(INCL)/utils/utils.h $(INCL)/utils/constants.h $(INCL)/utils/st_benchmark.h
	$(CC) -c $(INCL)/utils/utils.c -lm -lncurses

level.o : $(INCL)/level/level.c $(INCL)/level/level.h $(INCL)/liste/liste.h $(INCL)/utils/utils.h
	$(CC) -c $(INCL)/level/level.c -lm -lncurses

level_edit.o : $(INCL)/level_edit/level_edit.c $(INCL)/level_edit/level_edit.h $(INCL)/level/level.h $(INCL)/utils/utils.h
	$(CC) -c $(INCL)/level_edit/level_edit.c

objet.o : $(INCL)/objet/objet.c $(INCL)/objet/objet.h $(INCL)/utils/utils.h
	$(CC) -c $(INCL)/objet/objet.c

liste.o : $(INCL)/liste/liste.c $(INCL)/liste/liste.h $(INCL)/objet/objet.h
	$(CC) -c $(INCL)/liste/liste.c

clean:
	rm main $(OBJETS)