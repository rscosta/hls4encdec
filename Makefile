CC = gcc
NASM= nasm
MV=mv
MKDIR=mkdir
CP=cp
LIBS = -lcrypto -m64 -lrt -lc -lm
INC=-Iinclude/
PROG_NAME = hls4encdec
ASM_CPUID = cpuida64
C_CPUID = cpuidc64
SRC_DIR=src
BIN_DIR=bin
INSTALL_DIR=/usr/bin
SCRIPT_DIR=scripts
GEN_HLS_SCRIPT_NAME=gen_hls.sh

$(PROG_NAME): $(ASM_CPUID).o $(C_CPUID).o $(PROG_NAME).o
	$(MV) *.o $(SRC_DIR) 
	$(MKDIR) -p $(BIN_DIR)
	$(CP) $(SCRIPT_DIR)/$(GEN_HLS_SCRIPT_NAME) $(BIN_DIR) 
	$(CC) $(SRC_DIR)/$(PROG_NAME).o -o $(BIN_DIR)/$(PROG_NAME) $(LIBS) $(SRC_DIR)/$(ASM_CPUID).o $(SRC_DIR)/$(C_CPUID).o
	
$(PROG_NAME).o: $(SRC_DIR)/$(PROG_NAME).c
	$(CC) $(LIBS) $(INC) -c $(SRC_DIR)/$(PROG_NAME).c

$(ASM_CPUID).o: $(SRC_DIR)/$(ASM_CPUID).asm
	$(NASM) -f elf64 $(SRC_DIR)/$(ASM_CPUID).asm

$(C_CPUID).o: $(SRC_DIR)/$(C_CPUID).c
	$(CC) -m64 -c $(SRC_DIR)/$(C_CPUID).c $(INC)

clean:
	rm -rf $(SRC_DIR)/$(PROG_NAME) $(SRC_DIR)/*.o *.o $(BIN_DIR)

