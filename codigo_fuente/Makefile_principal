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

AVRDUDE=$(MPIDE)/hardware/tools/avrdude
AVRDUDECONF=$(MPIDE)/hardware/tools/avrdude.conf
AVRDUDEFLAGS=-C$(AVRDUDECONF) -c stk500v2 -p pic32 -P $(SERIAL_PORT) -b 115200 -v -U

CPUTYPE:=32MX795F512L
export CPUTYPE
VARIANT:=Max32
export VARIANT

LDSCRIPT_COMMON=core/chipKIT-application-COMMON.ld
LDSCRIPT=core/chipKIT-application-32MX795F512.ld

LDFLAGS=-Os -Wl,--gc-sections -mdebugger -mprocessor=$(CPUTYPE)

CFLAGS=-O2 -c -mno-smart-io -w -fno-exceptions -ffunction-sections -fdata-sections \
			 -g -mdebugger -Wcast-align -fno-short-double -mprocessor=$(CPUTYPE) \
			 -DF_CPU=80000000L -DARDUINO=23 -D_BOARD_MEGA_ -DMPIDEVER=0x01000305 \
			 -DMPIDE=23 -Icore -Icore/variants/$(VARIANT) -Isrc  -Ilib/chipKITEthernet

LIBS=$(shell ls -d lib/**/**/)

CFLAGS+=$(patsubst %,-I%,$(LIBS))


SRC_C=$(wildcard src/*.c) 
SRC_CPP=$(wildcard src/*.cpp)
SRC_S=$(wildcard src/*.S)

OBJ_C=$(patsubst %.c,%.o,$(SRC_C))
OBJ_CPP=$(patsubst %.cpp,%.o,$(SRC_CPP))
OBJ_S=$(patsubst %.S,%.o,$(SRC_S))

LIB_C=$(shell find lib/ -name "*.c")
LIB_CPP=$(shell find lib/ -name "*.cpp")
LIB_S=$(shell find lib/ -name "*.S")

LIB_OBJ_C=$(patsubst %.c,%.o,$(LIB_C))
LIB_OBJ_CPP=$(patsubst %.cpp,%.o,$(LIB_CPP))
LIB_OBJ_S=$(patsubst %.S,%.o,$(LIB_C))

all: hex

core/core.a:
	$(MAKE) -C core

%.o: %.S
	$(CXX) $(CFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -o $@

link: core/core.a $(LIB_OBJ_CPP) $(LIB_OBJ_C) $(OBJ_S) $(OBJ_C) $(OBJ_CPP)
	$(LD) $(LDFLAGS) -o bin/main.elf $(OBJ_CPP) $(LIB_OBJ_CPP) $(LIB_OBJ_C) core/core.a -lm -T $(LDSCRIPT) -T$(LDSCRIPT_COMMON) 
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
