CC=gcc
CLANGS=-Wall
OBJ=resign.o resignCore.o
resign:$(OBJ)
	$(CC) $(CLANGS) $(OBJ) -o $@ -lm -lz
resign.o:resign.c 
	$(CC) -c $(CLANGS) -o $@ $<
resignCore.o:resignCore.c resignCore.h
	$(CC) -c $(CLANGS) -o $@ $<
.PHONY:clean
clean:
	rm *.o resign