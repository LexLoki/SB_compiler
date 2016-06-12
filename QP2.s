.globl _bemboba

_bemboba:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl $0, %r11d
movl %edi, %r10d
addl %r10d, %r11d
movl %r11d, -4(%rbp)
movl -4(%rbp), %r11d
cmpl $0, %r11d
jl 0x32
je 0x32
jg 0x32
movl %esi, %eax
leave
ret