GCC=gcc
GPP=g++
LD=ld
AR=ar
ASM=nasm

GCCFLAGS=-m64 -g -fno-exceptions -std=c99 -Wall -ffreestanding -nostdlib -fno-pic -fno-common -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -fno-builtin-malloc -fno-builtin-free -fno-builtin-realloc -static
ARFLAGS=rvs
ASMFLAGS=-felf64 -g
