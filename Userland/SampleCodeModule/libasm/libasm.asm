global asm_testioe
global asm_testzde

asm_testioe:
    mov cr6,rax
    ret

asm_testzde:
    mov eax,10
    mov ebx,0
    div ebx
    ret
