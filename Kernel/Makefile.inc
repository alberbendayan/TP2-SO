GCC=gcc
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-m64 -g -fno-exceptions -fno-asynchronous-unwind-tables -fno-pic -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -mno-red-zone -Wall -ffreestanding -nostdlib -fno-common -std=c99 -static
ARFLAGS=rvs
ASMFLAGS=-felf64 -g
LDFLAGS=--warn-common -z max-page-size=0x1000
