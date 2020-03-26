CXX	  := gcc
# CXX	  := arm-linux-gnueabihf-g++
CXX_FLAGS := -Wall -g 

BIN	:= ../bin
SRC	:= ../src
INCLUDE	:= ../include
LIB	:= ../lib

LIBRARIES	:=


all: $(BIN)/tap2wav 

$(BIN)/tap2wav: $(SRC)/tap2wav.c
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@ $(LIBRARIES)

clean:
	-rm -f $(BIN)/*
	-rm -f $(SRC)/*.c~


