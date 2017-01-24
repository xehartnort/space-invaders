CC = g++
OPT = -O2
LDD = -lSDL -lGL -lGLU -lglut -lSOIL
FILENAMES = $(wildcard *.cpp)
OUT=space

all : $(FILENAMES)
	$(CC) $(OPT) $< $(LDD) -o $(OUT)

# PHONY rule
.PHONY: all
