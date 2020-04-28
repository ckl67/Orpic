# Win 32 bits
CXX_WIN32	:= i686-w64-mingw32-gcc 
CIBLE_WIN32	:= tap2wav_w32.exe

# Win 64 bits
CXX_WIN64	:= x86_64-w64-mingw32-gcc 
CIBLE_WIN64	:= tap2wav_w64.exe

# Raspberry
CXX_RASP	:= arm-linux-gnueabihf-gcc
CIBLE_RASP	:= tap2wav_rasp

# Ubuntu
CXX		:= gcc
CIBLE		:= tap2wav	

CXX_FLAGS 	:= -Wall -g 

BIN		:= ../bin
SRC		:= ../src
INCLUDE		:= ../include
LIB		:= ../lib

LIBRARIES	:=

all: $(BIN)/$(CIBLE) $(BIN)/$(CIBLE_WIN64) $(BIN)/$(CIBLE_WIN32) $(BIN)/$(CIBLE_RASP)

$(BIN)/$(CIBLE): $(SRC)/tap2wav.c
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE) $(LIBRARIES)

$(BIN)/$(CIBLE_WIN64): $(SRC)/tap2wav.c
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE_WIN64) $(LIBRARIES)

$(BIN)/$(CIBLE_WIN32): $(SRC)/tap2wav.c
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE_WIN32) $(LIBRARIES)

$(BIN)/$(CIBLE_RASP): $(SRC)/tap2wav.c
	$(CXX_RASP) -I$(INCLUDE) -L$(LIB) $^ -o $(BIN)/$(CIBLE_RASP) $(LIBRARIES)

clean:
	-rm -f $(BIN)/*
	-rm -f $(SRC)/*.c~


