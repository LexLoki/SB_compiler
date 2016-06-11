pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl $0, %r11d
movl %edi, %r10d
addl %r10d, %r11d
movl %r11d, -4(%rbp)
movl -4(%rbp), %r11d
cmpl $0, %r11d
jl 13
je 13
jg 13
movl %esi, %eax
leave
ret