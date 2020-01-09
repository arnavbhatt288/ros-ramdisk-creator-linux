CC=gcc
CFLAGS=-I.
DEPS = install.h volume.h copy.h ini.h fat.h fat32.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ramdisk-creator: main.o install.o volume.o copy.o ini.o fat.o fat32.o
	$(CC) -o ramdisk-creator main.o install.o volume.o copy.o ini.o fat.o fat32.o