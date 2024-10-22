global call_win_host_func;
global HOST_CALL_PTR


%macro  _DEF_HOST_FUNC 2
GLOBAL %1;
%1:
	push rbp
	mov rbp, rsp
	mov rax,%2
	jmp [HOST_CALL_PTR]
	leave
	ret
%endmacro

%define DEF_HOST_FUNC(func,idx) _DEF_HOST_FUNC func,idx




section .text

; DEF_HOST_FUNC puts,1
; DEF_HOST_FUNC(puts,1)
%include "cpu/host_call.hpp"
%include "gen_code/wrap_gen_code.h"




;;void call_host_func(uint32_t func_id, ...)
;;in gcc!! f (a, b, c, d, e, f);
;;			  a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9
;;in win x64 a->rcx,b->rdx,c->r8,d->r9

call_win_host_func:
	push rbp
	mov rbp, rsp
	; get_GOT ;rbx = rip?
	mov rbx, HOST_CALL_PTR
	; mov rax, [rbx+rdi]
	; push [rbx]
	jmp [rbx];
	leave
	ret


section .data
HOST_CALL_PTR: dq 0x1FFFFFFE





