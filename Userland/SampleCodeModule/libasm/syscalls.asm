; i/o interaction
global asm_getchar
global asm_putchar
sys_read         equ 1
sys_write        equ 2

; drawing
global asm_draw
global asm_clear
global asm_cursor
global asm_show_cursor
sys_draw         equ 3
sys_clear        equ 4
sys_cursor       equ 5
sys_show_cursor  equ 6

; properties
global asm_winprops
sys_winprops     equ 7

; system
global asm_ticked
global asm_sleep
global asm_printreg
global asm_datetime
global asm_sound
sys_ticks        equ 8
sys_sleep        equ 9
sys_regs         equ 10
sys_rtc          equ 11
sys_sound        equ 12

; memory
global asm_malloc
global asm_free
global asm_total_heap
global asm_free_heap
global asm_used_heap
sys_malloc      equ 13
sys_free        equ 14
sys_total_heap  equ 15
sys_free_heap   equ 16
sys_used_heap   equ 17

; process
global asm_init_process
global asm_kill_process
global asm_kill_current_process
global asm_get_snapshots_info
global asm_get_current_id
global asm_block_process
global asm_unblock_process
sys_init_process                    equ 18
sys_kill_process                    equ 19
sys_kill_current_process            equ 20
sys_get_snapshots_info              equ 21
sys_get_current_id                  equ 22
sys_block_process                   equ 23
sys_unblock_process                 equ 24

%macro syscall_handler 1
    push rbp
    mov rbp,rsp
    mov r9,r8
    mov r8,rcx
    mov rcx,rdx
    mov rdx,rsi
    mov rsi,rdi
    mov rdi,%1
    int 80h
    leave
    ret
%endmacro

asm_getchar:
    syscall_handler sys_read

asm_putchar:
    syscall_handler sys_write

asm_draw:
    syscall_handler sys_draw

asm_clear:
    syscall_handler sys_clear

asm_cursor:
    syscall_handler sys_cursor

asm_show_cursor:
    syscall_handler sys_show_cursor

asm_winprops:
    syscall_handler sys_winprops

asm_ticked:
    syscall_handler sys_ticks

asm_sleep:
    syscall_handler sys_sleep

asm_printreg:
    syscall_handler sys_regs

asm_datetime:
    syscall_handler sys_rtc

asm_sound:
    syscall_handler sys_sound

asm_malloc:
    syscall_handler sys_malloc

asm_free:
    syscall_handler sys_free

asm_total_heap:
    syscall_handler sys_total_heap

asm_free_heap:
    syscall_handler sys_free_heap

asm_used_heap:
    syscall_handler sys_used_heap

asm_init_process:
    syscall_handler sys_init_process

asm_kill_process:
    syscall_handler sys_kill_process

asm_kill_current_process:
    syscall_handler sys_kill_current_process

asm_get_snapshots_info:
    syscall_handler sys_get_snapshots_info

asm_get_current_id:
    syscall_handler sys_get_current_id

asm_block_process:
    syscall_handler sys_block_process

asm_unblock_process:
    syscall_handler sys_unblock_process