	.arch armv8-a
	.file	"decomment.c"
	.text
	.global	comment_start_line
	.bss
	.align	2
	.type	comment_start_line, %object
	.size	comment_start_line, 4
comment_start_line:
	.zero	4
	.global	line
	.data
	.align	2
	.type	line, %object
	.size	line, 4
line:
	.word	1
	.text
	.align	2
	.global	handleNormalState
	.type	handleNormalState, %function
handleNormalState:
.LFB0:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 47
	bne	.L2
	mov	w0, 1
	b	.L3
.L2:
	ldr	w0, [sp, 28]
	cmp	w0, 34
	bne	.L4
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 4
	b	.L3
.L4:
	ldr	w0, [sp, 28]
	cmp	w0, 39
	bne	.L5
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 6
	b	.L3
.L5:
	ldr	w0, [sp, 28]
	cmp	w0, 10
	bne	.L6
	mov	w0, 10
	bl	putchar
	mov	w0, 0
	b	.L3
.L6:
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 0
.L3:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE0:
	.size	handleNormalState, .-handleNormalState
	.align	2
	.global	handleSlashState
	.type	handleSlashState, %function
handleSlashState:
.LFB1:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 42
	bne	.L8
	adrp	x0, line
	add	x0, x0, :lo12:line
	ldr	w1, [x0]
	adrp	x0, comment_start_line
	add	x0, x0, :lo12:comment_start_line
	str	w1, [x0]
	mov	w0, 32
	bl	putchar
	mov	w0, 2
	b	.L9
.L8:
	ldr	w0, [sp, 28]
	cmp	w0, 47
	bne	.L10
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 1
	b	.L9
.L10:
	ldr	w0, [sp, 28]
	cmp	w0, 34
	bne	.L11
	mov	w0, 47
	bl	putchar
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 4
	b	.L9
.L11:
	ldr	w0, [sp, 28]
	cmp	w0, 39
	bne	.L12
	mov	w0, 47
	bl	putchar
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 6
	b	.L9
.L12:
	mov	w0, 47
	bl	putchar
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 0
.L9:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE1:
	.size	handleSlashState, .-handleSlashState
	.align	2
	.global	handleInCommentState
	.type	handleInCommentState, %function
handleInCommentState:
.LFB2:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 42
	bne	.L14
	mov	w0, 3
	b	.L15
.L14:
	ldr	w0, [sp, 28]
	cmp	w0, 10
	bne	.L16
	mov	w0, 10
	bl	putchar
.L16:
	mov	w0, 2
.L15:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE2:
	.size	handleInCommentState, .-handleInCommentState
	.align	2
	.global	handleStarState
	.type	handleStarState, %function
handleStarState:
.LFB3:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 42
	bne	.L18
	mov	w0, 3
	b	.L19
.L18:
	ldr	w0, [sp, 28]
	cmp	w0, 47
	bne	.L20
	mov	w0, 0
	b	.L19
.L20:
	ldr	w0, [sp, 28]
	cmp	w0, 10
	bne	.L21
	mov	w0, 10
	bl	putchar
	mov	w0, 2
	b	.L19
.L21:
	mov	w0, 2
.L19:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE3:
	.size	handleStarState, .-handleStarState
	.align	2
	.global	handleInStringState
	.type	handleInStringState, %function
handleInStringState:
.LFB4:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 92
	bne	.L23
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 5
	b	.L24
.L23:
	ldr	w0, [sp, 28]
	cmp	w0, 34
	bne	.L25
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 0
	b	.L24
.L25:
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 4
.L24:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE4:
	.size	handleInStringState, .-handleInStringState
	.align	2
	.global	handleEscapedInStringState
	.type	handleEscapedInStringState, %function
handleEscapedInStringState:
.LFB5:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 4
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE5:
	.size	handleEscapedInStringState, .-handleEscapedInStringState
	.align	2
	.global	handleInCharState
	.type	handleInCharState, %function
handleInCharState:
.LFB6:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	cmp	w0, 92
	bne	.L29
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 7
	b	.L30
.L29:
	ldr	w0, [sp, 28]
	cmp	w0, 39
	bne	.L31
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 0
	b	.L30
.L31:
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 6
.L30:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE6:
	.size	handleInCharState, .-handleInCharState
	.align	2
	.global	handleEscapedInCharState
	.type	handleEscapedInCharState, %function
