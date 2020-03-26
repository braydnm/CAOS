[org 0x0]
[bits 16]

start: jmp main

filesystem:
    filesystem.oem:                 db "CAOS"
    filesystem.bytesPerSector:      dw 512
    filesystem.sectorsPerCluster:   db 1
    filesystem.reservedSector:      dw 1
    filesystem.numberOfFATs:        db 2
    filesystem.rootEntries:         dw 224
    filesystem.totalSectors:        dw 2880
    filesystem.media:               db 0xf0
    filesystem.sectorsPerFAT:       dw 9
    filesystem.sectorsPerTrack:     dw 18
    filesystem.headsPerCylinder:    dw 2
    filesystem.hiddenSectors:       dd 0
    filesystem.totalSectorsBig:     dd 0
    filesystem.driveNumber:         db 0
    filesystem.unused:              db 0
    filesystem.extBootSignature:    db 0x29
    filesystem.serialNumber:        dd 0xc001c0d3
    filesystem.volumeLabel:         db "BRAYDN OS  "
    filesystem.fileSystem:          db "FAT12   "


%include "include/lightPrint.inc"

absoluteSector db 0x00
absoluteHead db 0x00
absoluteTrack db 0x00

chsToLBA:
    sub ax, 0x0002
    xor cx, cx
    mov cl, byte[filesystem.sectorsPerCluster]
    mul cx
    add ax, word[dataSector]
    ret

lbaToCHS:
    xor dx, dx
    div word[filesystem.sectorsPerTrack]
    inc dl
    mov byte[absoluteSector], dl
    xor dx, dx
    div word[filesystem.headsPerCylinder]
    mov byte[absoluteHead], dl
    mov byte[absoluteTrack], al
    ret

readSectors:
    .main
        mov di, 0x0005
    .readLoop
        push ax
        push bx
        push cx
        call lbaToCHS
        mov ah, 0x02
        mov al, 0x01
        mov ch, byte[absoluteTrack]
        mov cl, byte[absoluteSector]
        mov dh, byte[absoluteHead]
        mov dl, byte[filesystem.driveNumber]
        int 0x13
        jnc .goodRead
        xor ax, ax
        int 0x13
        dec di
        pop cx
        pop bx
        pop ax
        jnz .readLoop
        int 0x18
    .goodRead
        pop cx
        pop bx
        pop ax
        add bx, word[filesystem.bytesPerSector]
        inc ax
        loop .main
        ret

main:
    cli
    mov ax, 0x07C0
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ax, 0x0000
    mov ss, ax
    mov sp, 0xffff
    sti

    mov [filesystem.driveNumber], dl

    print16 greetings

    loadRoot:
        xor cx, cx
        xor dx, dx
        mov ax, 0x0020
        mul word[filesystem.rootEntries]
        div word[filesystem.bytesPerSector]
        xchg ax, cx

        mov al, byte[filesystem.numberOfFATs]
        mul word[filesystem.sectorsPerFAT]
        add ax, word[filesystem.reservedSector]
        mov word[dataSector], ax
        add word[dataSector], cx

        mov bx, 0x0200
        call readSectors

        mov cx, word[filesystem.rootEntries]
        mov di, 0x0200

        .searchLoop:
            push cx
            mov cx, 0x000b
            mov si, secondStage
            push di
            rep cmpsb
            pop di
            je loadFAT
            pop cx
            add di, 0x0020
            loop .searchLoop
            jmp failure
        
        loadFAT:
            mov dx, word[di+0x001a]
            mov word[cluster], dx

            xor ax, ax
            mov al, byte[filesystem.numberOfFATs]
            mul word[filesystem.sectorsPerFAT]
            mov cx, ax

            mov ax, word[filesystem.reservedSector]

            mov bx, 0x0200
            call readSectors

            mov ax, 0x0050
            mov es, ax
            mov bx, 0x0000
            push bx

        loadImage:
            mov ax, word[cluster]
            pop bx
            call chsToLBA
            xor cx, cx
            mov cl, byte[filesystem.sectorsPerCluster]
            call readSectors
            push bx

            mov ax, word[cluster]
            mov cx, ax
            mov dx, ax
            shr dx, 0x0001 ; divide by 2
            add cx, dx
            mov bx, 0x0200
            add bx, cx
            mov dx, word[bx]
            test ax, 0x0001
            jnz oddCluster
        
        evenCluster:
            and dx, 0000111111111111b
            jmp checkDone
        
        oddCluster:
            shr dx, 0x0004

        checkDone:
            mov word[cluster], dx
            cmp dx, 0x0ff0
            jb loadImage

        done:
            mov dl, byte[filesystem.driveNumber]
            push word 0x0050
            push word 0x0000
            retf
        
        failure:
            print16 failed
            mov ah, 0x00
            int 0x16
            int 0x19

greetings db "Welcome, loading stage 2", 0
failed db "Oops, something went wrong, hit any key to restart",0
dataSector dw 0x0000
cluster dw 0x0000
secondStage db "KRNLLDR SYS"

times 510-($-$$) db 0
dw 0xaa55