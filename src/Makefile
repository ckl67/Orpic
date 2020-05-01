CXX_WIN32	:= i686-w64-mingw32-gcc 	# Win 32 bits
CXX_WIN64	:= x86_64-w64-mingw32-gcc 	# Win 64 bits
CXX_RASP	:= arm-linux-gnueabihf-gcc	# Raspberry
CXX_LINUX	:= gcc				# linux
CXX		:= gcc				# Ubuntu

CIBLE_1		:= tap2wav	
CIBLE_2		:= tap2wav_ff
CIBLE_3		:= tap2cd
CIBLE_4		:= tap2f16
CIBLE_5		:= novalight

#CXX_FLAGS 	:= -Wall -g 
CXX_FLAGS 	:= -g 

BIN		:= ../bin
SRC		:= ../src
INCLUDE		:= ../include
LIB		:= ../lib

all: $(BIN)/$(CIBLE_1) $(BIN)/$(CIBLE_2) $(BIN)/$(CIBLE_3) $(BIN)/$(CIBLE_4) $(BIN)/$(CIBLE_5) 

$(BIN)/$(CIBLE_1): $(SRC)/tap2wav.c
	$(CXX)       $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@
	$(CXX_LINUX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_ubuntu
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win64.exe 
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win32.exe 
	$(CXX_RASP)  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_rasp

$(BIN)/$(CIBLE_2): $(SRC)/tap2wav_ff.c
	$(CXX)       $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@
	$(CXX_LINUX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_ubuntu
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win64.exe 
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win32.exe 
	$(CXX_RASP)  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_rasp

$(BIN)/$(CIBLE_3): $(SRC)/tap2cd.c
	$(CXX)       $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@
	$(CXX_LINUX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_ubuntu
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win64.exe 
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win32.exe 
	$(CXX_RASP)  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_rasp

$(BIN)/$(CIBLE_4): $(SRC)/tap2f16.c
	$(CXX)       $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@
	$(CXX_LINUX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_ubuntu
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win64.exe 
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win32.exe 
	$(CXX_RASP)  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_rasp


$(BIN)/$(CIBLE_5): $(SRC)/novalight_1.2a.c
	$(CXX)       $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@
	$(CXX_LINUX) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_ubuntu
	$(CXX_WIN64) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win64.exe 
	$(CXX_WIN32) $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_win32.exe 
	$(CXX_RASP)  $(CXX_FLAGS) -I$(INCLUDE) -L$(LIB) $^ -o $@_rasp

clean:
	-rm -f $(BIN)/*
	-rm -f $(SRC)/*.c~


