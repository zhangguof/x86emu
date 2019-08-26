extern  _GLOBAL_OFFSET_TABLE_   ; in ELF

%macro  get_GOT 0 
		push rbx
        call    %%getgot 
  %%getgot: 
        pop     rbx 
        add     rbx,_GLOBAL_OFFSET_TABLE_+$$-%%getgot wrt ..gotpc 

%endmacro


global call_host_func:function;
global call_host_ret:function;
; global get_val;
global HOST_CALL_PTR

; HOST_CALL_PTR: dq 0x000000001FFFFFFF
HOST_CALL_PTR: dq 0x1FFFFFFF


;;void call_host_func(uint32_t func_id, ...)
;;in gcc!! f (a, b, c, d, e, f);
;;			  a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9
call_host_func:
	push rbp
	mov rbp, rsp
	get_GOT ;rbx = rip?
	mov r10, HOST_CALL_PTR wrt ..gotoff
	; mov rax, [rbx+rdi]
	; push 0x1FFFFFFF
	jmp [rbx+r10];
	leave
	ret
;;return to host code:
call_host_ret:
	push rbp
	mov rbp, rsp
	get_GOT ;rbx = rip?
	mov r10, HOST_CALL_PTR wrt ..gotoff
	; mov rax, [rbx+rdi]
	; push 0x1FFFFFFF
	mov rdi, 0x0;
	mov rsi, rax; //ret val
	jmp [rbx+r10];
	leave
	ret





; val1: dd 0x10000;

; get_val:
; push rbp;
; mov rbp, rsp
; ; mov eax, edi
; ; add eax, esi
; mov eax, [val1] ;8B042570014000
; ; mov rsp, rbp
; ; pop rbp
; leave
; ret