
global _add;

;c = add(a,b)
;rdi,rsi,rdx,rcx,r8,r9
_add:
push rbp;
mov rbp, rsp
mov eax, edi
add eax,esi
; mov rsp, rbp
; pop rbp
leave
ret