GLOBAL  _read_msw,_lidt
GLOBAL  _int_80_hand
GLOBAL  _int_09_hand
GLOBAL  _int_08_hand
GLOBAL  _call_int_08
GLOBAL  _restart_PC
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug
GLOBAL _wrapper_int_80
GLOBAL _screenCursorPos
GLOBAL  CPU_Speed
GLOBAL	_Halt
GLOBAL	_out
GLOBAL	_inw
GLOBAL  _outw
GLOBAL	_in
GLOBAL 	_inb
GLOBAL	_outb

EXTERN  int_08
EXTERN  int_09
EXTERN  service_Read
EXTERN  service_Write
EXTERN 	int_80
EXTERN	int_81
EXTERN _saveContext
EXTERN _getNewContext



SECTION .text

_Halt:
	hlt
	ret


_Cli:
	cli			; Cleans interruption flags
	ret

_Sti:

	sti			; Enables interruption flags
	ret

_mascaraPIC1:			; Writes PIC 1 mask
		push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = 16 bits mask
        out	21h,al
        pop     ebp
        retn

_mascaraPIC2:			; Writes PIC 2 mask
		push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = 16 bits mask
        out	0A1h,al
        pop     ebp
        retn

_read_msw:
        smsw    ax		; Gets the Machine Status Word
        retn


_lidt:				; Loads IDTR
        push    ebp
        mov     ebp, esp
        push    ebx
        mov     ebx, [ss: ebp + 6] ; ds:bx = IDTR pointer
		rol	ebx,16		    	
		lidt    [ds: ebx]          ; Loads IDTR
        pop     ebx
        pop     ebp
        retn

_call_int_08:
	push ebp
	mov ebp,esp
	int 08h
	mov	esp,ebp
	pop	ebp
	ret

_int_08_hand:				; INT 8 Handler(Timer Tick)
	cli
	pushad
		mov eax, esp
		push eax
			call _saveContext
		pop eax
		;call _GetTemporaryESP
		;mov esp, eax
		;push eax
		call _getNewContext
			;call _LoadESP
		;pop ebx
		mov esp,eax
		;call _debug;
	popad
	mov al,20h			; Envio de EOI generico al PIC
	out 20h,al

	sti

	iret
		
_int_09_hand:				; INT 9 Handler (Keyboard)
			push ebp 		; Does StackFrame
			mov ebp,esp
			pusha
			
			mov eax,00h

			in al, 60h 		; Reads ScanCode from Keyboard
			mov ah, 00h	

			push eax;		; Parameter for C function

			call int_09		; Calls C function

			pop eax;

			mov	al,20h		; EOI to PIC
			out	20h,al
			
			popa
			mov	esp,ebp		; Breaks up StackFrame
			pop	ebp
			
			iret

_wrapper_int_80:
		push ebp
		mov	ebp,esp
		pusha
		
		mov	ecx,00h
	
		; Loads parameters from C
		mov	ecx,[ebp+8]		; Loads the operation
		;mov	ecx,[ebp+12]	; Loads data
		;mov	ebx,[ebp+16]	; Loads the file descriptor
		
		int 80h
		
		popa
		mov	esp,ebp
		pop	ebp
		ret			
						
_int_80_hand:
			cli				; INT 80 Handler (System Call)
			push ebp		; Does StackFrame
			mov ebp, esp	
			pusha	
			
			
			; Push parameters to int_80 in C
			;push ebx
			;push ecx
			push ecx
			
			call int_80

			pop ecx
			;pop ecx
			;pop ebx

			popa
			mov	esp,ebp			; Breaks up StackFrame
			pop	ebp

			sti
			iret			

; Warm boot, taken from:
; http://www.uv.tietgen.dk/staff/mlha/pc/prog/asm/index.htm
_restart_PC:	
			mov ax,40h
			mov ds,ax
			mov ax, 1234h ; Warm boot flag, bypass Memory Test
			mov [72h], ax
			jmp 0FFFFh:0000h ;At FFFF:0000 is the intruction
								; JMP F000:E05B

				
_screenCursorPos:
			push ebp
			mov ebp, esp
			mov dx, [ebp+8]
			mov al, [ebp+12]
			out dx, al
			mov esp, ebp
			pop ebp
			ret		
			

CPU_Speed:
			push ebp
			mov ebp, esp
			
			rdtsc
			
			mov esp, ebp
			pop ebp
			
			ret		
				
_debug:
        push    bp
        mov     bp, sp
        push	ax

vuelve:	mov     ax, 1
        cmp	ax, 0
		jne	vuelve
		pop	ax
		pop     bp
        retn

_out:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]   	; Puerto
		mov		eax, [ebp+12]  	; Lo que se va a mandar
		out		dx, al
		pop		ebp
		ret

_inw:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]    ; Puerto
		mov		eax, 0          ; Limpio eax
		in		ax, dx
		pop		ebp
		ret

_outw:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]   	; Puerto
		mov		eax, [ebp+12]  	; Lo que se va a mandar
		out		dx, ax
		pop		ebp
		ret

_in:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]    ; Puerto
		mov		eax, 0          ; Limpio eax
		in		al, dx
		pop		ebp
		ret

_inb:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]    ; Puerto
		mov		eax, 0          ; Limpio eax
		in byte		al, dx
		pop		ebp
		ret

_outb:
		push	ebp
		mov		ebp, esp		; Stack frame
		mov		edx, [ebp+8]   	; Puerto
		mov		eax, [ebp+12]  	; Lo que se va a mandar
		out	dx, al
		pop		ebp
		ret

