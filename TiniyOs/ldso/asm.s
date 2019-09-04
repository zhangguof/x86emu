extern  _GLOBAL_OFFSET_TABLE_   ; in ELF

global call_host_func:function;
global call_host_ret:function;
; global get_val;
global HOST_CALL_PTR


%macro  get_GOT 0 
		push rbx
        call    %%getgot 
  %%getgot: 
        pop     rbx 
        add     rbx,_GLOBAL_OFFSET_TABLE_+$$-%%getgot wrt ..gotpc 

%endmacro

%macro leave_GOT 0
	mov rbx, [rbp-8]
%endmacro

%macro  _DEF_HOST_FUNC 2
GLOBAL %1:function;
%1:
	push rbp
	mov rbp, rsp

	get_GOT ;rbx = rip?
	mov rax,%2
	mov r10, HOST_CALL_PTR wrt ..gotoff
	jmp [rbx+r10];
	
	leave_GOT

	leave
	ret
%endmacro

%define DEF_HOST_FUNC(func,idx) _DEF_HOST_FUNC func,idx

%include "cpu/host_call.hpp"


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
	leave_GOT
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
	mov rdi, rax; //ret val
	mov rax, 0x0;
	jmp [rbx+r10];
	
	leave_GOT
	leave
	ret

