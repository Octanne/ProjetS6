CC = gcc -Wall
OBJETS = main.o functions.o objet.o liste.o level.o level_edit.o constants.o table_addr.o
INCL = ../includes
main : $(OBJETS)
	$(CC) -o main $(OBJETS) -lm -lncurses

main.o : ../main.c $(INCL)/constants.h $(INCL)/level/level.h $(INCL)/level_edit/level_edit.h $(INCL)/functions/functions.h $(INCL)/table_addr/table_addr.h
	$(CC) -c ../main.c

table_addr.o : $(INCL)/table_addr/table_addr.c $(INCL)/table_addr/table_addr.h $(INCL)/level/level.h
	$(CC) -c $(INCL)/table_addr/table_addr.c

constants.o : $(INCL)/constants.c $(INCL)/constants.h
	$(CC) -c $(INCL)/constants.c

functions.o : $(INCL)/functions/functions.c $(INCL)/functions/functions.h
	$(CC) -c $(INCL)/functions/functions.c -lm -lncurses

level.o : $(INCL)/level/level.c $(INCL)/level/level.h $(INCL)/liste/liste.h $(INCL)/constants.h
	$(CC) -c $(INCL)/level/level.c

level_edit.o : $(INCL)/level_edit/level_edit.c $(INCL)/level_edit/level_edit.h $(INCL)/level/level.h $(INCL)/constants.h
	$(CC) -c $(INCL)/level_edit/level_edit.c

objet.o : $(INCL)/objet/objet.c
	$(CC) -c $(INCL)/objet/objet.c

liste.o : $(INCL)/liste/liste.c
	$(CC) -c $(INCL)/liste/liste.c

clean:
	rm main $(OBJETS)