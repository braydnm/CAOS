%ifndef FLOPPY_INCLUDE
%define FLOPPY_INCLUDE

[bits 16]

filesystem:
    filesystem.oem:                 db "CAOS"
    filesystem.bytesPerSector:      dw 512
    filesystem.sectorsPerCluster:   db 1
    filesystem.reservedSectors:      dw 1
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
    filesystem.volumeLabel:         db "BRAYDN-OS  "
    filesystem.fileSystem:          db "FAT12   "
 
dataSector dw 0  ; Stores our data sector
fileCluster  dw 0   ; Stores the cluster of the strap file
absoluteSector db 0x00
absoluteHead db 0x00
absoluteTrack db 0x00

; ax contains LBA address
; returns to chs variables
; sector = (LBA%sectorsPerTrack)+1
; head = (LBA/sectorsPerTrack)%headsPerCylinder
; track = LBA/(sectorsPerTrack*numHeads)
lbaToCHS:
    xor dx, dx
    div word[filesystem.sectorsPerTrack]   ; mod is calculated in dl  
    inc dl
    mov byte[absoluteSector], dl

    xor dx, dx
    div word[filesystem.headsPerCylinder]
    mov byte[absoluteHead], dl

    ; because of some weird voodoo math with how registers work we already have our 3rd answer
    mov byte[absoluteTrack], al
    ret

; @param ax = cluster address
; @return ax = start
; @return cx = length
; equation = lba = (Cluster-2)*sectorsPerCluster
chsToLBA:
    sub ax, 2
    xor cx, cx
    mov cl, byte[filesystem.sectorsPerCluster]
    mul cx
    add ax, word[dataSector]

    ret

; @param ax = start of sector
; @param es:bx = address to read to
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

%endif