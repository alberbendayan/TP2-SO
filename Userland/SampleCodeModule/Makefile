include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
MODULE_ELF=$(MODULE:.bin=.elf)

SOURCES=$(wildcard *.c)
SOURCES_LIBASM=$(wildcard libasm/*.asm)
SOURCES_LIBC=$(wildcard libc/*.c)
SOURCES_PROGRAMS=$(wildcard programs/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_LIBASM=$(SOURCES_LIBASM:.asm=.o)
OBJECTS_LIBC=$(SOURCES_LIBC:.c=.o)
OBJECTS_PROGRAMS=$(SOURCES_PROGRAMS:.c=.o)

all: $(MODULE)

debug: all
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 -o ../$(MODULE_ELF) $(OBJECTS) $(OBJECTS_LIBASM) $(OBJECTS_LIBC) $(OBJECTS_PROGRAMS)

$(MODULE): $(OBJECTS) $(OBJECTS_LIBASM) $(OBJECTS_LIBC) $(OBJECTS_PROGRAMS)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld -o ../$(MODULE) $(OBJECTS) $(OBJECTS_LIBASM) $(OBJECTS_LIBC) $(OBJECTS_PROGRAMS)

%.o : %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o libasm/*.o libc/*.o programs/*.o

.PHONY: all clean print
