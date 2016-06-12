.globl _bemboba

_bemboba:
pushq %rbp
movq %rsp, %rbp
subq $16, %rsp
movl %edi, %eax
jmp 0xf
leave
ret
/*
movl $0, %r11d
movl $1, %r10d
addl %r10d, %r11d
movl %r11d, -4(%rbp)
movl %edi, %r11d
movl $0, %r10d
addl %r10d, %r11d
movl %r11d, -8(%rbp)
movl -8(%rbp), %r11d
cmpl $0, %r11d
jl 32	//6
je 30	//6
jg 0	//6
movl -4(%rbp), %r11d 	//4
movl -8(%rbp), %r10d 	//4
imull %r10d, %r11d		//3
movl %r11d, -4(%rbp)	//4
movl -8(%rbp), %r11d	//4
movl $1, %r10d			//6
subl %r10d, %r11d		//3
movl %r11d, -8(%rbp)	//4
movl -8(%rbp), %r11d	//4
cmpl $0, %r11d			//4
jl 6					//2
je 0x4					//2
jg -24					//2
movl -4(%rbp), %eax
*/