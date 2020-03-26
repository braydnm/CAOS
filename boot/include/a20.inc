fastA20Gate equ 0x92
a20TestCount equ 5

ioDelay:
    out 0x80, al
    ret

enableA20:
    call checkA20
    jnc doneEnabled
    call enableA20BIOS
    call checkA20
    jnc doneEnabled
    call enableA20Fast
    jnc doneEnabled
    call enableA20_8042
    call checkA20
doneEnabled:
    ret


enableA20BIOS:
    mov ax, 0x2401
    int 0x15
    ret

enableA20Fast:
    in al, fastA20Gate
    test al, 2
    jnz a20FastReturn
    or al, 2
    and al, 0xfe
    out fastA20Gate, al
a20FastReturn:
    ret

enableA20_8042:
    call empty8042
    mov al, 0xd1
    out 0x64, al
    call empty8042
    mov al, 0xdf
    out 0x60, al
    call empty8042
    mov al, 0xff
    out 0x64, al
    call empty8042
    ret

empty8042:
    call ioDelay
    in al, 0x64
    test al, 1
    jne noOutput
    call ioDelay
    in al, 0x60
    jmp empty8042
noOutput:
    test al, 2
    jne empty8042
    ret

checkA20:
    push ds
    push es
    xor ax, ax
    mov ds, ax
    mov ax, 0xffff
    mov es, ax
    mov ax, [0x1000]
    mov cx, a20TestCount

checkLoop:
    call ioDelay
    mov bx, [0x1000]
    cmp bx, [es:0x1010]
    jne a20Enabled
    inc bx
    mov [0x1000], bx
    cmp bx, [es:0x1010]
    jne a20Enabled
    dec cx
    jnz checkLoop
    stc ; check if still disabled
    jmp a20Return

a20Enabled:
    clc

a20Return:
    mov [0x1000], ax
    pop es
    pop ds
    ret