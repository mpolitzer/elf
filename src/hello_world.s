.global _start
_start:
	mov     $42, %bl 
	xor     %eax, %eax
	inc     %eax
	int     $0x80
