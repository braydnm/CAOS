%ifndef FAT12_INCLUDE
%define FAT12_INCLUDE

[bits 16]

%include "include/floppy.inc"
%define ROOT_OFFSET 0x2e00
%define FAT_SEG 0x2c0
%define ROOT_SEG 0x2e0

loadRoot:
    pusha
    push es

    xor cx, cx
    xor dx, dx
    mov ax, 32
    mul word[filesystem.rootEntries]
    div word[filesystem.bytesPerSector]
    xchg ax, cx

    mov al, byte[filesystem.numberOfFATs]
    mul word[filesystem.sectorsPerFAT]
    add ax, word[filesystem.reservedSectors]
    mov word[dataSector], ax
    add word[dataSector], cx

    push word ROOT_SEG
    pop es
    mov bx, 0
    call readSectors
    pop es
    popa
    ret

loadFAT:
    pusha
    push es

    xor ax, ax
    mov al, byte[filesystem.numberOfFATs]
    mul word[filesystem.sectorsPerFAT]
    mov cx, ax

    mov ax, word[filesystem.reservedSectors]

    push word FAT_SEG
    pop es
    xor bx, bx
    call readSectors
    pop es
    popa
    ret

findFile:
    push cx
    push dx
    push bx
    mov bx, si

    mov cx, word[filesystem.rootEntries]
    mov di, ROOT_OFFSET
    cld

    searchLoop:
        push cx
        mov cx, 11
        mov si, bx
        push di
        repe cmpsb
        pop di
        je found
        pop cx
        add di, 0x0020
        loop searchLoop
    notFound:
        pop bx
        pop dx
        pop cx
        mov ax, -1
        ret
    
    found:
        pop ax
        pop bx
        pop dx
        pop cx
        ret

loadFile:
    xor ecx, ecx
    push ecx

    searchFile:
        push bx
        push bp
        call findFile
        cmp ax, -1
        jne preLoadImage
        pop bp
        pop bx
        pop ecx
        mov ax, -1
        ret
    preLoadImage:
        sub edi, ROOT_OFFSET
        sub eax, ROOT_OFFSET

        push word ROOT_SEG
        pop es
        mov dx, word[es:di+0x001a]
        mov word[fileCluster], dx
        pop bx
        pop es
        push bx
        push es
        call loadFAT
    loadImage:
        mov ax, word[fileCluster]
        pop es
        pop bx
        call chsToLBA
        xor cx, cx
        mov cl, byte[filesystem.sectorsPerCluster]
        call readSectors
        pop ecx
        inc ecx
        push ecx
        push bx
        push es
        mov ax, FAT_SEG
        mov es, ax
        xor bx, bx

        mov ax, word[fileCluster]
        mov cx, ax
        mov dx, ax
        shr dx, 0x0001
        add cx, dx

        mov bx, 0
        add bx, cx
        mov dx, word[es:bx]
        test ax, 0x0001
        jnz oddCluster

        evenCluster:
            ; take low 12 bits
            and dx, 0000111111111111b
            jmp checkDone
        
        oddCluster:
            ; get high 12 bits
            shr dx, 0x0004

        checkDone:
            mov word[fileCluster], dx
            cmp dx, 0x0ff0
            jb loadImage

        done:
            pop es
            pop bx
            pop ecx
            xor ax, ax
            ret
%endif