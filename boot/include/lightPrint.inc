%ifndef PRINT_INCLUDE
%define PRINT_INCLUDE
[bits 16]

%macro print16 1
    mov si, %1
    call _print16
    mov si, newLine
    call _print16
%endmacro

_print16:
    pusha
    _printLoop:
        lodsb
        or al, al
        jz _print16Done
        mov ah, 0x0e
        int 0x10
        jmp _printLoop
_print16Done:
    popa
    ret

newLine db 0x0d, 0x0a, 0x00

%endif