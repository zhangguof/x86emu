


global call_host_func;
global get_val;
global HOST_CALL_PTR

HOST_CALL_PTR: dq 0x000000001FFFFFFF

;;void call_host_func(uint32_t func_id, uint8_t* data, uint32_t len)
;;in gcc!! f (a, b, c, d, e, f);
;;			  a->%rdi, b->%rsi, c->%rdx, d->%rcx, e->%r8, f->%r9
call_host_func:
	push rbp
	mov rbp, rsp
	jmp [HOST_CALL_PTR];
	leave
	ret





val1: dd 0x10000;

get_val:
push rbp;
mov rbp, rsp
; mov eax, edi
; add eax, esi
mov eax, [val1] ;8B042570014000
; mov rsp, rbp
; pop rbp
leave
ret