include Makefile.inc

KERNEL=kernel.bin
KERNEL_ELF=$(KERNEL:.bin=.elf)

SOURCES=$(wildcard *.c)
SOURCES_DRIVERS=$(wildcard drivers/*.c)
SOURCES_IDT=$(wildcard idt/*.c)
SOURCES_LIBASM=$(wildcard libasm/*.asm)
SOURCES_LIBC=$(wildcard libc/*.c)

OBJECTS=$(SOURCES:.c=.o)
OBJECTS_DRIVERS=$(SOURCES_DRIVERS:.c=.o)
OBJECTS_IDT=$(SOURCES_IDT:.c=.o)
OBJECTS_LIBASM=$(SOURCES_LIBASM:.asm=.o)
OBJECTS_LIBC=$(SOURCES_LIBC:.c=.o)

LOADERSRC=loader.asm
LOADEROBJ=$(LOADERSRC:.asm=.o)
STATICLIBS=

all: $(KERNEL)

debug: all
	$(LD) $(LDFLAGS) -T kernel.ld --oformat=elf64-x86-64 -o $(KERNEL_ELF) $(LOADEROBJ) $(STATICLIBS) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_LIBASM) $(OBJECTS_LIBC)

$(KERNEL): $(LOADEROBJ) $(STATICLIBS) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_LIBASM) $(OBJECTS_LIBC)
	$(LD) $(LDFLAGS) -T kernel.ld -o $(KERNEL) $(LOADEROBJ) $(STATICLIBS) $(OBJECTS) $(OBJECTS_DRIVERS) $(OBJECTS_IDT) $(OBJECTS_LIBASM) $(OBJECTS_LIBC)

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include $(MM) -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

$(LOADEROBJ):
	$(ASM) $(ASMFLAGS) $(LOADERSRC) -o $(LOADEROBJ)

clean:
	rm -rf *.bin *.elf *.o drivers/*.o idt/*.o libasm/*.o libc/*.o

.PHONY: all clean
