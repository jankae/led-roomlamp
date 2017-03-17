MCU = atmega8
F_CPU = 8000000UL

# Settings for flashing via esp-link
PROGRAMMER_TYPE = arduino
PROGRAMMER_ARGS = -D -Pnet:192.168.178.174:23 -b38400

#PROGRAMMER_TYPE = stk500v2
# extra arguments to avrdude: baud rate, chip type, -F flag, etc.
#PROGRAMMER_ARGS = -P/dev/ttyUSB0

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRSIZE = avr-size
AVRDUDE = avrdude

#EFUSE = 0xFF
HFUSE = 0xDC
LFUSE = 0xFF

##########------------------------------------------------------##########
##########                   Makefile Magic!                    ##########
##########         Summary:                                     ##########
##########             We want a .hex file                      ##########
##########        Compile source files into .elf                ##########
##########        Convert .elf file into .hex                   ##########
##########        You shouldn't need to edit below.             ##########
##########------------------------------------------------------##########

## The name of your project (without the .c)
TARGET = main

# Object files: will find all .c/.h files in current directory
#  and in LIBDIR.  If you have any other (sub-)directories with code,
#  you can add them in to SOURCES below in the wildcard statement.
SOURCES=$(wildcard *.c)
ASOURCES=$(wildcard *.S)
OBJECTS=$(SOURCES:.c=.o) $(ASOURCES:.S=.o)
HEADERS=$(SOURCES:.c=.h)

## Compilation options, type man avr-gcc if you're curious.
CFLAGS = -Os -g -std=gnu99 -Wall -Wextra -DF_CPU=$(F_CPU)
ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs
## Use short (8-bit) data types 
CFLAGS += -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums 
## Splits up object files per function
CFLAGS += -ffunction-sections -fdata-sections 
LDFLAGS = -Wl,-Map,$(TARGET).map 
## Optional, but often ends up with smaller code
LDFLAGS += -Wl,--gc-sections 
## Relax shrinks code even more, but makes disassembly messy
## LDFLAGS += -Wl,--relax
## LDFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm  ## for floating-point printf
## LDFLAGS += -Wl,-u,vfprintf -lprintf_min      ## for smaller printf
TARGET_ARCH = -mmcu=$(MCU)

## Explicit pattern rules:
all: $(TARGET).hex
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf

##  To make .o files from .c files 
%.o: %.c Makefile
	 $(CC) $(CFLAGS) $(TARGET_ARCH) -c -o $@ $<
	 
%.o: %.S
	 $(CC) -c $(ASFLAGS) $(TARGET_ARCH) $< -o $@

$(TARGET).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) $(TARGET_ARCH) $^ $(LDLIBS) -o $@

%.hex: %.elf
	 $(OBJCOPY) -j .text -j .data -O ihex $< $@

%.eeprom: %.elf
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@ 

%.lst: %.elf
	$(OBJDUMP) -S $< > $@

depend: .depend

.depend: $(SOURCES)
	rm -f ./.depend
	$(CC) $(CFLAGS) -MM $^ > ./.depend;

include .depend

## These targets don't have files named after them
.PHONY: all disassemble disasm eeprom size clean squeaky_clean flash fuses

clean:
	rm -f $(TARGET).elf $(TARGET).hex $(TARGET).obj \
	$(TARGET).o $(TARGET).d $(TARGET).eep $(TARGET).lst \
	$(TARGET).lss $(TARGET).sym $(TARGET).map $(TARGET)~ \
	$(TARGET).eeprom $(OBJECTS) $(SOURCES:.c=.asm)

flash: $(TARGET).elf
	@echo
	$(AVRSIZE) -C --mcu=$(MCU) $(TARGET).elf
	@echo Calling avrdude to flash the firmware...
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -U flash:w:$(TARGET).hex

fuses:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS) -u -U hfuse:w:$(HFUSE):m -U lfuse:w:$(LFUSE):m

reset:
	$(AVRDUDE) -c $(PROGRAMMER_TYPE) -p $(MCU) $(PROGRAMMER_ARGS)

