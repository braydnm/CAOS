; loaded to 0x50:0
; so our org is 0x500
[org 0x500]
[bits 16]

start: jmp main

%include "include/print.inc"
%include "include/gdt.inc"
%include "include/a20.inc"
%include "include/fat12.inc"
%include "include/common.inc"
%include "include/memory.inc"
%include "include/bootinfo.inc"

bootInfo:
istruc multibootInfo
	at multibootInfo.flags,			dd 0
	at multibootInfo.memoryLo,			dd 0
	at multibootInfo.memoryHi,			dd 0
	at multibootInfo.bootDevice,		dd 0
	at multibootInfo.cmdLine,			dd 0
	at multibootInfo.modsCount,		dd 0
	at multibootInfo.modsAddr,		dd 0
	at multibootInfo.syms0,			dd 0
	at multibootInfo.syms1,			dd 0
	at multibootInfo.syms2,			dd 0
	at multibootInfo.mmapLength,		dd 0
	at multibootInfo.mmapAddr,		dd 0
	at multibootInfo.drivesLength,	dd 0
	at multibootInfo.drivesAddr,		dd 0
	at multibootInfo.configTable,		dd 0
	at multibootInfo.bootloaderName,	dd 0
	at multibootInfo.apmTable,		dd 0
	at multibootInfo.vbeControlInfo,	dd 0
	at multibootInfo.vbeModeInfo,	dw 0
	at multibootInfo.vbeInterfaceSeg,dw 0
	at multibootInfo.vbeInterfaceOff,dw 0
	at multibootInfo.vbeInterfaceLen,dw 0
iend

main:
    mov [filesystem.driveNumber], dl
    cli
    xor ax,ax
    mov ds, ax
    mov es, ax
    mov ax, 0x0
    mov ss, ax
    mov sp, 0xffff
    sti
    
    print16 line
    print16 success
    
    call installGDT
    print16 gdtInstalled

    call enableA20
    print16 a20EnabledMsg

    xor eax, eax
    xor ebx, ebx
    call getExtendedMemoryPassed64
    print16 gotMemorySize

    mov word[bootInfo+multibootInfo.memoryHi], bx
    mov word[bootInfo+multibootInfo.memoryLo], ax

    mov eax, 0x0
    mov ds, ax
    mov di, 0x1000
    call getMemoryMap

    print16 loading
    call loadRoot
    print16 loadedRoot
    
    mov ebx, 0
    mov bp, kernelRealModeBase
    mov si, kernelName
    call loadFile
    mov dword[kernelSize], ecx
    cmp ax, 0
    je enter32BitMode
    print16 failure
    mov ah, 0x00
    int 0x16
    int 0x19
    cli
    hlt

enter32BitMode:
    cli
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEG:protectedMode

success db "Second stage is in the house",0
line db "---------------------------", 0
a20EnabledMsg db "A20 is enabled", 0
gdtInstalled db "The global descriptor table has been initialized", 0
loading db "Loading root", 0
loadedRoot db "Loaded root", 0
failure db "Corrupt or missing kernel. Press to reboot", 0
gotFile db "Got the kernel", 0
gotMemorySize db "Got the size of the memory on the computer", 0
mappedMemory db "Mapped the memory of the computer", 0


[bits 32]
protectedMode:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000
    
copyKernelIntoMem:
    mov eax, dword[kernelSize]
    movzx ebx, word[filesystem.bytesPerSector]
    mul ebx
    mov ebx, 4
    div ebx
    cld
    mov esi, kernelRealModeBase
    mov edi, kernelProtectedModeBase
    mov ecx, eax
    rep movsd

checkImage:
    call clearScreen
    cmp dword[kernelProtectedModeBase], magicNumber
    je execute
    print32 badExec
    cli
    hlt

execute:
    print32 goodExec
    mov eax, 0x2badb002
    mov ebx, 0
    mov edx, [kernelSize]
    mov ecx, dword bootInfo
    cli
    call kernelProtectedModeBase+0x4
    cli
    hlt

magicNumber equ 0x4b454e02
goodExec db "Found a valid executable for the kernel", 0
badExec db "The executable appears to be corrupt",0
entryPoint db "Estimating entry point at around ", 0
error db "There was error entering the kernel", 0