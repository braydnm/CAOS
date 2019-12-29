%ifndef GDT_INCLUDE
%define GDT_INCLUDE

[bits 16]

installGDT:
    cli
    pusha
    lgdt [gdt_descriptor]
    sti
    popa
    ret

gdt_start:

; null start(don't ask me why but it has to be here)
gdt_null:
    dd 0x0
    dd 0x0

; code segement register
gdt_code:
    ; base of code segment = 0x0, limit of segment is 0xfffff
    ; 1 st flags : ( present )1 ( privilege )00 ( descriptor type )1 -> 1001 b
    ; type flags : ( code )1 ( conforming )0 ( readable )1 ( accessed )0 -> 1010 b
    ; 2nd flags : ( granularity )1 (32 - bit default )1 (64 - bit seg )0 ( AVL )0 -> 1100 b
    dw 0xffff ; limit
    dw 0x0 ; base (bits 0-15)
    db 0x0 ; base (bits 16-23)
    db 10011010b ; 1st flags
    db 11001111b ; 2nd flags
    db 0x0 ; Base (bits 24 -31)

gdt_data: ; the data segment descriptor
    ; Same as code segment except for the type flags :
    ; type flags : ( code )0 ( expand down )0 ( writable )1 ( accessed )0 -> 0010 b
    dw 0xffff ;Limit(bits 0 -15)
    dw 0x0 ;Base(bits 0 -15)
    db 0x0 ;Base ( bits 16 -23)
    db 10010010b ;1st flags
    db 11001111b ;2nd flags
    db 0x0 ;Base ( bits 24 -31)

; used to calculate gdt size
gdt_end:

; descriptor for use
gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; used to hop segments
CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start


%endif