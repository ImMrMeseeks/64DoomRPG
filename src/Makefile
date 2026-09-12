ROOTDIR = $(N64_INST)
GCCN64PREFIX = $(ROOTDIR)/bin/mips64-elf-
CHKSUM64PATH = $(ROOTDIR)/bin/chksum64
MKDFSPATH = $(ROOTDIR)/bin/mkdfs
HEADERPATH = $(ROOTDIR)/mips64-elf/lib
N64TOOL = $(ROOTDIR)/bin/n64tool
HEADERNAME = header
LINK_FLAGS = -L%N64_INST%/Libdragon/Lib -L%MIPS%/lib -L%MIPS%/lib/gcc/mips64-elf/4.8.1 -L$(ROOTDIR)/mips64-elf/lib -lmikmod -ldragon -lm -lc -ldragonsys -Tn64ld.x
PROG_NAME = DoomRPG
CFLAGS = -march=vr4300 -mtune=vr4300 -O2 -Wall -nostdlib -ffreestanding -fno-exceptions

ASFLAGS = -mtune=vr4300 -march=vr4300
CC = $(GCCN64PREFIX)gcc
AS = $(GCCN64PREFIX)as
LD = $(GCCN64PREFIX)ld
OBJCOPY = $(GCCN64PREFIX)objcopy

O = Build
OBJS=						\
		$(O)/Combat.o		\
		$(O)/CombatEntity.o		\
		$(O)/DoomCanvas.o			\
		$(O)/DoomRPG.o		\
		$(O)/Entity.o		\
		$(O)/EntityDef.o		\
		$(O)/EntityMonster.o		\
		$(O)/Game.o		\
		$(O)/Hud.o			\
		$(O)/Main.o			\
		$(O)/Menu.o			\
		$(O)/MenuItem.o			\
		$(O)/MenuSystem.o		\
		$(O)/N64.o		\
		$(O)/N64_Video.o			\
		$(O)/ParticleSystem.o		\
		$(O)/Player.o			\
		$(O)/Render.o			\
		$(O)/Sound.o			\
		$(O)/Weapon.o			\
		$(O)/Z_Zip.o			\
		$(O)/Z_Zone.o			


$(PROG_NAME).z64: $(PROG_NAME).elf FILESYSTEM.dfs
	$(OBJCOPY) $(PROG_NAME).elf $(PROG_NAME).bin -O binary
	rm -f $(PROG_NAME).z64
	$(N64TOOL) -l 8M -t $(PROG_NAME) -h $(HEADERPATH)/$(HEADERNAME) -o $(PROG_NAME).z64 $(PROG_NAME).bin -s 1M FILESYSTEM.dfs
	$(CHKSUM64PATH) $(PROG_NAME).z64

$(PROG_NAME).elf : $(OBJS).o
	$(LD) -o $(PROG_NAME).elf $(PROG_NAME).o $(LINK_FLAGS)

copy: $(PROG_NAME).z64
	cp $(PROG_NAME).z64 ~/public_html/

FILESYSTEM.dfs:
	$(MKDFSPATH) FILESYSTEM.dfs ./filesystem/

all: $(PROG_NAME).z64

$(O)/%.o:	%.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.z64 *.elf *.o *.bin *.dfs
