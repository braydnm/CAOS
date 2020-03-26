%ifndef PRINT_INCLUDE
%define PRINT_INCLUDE
[bits 16]

%macro print16 1
    mov si, %1
    call _print16
    mov si, newLine
    call _print16
%endmacro

newLine16:
    mov si, newLine

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

[bits 32]

%define VIDEOMEM 0xB8000
%define NUM_COLS 80
%define NUM_ROWS 25
%define CHAR_COLORS 0x07 ;63 ; white text on light blue background

cursorX db 0
cursorY db 0

%macro printChar32 1
    mov bl, %1
    call _printChar32
%endmacro

%macro print32 1
    push ebx
    mov ebx, %1
    call _print32
    pop ebx
%endmacro

%macro printHex 1
    mov edx, %1
    call _printHex
%endmacro

%macro newline 0
    mov bl, 0x0A
    call _printChar32
%endmacro

moveCursor:
    pusha
    
    xor eax, eax
    mov ecx, NUM_COLS
    mov al, bh
    mul ecx
    add al, bl
    mov ebx, eax

    ; update cursor position
    mov al, 0x0f
    mov dx, 0x03d4
    out dx, al 

    mov al, bl
    mov dx, 0x03d5
    ; send low byte to hardware
    out dx, al

    xor eax, eax

    mov al, 0x0e
    mov dx, 0x03d4
    out dx,al

    mov al, bh
    mov dx, 0x03d5
    out dx, al

    popa
    ret

clearScreen:
    pusha
    cld
    mov edi, VIDEOMEM
    mov cx, 2000
    mov ah, CHAR_COLORS
    mov al, ' '
    rep stosw

    mov byte[cursorX], 0
    mov byte[cursorY], 0
    popa
    ret

moveTo:
    pusha
    mov byte[cursorX], al
    mov byte[cursorY],ah
    popa
    ret

_printChar32:
    pusha
    mov edi, VIDEOMEM
    xor eax, eax

    mov ecx, NUM_COLS*2
    mov al, byte[cursorY]
    mul ecx
    push eax

    mov al, byte[cursorX]
    mov cl, 2
    mul cl
    pop ecx
    add eax, ecx

    xor ecx, ecx
    add edi, eax

    cmp bl, 0x0A
    je newRow

    mov dl, bl
    mov dh, CHAR_COLORS
    mov word[edi], dx

    inc byte[cursorX]
    cmp byte[cursorX], NUM_COLS
    jne putChar32Done

newRow:
    mov byte[cursorX], 0
    inc byte[cursorY]

putChar32Done:
    popa
    ret

_print32:
    pusha
    push ebx ; save string address
    pop edi

    printLoop:
        mov bl, byte[edi]
        cmp bl, 0
        je _print32Done

        call _printChar32

        inc edi
        jmp printLoop
    
_print32Done:
    mov bh, byte[cursorY]
    mov bl, byte[cursorX]
    call moveCursor
    popa
    ret

; Prints the value of DX as hex.
_printHex:
  pusha             ; save the register values to the stack for later

  mov cx,8          ; Start the counter: we want to print 4 characters
                    ; 4 bits per char, so we're printing a total of 16 bits

charLoop:
  dec cx            ; Decrement the counter

  mov eax,edx         ; copy bx into ax so we can mask it for the last chars
  shr edx,4          ; shift bx 4 bits to the right
  and eax,0xf        ; mask ah to get the last 4 bits

  mov bx, HEX_OUT   ; set bx to the memory address of our string
  add bx, 2         ; skip the '0x'
  add bx, cx        ; add the current counter to the address

  cmp ax,0xa        ; Check to see if it's a letter or number
  jl setLetter     ; If it's a number, go straight to setting the value
  add byte [bx],7   ; If it's a letter, add 7
                    ; Why this magic number? ASCII letters start 17
                    ; characters after decimal numbers. We need to cover that
                    ; distance. If our value is a 'letter' it's already
                    ; over 10, so we need to add 7 more.
  jl setLetter

setLetter:
  add byte [bx],al  ; Add the value of the byte to the char at bx

  cmp cx,0          ; check the counter, compare with 0
  je hexDone ; if the counter is 0, finish
  jmp charLoop     ; otherwise, loop again

hexDone:
  print32 HEX_OUT

  popa              ; pop the initial register values back from the stack
  ret               ; return the function

; global variables
HEX_OUT: db '0x00000000',0


%endif
