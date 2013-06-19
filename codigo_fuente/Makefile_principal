MPIDE:=/home/lluis/mpide
export MPIDE
TOOLCHAIN_PREFIX=$(MPIDE)/hardware/pic32/compiler/pic32-tools
CC=$(TOOLCHAIN_PREFIX)/bin/pic32-gcc
CXX=$(TOOLCHAIN_PREFIX)/bin/pic32-g++
AR=$(TOOLCHAIN_PREFIX)/bin/pic32-ar
LD=$(CXX)
OBJCPY=$(TOOLCHAIN_PREFIX)/bin/pic32-objcopy
BIN2HEX=$(TOOLCHAIN_PREFIX)/bin/pic32-bin2hex

SERIAL_PORT=/dev/ttyUSB0

AVRDUDE=/usr/bin/avrdude
AVRDUDECONF=$(MPIDE)/hardware/tools/avrdude.conf
AVRDUDEFLAGS=-C$(AVRDUDECONF) -c stk500v2 -p pic32 -P $(SERIAL_PORT) -b 115200 -v -U

CPUTYPE:=32MX795F512L
export CPUTYPE
VARIANT:=Max32
export VARIANT

LDSCRIPT_COMMON=core/chipKIT-application-COMMON.ld
LDSCRIPT=core/chipKIT-application-32MX795F512.ld

LDFLAGS=-Os -Wl,--gc-sections -mdebugger -mprocessor=$(CPUTYPE)

CFLAGS=-O0 -mno-smart-io -w -fno-exceptions -ffunction-sections -fdata-sections \
			 -g -mdebugger -Wcast-align -fno-short-double -mprocessor=$(CPUTYPE) \
			 -DF_CPU=80000000L -DARDUINO=23 -D_BOARD_MEGA_ -DMPIDEVER=00x01000305 \
			 -DMPIDE=23 -Icore -Icore/variants/$(VARIANT) -Isrc

# for library in `ls -d lib/*/` ; do \
# 	CFLAGS+=-I$library ; \
# done

LIBS=$(shell ls -d lib/*/)

CFLAGS+=$(patsubst %,-I%,$(LIBS))


SRC_C=$(wildcard src/*.c) 
SRC_CPP=$(wildcard src/*.cpp)
SRC_S=$(wildcard src/*.S)

OBJ_C=$(patsubst %.c,%.o,$(SRC_C))
OBJ_CPP=$(patsubst %.cpp,%.o,$(SRC_CPP))
OBJ_S=$(patsubst %.S,%.o,$(SRC_S))

all: hex

core:
	$(MAKE) -C core

lib:
	$(MAKE) -C lib

%.o: %.S
	$(CXX) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) -c $< -o $@

%.S: %.cpp
	$(CXX) $(CFLAGS) -S $< -o $@

%.ii: %.cpp
	$(CXX) $(CFLAGS) -E $< -o $@

%.i: %.c
	$(CC) $(CFLAGS) -E $< -o $@

link: $(OBJ_S) $(OBJ_C) $(OBJ_CPP)
	$(MAKE) -C core
	$(MAKE) -C lib
	$(LD) $(LDFLAGS) -o bin/main.elf $(OBJ_S) $(OBJ_C) $(OBJ_CPP) \
		core/core.a lib/libs.a -Lcore -lm -T $(LDSCRIPT) -T$(LDSCRIPT_COMMON) 
hex: link
	$(OBJCPY) -O ihex -j .eeprom --set-section-flags=.eeprom=alloc,load \
		--no-change-warnings --change-section-lma .eeprom=0 bin/main.elf bin/main.eep
	$(BIN2HEX) -a bin/main.elf

load: hex
	$(AVRDUDE) $(AVRDUDEFLAGS) flash:w:bin/main.hex:i

.PHONY: clean
clean:
	-rm src/*.o
	-rm bin/*
	$(MAKE) -C core clean
	$(MAKE) -C lib clean
