; Defined in error.c
[extern errorHandler]
[extern interruptServiceRoutineHandler]

global idtLoad
extern reg
idtLoad:
	lidt [reg]
	ret

; Common error code
errorMain:
    ; 1. Save CPU state
	pusha ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds ; Lower 16-bits of eax = ds.
	push eax ; save the data segment descriptor
	mov ax, 0x10  ; kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

    ; 2. Call C handler
	call errorHandler

    ; 3. Restore state
	pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8 ; Cleans up the pushed error code and pushed error number
	sti
	iret ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; saves everything to the stack and calls the handler method
interruptServiceRoutineMain:
    pusha
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    call interruptServiceRoutineHandler ; Different than the ISR code
    pop ebx  ; Different than the ISR code
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    sti
    iret

; We don't get information about which interrupt was caller
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; First make the errors global
global error0
global error1
global error2
global error3
global error4
global error5
global error6
global error7
global error8
global error9
global error10
global error11
global error12
global error13
global error14
global error15
global error16
global error17
global error18
global error19
global error20
global error21
global error22
global error23
global error24
global error25
global error26
global error27
global error28
global error29
global error30
global error31

global interruptServiceRoutine0
global interruptServiceRoutine1
global interruptServiceRoutine2
global interruptServiceRoutine3
global interruptServiceRoutine4
global interruptServiceRoutine5
global interruptServiceRoutine6
global interruptServiceRoutine7
global interruptServiceRoutine8
global interruptServiceRoutine9
global interruptServiceRoutine10
global interruptServiceRoutine11
global interruptServiceRoutine12
global interruptServiceRoutine13
global interruptServiceRoutine14
global interruptServiceRoutine15
global syscallRoutine

; 0: Divide By Zero Exception
error0:
    cli
    push byte 0
    push byte 0
    jmp errorMain

; 1: Debug Exception
error1:
    cli
    push byte 0
    push byte 1
    jmp errorMain

; 2: Non Maskable Interrupt Exception
error2:
    cli
    push byte 0
    push byte 2
    jmp errorMain

; 3: Int 3 Exception
error3:
    cli
    push byte 0
    push byte 3
    jmp errorMain

; 4: INTO Exception
error4:
    cli
    push byte 0
    push byte 4
    jmp errorMain

; 5: Out of Bounds Exception
error5:
    cli
    push byte 0
    push byte 5
    jmp errorMain

; 6: Invalid Opcode Exception
error6:
    cli
    push byte 0
    push byte 6
    jmp errorMain

; 7: Coprocessor Not Available Exception
error7:
    cli
    push byte 0
    push byte 7
    jmp errorMain

; 8: Double Fault Exception (With Error Code!)
error8:
    cli
    push byte 8
    jmp errorMain

; 9: Coprocessor Segment Overrun Exception
error9:
    cli
    push byte 0
    push byte 9
    jmp errorMain

; 10: Bad TSS Exception (With Error Code!)
error10:
    cli
    push byte 10
    jmp errorMain

; 11: Segment Not Present Exception (With Error Code!)
error11:
    cli
    push byte 11
    jmp errorMain

; 12: Stack Fault Exception (With Error Code!)
error12:
    cli
    push byte 12
    jmp errorMain

; 13: General Protection Fault Exception (With Error Code!)
error13:
    cli
    push byte 13
    jmp errorMain

; 14: Page Fault Exception (With Error Code!)
error14:
    cli
    push byte 14
    jmp errorMain

; 15: Reserved Exception
error15:
    cli
    push byte 0
    push byte 15
    jmp errorMain

; 16: Floating Point Exception
error16:
    cli
    push byte 0
    push byte 16
    jmp errorMain

; 17: Alignment Check Exception
error17:
    cli
    push byte 0
    push byte 17
    jmp errorMain

; 18: Machine Check Exception
error18:
    cli
    push byte 0
    push byte 18
    jmp errorMain

; 19: Reserved
error19:
    cli
    push byte 0
    push byte 19
    jmp errorMain

; 20: Reserved
error20:
    cli
    push byte 0
    push byte 20
    jmp errorMain

; 21: Reserved
error21:
    cli
    push byte 0
    push byte 21
    jmp errorMain

; 22: Reserved
error22:
    cli
    push byte 0
    push byte 22
    jmp errorMain

; 23: Reserved
error23:
    cli
    push byte 0
    push byte 23
    jmp errorMain

; 24: Reserved
error24:
    cli
    push byte 0
    push byte 24
    jmp errorMain

; 25: Reserved
error25:
    cli
    push byte 0
    push byte 25
    jmp errorMain

; 26: Reserved
error26:
    cli
    push byte 0
    push byte 26
    jmp errorMain

; 27: Reserved
error27:
    cli
    push byte 0
    push byte 27
    jmp errorMain

; 28: Reserved
error28:
    cli
    push byte 0
    push byte 28
    jmp errorMain

; 29: Reserved
error29:
    cli
    push byte 0
    push byte 29
    jmp errorMain

; 30: Reserved
error30:
    cli
    push byte 0
    push byte 30
    jmp errorMain

; 31: Reserved
error31:
    cli
    push byte 0
    push byte 31
    jmp errorMain

interruptServiceRoutine0:
	cli
	push byte 0
	push byte 32
	jmp interruptServiceRoutineMain

interruptServiceRoutine1:
	cli
	push byte 1
	push byte 33
	jmp interruptServiceRoutineMain

interruptServiceRoutine2:
	cli
	push byte 2
	push byte 34
	jmp interruptServiceRoutineMain

interruptServiceRoutine3:
	cli
	push byte 3
	push byte 35
	jmp interruptServiceRoutineMain

interruptServiceRoutine4:
	cli
	push byte 4
	push byte 36
	jmp interruptServiceRoutineMain

interruptServiceRoutine5:
	cli
	push byte 5
	push byte 37
	jmp interruptServiceRoutineMain

interruptServiceRoutine6:
	cli
	push byte 6
	push byte 38
	jmp interruptServiceRoutineMain

interruptServiceRoutine7:
	cli
	push byte 7
	push byte 39
	jmp interruptServiceRoutineMain

interruptServiceRoutine8:
	cli
	push byte 8
	push byte 40
	jmp interruptServiceRoutineMain

interruptServiceRoutine9:
	cli
	push byte 9
	push byte 41
	jmp interruptServiceRoutineMain

interruptServiceRoutine10:
	cli
	push byte 10
	push byte 42
	jmp interruptServiceRoutineMain

interruptServiceRoutine11:
	cli
	push byte 11
	push byte 43
	jmp interruptServiceRoutineMain

interruptServiceRoutine12:
	cli
	push byte 12
	push byte 44
	jmp interruptServiceRoutineMain

interruptServiceRoutine13:
	cli
	push byte 13
	push byte 45
	jmp interruptServiceRoutineMain

interruptServiceRoutine14:
	cli
	push byte 14
	push byte 46
	jmp interruptServiceRoutineMain

interruptServiceRoutine15:
	cli
	push byte 15
	push byte 47
	jmp interruptServiceRoutineMain

syscallRoutine:
    cli
    push eax
    push byte 80
    jmp interruptServiceRoutineMain

global IRQ_START
global IRQ_END

IRQ_START:
    add esp, 8
    pusha

IRQ_END:
    popa
    iret
