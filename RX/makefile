#-----------------------------------
# SDCC
CC = sdcc
AS = sdas8051
SDCCOPTS ?= --iram-size 256


RX_ADDR = 0xE7_0xE7_0xE7_0xE7

INCDIR  = ./inc
SRCDIR  =./src
OBJDIR  = ./obj
HEXDIR  = ./hex
RELEASEDIR = ./release

C_SRC = $(wildcard  $(SRCDIR)/*.c)
C_SRC_FILE = $(notdir $(C_SRC))
C_OBJ_FILE = $(C_SRC_FILE:%.c=%.c.rel)

CFLAGS = -I$(INCDIR) $(SDCCOPTS) 
LFLAGS = $(LIBPATH) $(LIBS) $(SDCCOPTS)

OBJ = $(addprefix $(OBJDIR)/, $(C_OBJ_FILE))

#-----------------------------------
# STCGAL
PORT ?= com9
STCGAL ?= stcgal
SYSCLK ?= 11059
FLASHFILE ?= $(HEXDIR)/rx.hex
RELEASEFILE ?= $(RELEASEDIR)/RX_$(RX_ADDR).hex

all: $(FLASHFILE) $(RELEASEFILE)

$(HEXDIR)/%.hex: $(OBJDIR)/%.ihx
	mkdir -p $(dir $@)
	$(S) packihx $^ > $@
	
$(OBJDIR)/%.ihx: $(OBJ)
	$(S) $(CC) -o $@ $(SDCCOPTS) $^

$(OBJDIR)/%.c.rel: $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(S) $(CC) -o $@ $(SDCCOPTS) $(CFLAGS) -c $^
	
$(RELEASEFILE): $(FLASHFILE)
	mkdir -p $(dir $@)
	cp $(FLASHFILE) $@

flash:
	$(STCGAL) -p $(PORT) -P stc15a -t $(SYSCLK) $(FLASHFILE)

clean:
	rm -rf $(OBJDIR)/*
	rm -rf $(HEXDIR)/* 


clobber: clean
	rm -rf $(RELEASEDIR)/*	
