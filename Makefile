# Declaratiile de variabile
CC	= gcc
CFLAGS	= -Wall -lm
SRC	= tema3.c
EXE	= tema3

all: clean build run

# Regula de rulare
run:
	./$(EXE)
 
# Regula de buildare
build:
	$(CC) -o $(EXE) $(SRC) $(CFLAGS)
 
# Regulile de "curatenie" (se folosesc pentru stergerea fisierelor intermediare si/sau rezultate)
.PHONY : clean
clean :
	rm -f $(EXE) *~
