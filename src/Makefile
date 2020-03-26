# Win 32 bits
#CXX	:= i686-w64-mingw32-gcc 
#CIBLE	:= tap2wav.exe

# Win 64 bits
CXX1	:= x86_64-w64-mingw32-gcc 
CIBLE1	:= tap2wav.exe

# Ubuntu
CXX2	:= gcc
CIBLE2	:= tap2wav

# Raspberry
#CXX	:= arm-linux-gnueabihf-g++
#CIBLE	:= tap2wav

CXX_FLAGS := -Wall -g 

BIN	:= ../bin
SRC	:= ../src
INCLUDE	:= ../include
LIB	:= ../lib

LIBRARIES	:=


all: $(BIN)/$(CIBLE2) $(BIN)/$(CIBLE1)

$(BIN)/$(CIBLE2): $(SRC)/tap2wav.c
	$(CXX2) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE2) $(LIBRARIES)

$(BIN)/$(CIBLE1): $(SRC)/tap2wav.c
	$(CXX1) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE1) $(LIBRARIES)

clean:
	-rm -f $(BIN)/*
	-rm -f $(SRC)/*.c~


