global asm_cpu_vendor
global asm_rtc_gettime
global asm_kbd_active
global asm_kbd_getkey
global asm_getsp
global asm_getbp
global asm_printreg
global asm_sound
global asm_nosound
global asm_timertick
global asm_idle
global asm_move_rsp
global asm_initialize_stack

extern exc_printreg
extern kill_current_process

section .text

%macro push_state 0
   push rbx
   push rcx
   push rdx
   push rbp
   push rdi
   push rsi
   push r8
   push r9
   push r10
   push r11
   push r12
   push r13
   push r14
   push r15
%endmacro

%macro push_state_full 0
   push rax
   push_state
%endmacro

; devuelve el fabricante del cpu
asm_cpu_vendor:
    push rbp
    mov rbp, rsp

    push rbx

    mov rax, 0
    cpuid

    mov [rdi], ebx
    mov [rdi + 4], edx
    mov [rdi + 8], ecx

    mov byte [rdi+13], 0

    mov rax, rdi

    pop rbx

    mov rsp, rbp
    pop rbp
    ret

; devuelve el tiempo en rtc
asm_rtc_gettime:
    push rbp
    mov rbp,rsp

    mov rax,rdi     ; argumento de tiempo (0:secs, 2:mins, 4:hs, ...)
    out 70h,al
    in al,71h

    leave
    ret

; devuelve si hay una tecla presionada o liberada
asm_kbd_active:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,64h
    and al,01h

    leave
    ret

; devuelve el código de una tecla en el teclado (si fue presionada o soltada)
asm_kbd_getkey:
    push rbp
    mov rbp,rsp

    mov rax,0
    in al,60h

    leave
    ret

; devuelve el valor del stack-pointer
asm_getsp:
    mov rax,rsp
    ret

; devuelve el valor del base-pointer
asm_getbp:
    mov rax,rbp
    ret

; emite un sonido con cierta frecuencia y duración
asm_sound:
    push rbp
    mov rbp,rsp

    mov al,182         ; configuro altavoz
    out 43h,al

    mov ax,di          ; frecuencia en bx
    out 42h,al         ; envio byte - significativo
    mov al,ah
    out 42h,al         ; envio byte + significativo

    in al,61h
    or al,3h
    out 61h,al

    leave
    ret

; stopea el sonido que esté sonando
asm_nosound:
    push rbp
    mov rbp,rsp

    in al,61h
    and al,0fch
    out 61h,al

    leave
    ret

asm_timertick:
    int 0x20
    ret

asm_idle:
	hlt
	ret

asm_move_rsp:
    mov rsp, rdi
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq

asm_process_wrapper:
    call rdi
    mov rdi, rax
    call kill_current_process

asm_initialize_stack:
	mov r8, rsp 	
	mov r9, rbp		
	mov rsp, rdx 	
	mov rbp, rdx
	push 0x0
	push rdx
	push 0x202
	push 0x8
    push asm_process_wrapper
	mov rdi, rsi 		
	mov rsi, rcx		
	push_state_full
	mov rax, rsp
	mov rsp, r8
	mov rbp, r9
	ret