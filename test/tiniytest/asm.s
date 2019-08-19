

global get_val;

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