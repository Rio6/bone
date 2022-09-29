	.intel_syntax noprefix
	.text
	.globl	_start
_start:
	push	rbp
	mov	rbp, rsp

	mov	rcx, 10
loop:
	push	rcx

	mov	rax, 1
	mov 	rdi, 1
	mov 	rsi, offset message
	mov 	rdx, 6
	syscall

	pop	rcx
	sub	rcx, 1
	jnz	loop

	mov	rax, 60
	mov	rdi, 0
	syscall

	.data
message:
	.asciz "hello\n"
