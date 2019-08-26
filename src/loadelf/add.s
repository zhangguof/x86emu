extern  _GLOBAL_OFFSET_TABLE_   ; in ELF

%macro  get_GOT 0 
		push rbx
        call    %%getgot 
  %%getgot: 
        pop     rbx 
        add     rbx,_GLOBAL_OFFSET_TABLE_+$$-%%getgot wrt ..gotpc 

%endmacro

; %macro  end_get_GOT 0 
; 		mov ebx,[ebp-4]	
; %endmacro


global _add:function;

;c = add(a,b)
;rdi,rsi,rdx,rcx,r8,r9
val1: dd 0x10000

_add:
	push rbp;
	mov rbp, rsp
	get_GOT  ;rbx = got?
	mov eax, edi
	add eax, esi
	;add eax, [val1]
	mov rdi, val1 wrt ..gotoff
	add rax, [rbx+rdi]

	; mov rsp, rbp
	; pop rbp
	leave
	ret
