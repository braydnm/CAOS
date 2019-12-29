%ifndef MEMORY_INCLUDE
%define MEMORY_INCLUDE

; Types of addresses
; 1. Avaiable memory
; 2. Reserved
; 3. Usable but only after reading acpi
; 4. Must save between NVS sessions
; Everything else is undefines
struc MemoryMapEntry
    .baseAddress resq 1
    .length resq 1
    .type resd 1
    .acpi_null resd 1
endstruc

; @param es:bi where entries are stored
; @return bp contains number of entries
getMemoryMap:   
    pushad
    xor ebx, ebx
    xor bp, bp
    mov edx, 'PAMS' ; interpreted as SMAP
    mov eax, 0xe820
    mov ecx, 24
    int 0x15 ; get first
    jc memMapError
    cmp eax, 'PAMS' ; BIOS puts SMAP in eax
    jne memMapError
    test ebx, ebx ; if entry is one entry then exit
    je memMapError
    jmp memMapStart

    nextEntry:
        mov edx, 'PAMS'
        mov ecx, 24
        mov eax, 0xe820
        int 0x15
    memMapStart:
        jcxz skipEntry
    readEntry:
        mov ecx, [es:di + MemoryMapEntry.length]
        test ecx, ecx ; if the length is then skip
        jne short goodEntry
        mov ecx, [es:di + MemoryMapEntry.length+4]; get length
        jecxz skipEntry ; if length is 0 then skip
    
    goodEntry:
        inc bp
        add di, 24 ; move to next entry in buffer
    skipEntry:
        cmp ebx, 0
        jne nextEntry
        jmp doneMemMap
    memMapError:
        stc
    doneMemMap:
        popad
        ret


getMemorySize:
    int 0x12
    jc error
    test ax,ax
    je error
    cmp ah, 0x86
    je error
    ret

getExtendedMemory:
    mov ax, 0x88
    int 0x15
    jc .error
    test ax,ax
    je .error
    cmp ah, 0x86
    je .error
    cmp ah, 0x80
    je .error
    ret
.error:
    mov ax, -1
    ret

getExtendedMemoryPassed64:
    push ecx
    push edx
    xor ecx, ecx
    xor edx, edx
    mov ax, 0xe801
    int 0x15
    jc .error
    cmp ah, 0x86
    je .error
    cmp ah, 0x0
    je .error
    jcxz .use_ax ; check if cx is zero for use as the bios could put the value in ax, bx or cx, dx
    mov ax, cx
    mov bx, dx

.use_ax:
    pop edx
    pop ecx
    ret

.error:
    mov ax, -1
    mov bx, 0
    pop edx
    pop ecx
    ret

getExtendedMemoryPassed64_32:
    push ecx
    push edx
    xor ecx, ecx
    xor edx, edx
    mov ax, 0xe881
    int 0x15
    jc .error
    cmp ah, 0x86
    je .error
    cmp ah, 0x0
    je .error
    jcxz .use_ax ; check if cx is zero for use as the bios could put the value in ax, bx or cx, dx
    mov ax, cx
    mov bx, dx

.use_ax:
    pop edx
    pop ecx
    ret

.error:
    mov ax, -1
    mov bx, 0
    pop edx
    pop ecx
    ret


%endif