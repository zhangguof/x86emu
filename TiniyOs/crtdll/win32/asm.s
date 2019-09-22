global call_win_host_func;

global HOST_CALL_PTR32


%macro  _DEF_HOST_FUNC 2
GLOBAL %1;
GLOBAL _%1
export %1
_%1:
%1:
	push ebp
	mov ebp, esp
	mov eax,%2
	jmp [HOST_CALL_PTR32]
	leave
	ret
%endmacro

%define DEF_HOST_FUNC(func,idx) _DEF_HOST_FUNC func,idx

%macro  _DEF_HOST_STD_FUNC 3
GLOBAL %1;
GLOBAL _%1
export %1
_%1:
%1:
	push ebp
	mov ebp, esp
	mov eax,%2
	jmp [HOST_CALL_PTR32]
	leave
	ret %3
%endmacro

%define DEF_HOST_STD_FUNC(func,idx,args) _DEF_HOST_STD_FUNC func,idx,args





section .text
global _call_host_ret;
global call_host_ret
export call_host_ret;

; DEF_HOST_FUNC puts,1
; DEF_HOST_FUNC(puts,1)
%include "cpu/host_call.hpp"
%include "gen_code/wrap_gen_code.h"
%include "winapi/wrap_winapi.h"




;;void call_host_func(uint32_t func_id, ...)
;;in gcc!! f (a, b, c, d, e, f);
;;			  a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9
;;in win x64 a->rcx,b->rdx,c->r8,d->r9

call_win_host_func:
	push ebp
	mov ebp, esp
	; get_GOT ;rbx = rip?
	mov ebx, HOST_CALL_PTR32
	; mov rax, [rbx+rdi]
	; push [rbx]
	jmp [ebx];
	leave
	ret

;;;host-> guest_fun ->ret: call_host_ret

;;return to host code:
_call_host_ret:
call_host_ret:
	;mov ebx, HOST_CALL_PTR32;
	; mov rdi, eax; //ret val
	; push eax; //arg0 ,ret code
	; mov ecx, eax
	push edx ;;uint64_t hight bits
	push eax
	mov eax, 0x0;
	;jmp [ebx];
	jmp [HOST_CALL_PTR32]
	; leave
	; ret

extern _gdt_ptr
global _load_gdt
global load_gdt
_load_gdt:
load_gdt:
	lgdt[_gdt_ptr]
	ret


section .data
_HOST_CALL_PTR32:
HOST_CALL_PTR32: dd 0x1FFFFFFD

global _HOST_CALL_PTR32
export HOST_CALL_PTR32

section .drectve
; export_dll 'call_host_ret'
; db '-export:"call_host_ret"',0x20
; db 0x00