handleEscapedInCharState:
.LFB7:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	w0, [sp, 28]
	ldr	w0, [sp, 28]
	bl	putchar
	mov	w0, 6
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE7:
	.size	handleEscapedInCharState, .-handleEscapedInCharState
	.section	.rodata
	.align	3
.LC0:
	.string	"Error: line %d: unterminated comment\n"
	.text
	.align	2
	.global	main
	.type	main, %function
main:
.LFB8:
	.cfi_startproc
	stp	x29, x30, [sp, -32]!
	.cfi_def_cfa_offset 32
	.cfi_offset 29, -32
	.cfi_offset 30, -24
	mov	x29, sp
	str	wzr, [sp, 28]
	b	.L35
.L45:
	ldr	w0, [sp, 28]
	cmp	w0, 7
	beq	.L36
	ldr	w0, [sp, 28]
	cmp	w0, 7
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 6
	beq	.L38
	ldr	w0, [sp, 28]
	cmp	w0, 6
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 5
	beq	.L39
	ldr	w0, [sp, 28]
	cmp	w0, 5
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 4
	beq	.L40
	ldr	w0, [sp, 28]
	cmp	w0, 4
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 3
	beq	.L41
	ldr	w0, [sp, 28]
	cmp	w0, 3
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 2
	beq	.L42
	ldr	w0, [sp, 28]
	cmp	w0, 2
	bhi	.L37
	ldr	w0, [sp, 28]
	cmp	w0, 0
	beq	.L43
	ldr	w0, [sp, 28]
	cmp	w0, 1
	beq	.L44
	b	.L37
.L43:
	ldr	w0, [sp, 24]
	bl	handleNormalState
	str	w0, [sp, 28]
	b	.L37
.L44:
	ldr	w0, [sp, 24]
	bl	handleSlashState
	str	w0, [sp, 28]
	b	.L37
.L42:
	ldr	w0, [sp, 24]
	bl	handleInCommentState
	str	w0, [sp, 28]
	b	.L37
.L41:
	ldr	w0, [sp, 24]
	bl	handleStarState
	str	w0, [sp, 28]
	b	.L37
.L40:
	ldr	w0, [sp, 24]
	bl	handleInStringState
	str	w0, [sp, 28]
	b	.L37
.L39:
	ldr	w0, [sp, 24]
	bl	handleEscapedInStringState
	str	w0, [sp, 28]
	b	.L37
.L38:
	ldr	w0, [sp, 24]
	bl	handleInCharState
	str	w0, [sp, 28]
	b	.L37
.L36:
	ldr	w0, [sp, 24]
	bl	handleEscapedInCharState
	str	w0, [sp, 28]
	nop
.L37:
	ldr	w0, [sp, 24]
	cmp	w0, 10
	bne	.L35
	adrp	x0, line
	add	x0, x0, :lo12:line
	ldr	w0, [x0]
	add	w1, w0, 1
	adrp	x0, line
	add	x0, x0, :lo12:line
	str	w1, [x0]
.L35:
	bl	getchar
	str	w0, [sp, 24]
	ldr	w0, [sp, 24]
	cmn	w0, #1
	bne	.L45
	ldr	w0, [sp, 28]
	cmp	w0, 1
	bne	.L46
	mov	w0, 47
	bl	putchar
.L46:
	ldr	w0, [sp, 28]
	cmp	w0, 2
	beq	.L47
	ldr	w0, [sp, 28]
	cmp	w0, 3
	bne	.L48
.L47:
	adrp	x0, stderr
	add	x0, x0, :lo12:stderr
	ldr	x3, [x0]
	adrp	x0, comment_start_line
	add	x0, x0, :lo12:comment_start_line
	ldr	w0, [x0]
	mov	w2, w0
	adrp	x0, .LC0
	add	x1, x0, :lo12:.LC0
	mov	x0, x3
	bl	fprintf
	mov	w0, 1
	b	.L49
.L48:
	mov	w0, 0
.L49:
	ldp	x29, x30, [sp], 32
	.cfi_restore 30
	.cfi_restore 29
	.cfi_def_cfa_offset 0
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.ident	"GCC: (GNU) 11.4.1 20231218 (Red Hat 11.4.1-3)"
	.section	.note.GNU-stack,"",@progbits
