# Reusable I/O subroutines for general use
	.data
# Prompts and labels for interactive mode
msg_enter_n:	.asciz "Enter N (1..10): "
msg_enter_x:	.asciz "Enter X (int): "
msg_bad_n:	.asciz "Invalid N. Try again.\n"
msg_enter_ai:	.asciz "Enter A["
msg_close_ai:	.asciz "] = "
lbl_A:		.asciz "A: "
lbl_B:		.asciz "B: "
spc:		.byte 32,0
nl:		.byte 10,0

	.text

# print_array(label_addr, base_addr, len)
# Caller pushes (bottom to top): len, base, label
# Frame: 64; Args: len=64(s0), base=68(s0), label=72(s0)
print_array:
	addi	sp, sp, -64
	sw	ra, 60(sp)
	sw	s0, 56(sp)
	addi	s0, sp, 0

	lw	t0, 72(s0)	# label
	lw	t1, 68(s0)	# base
	lw	t2, 64(s0)	# len

	li	t3, 0
	sw	t3, 0(s0)	# i = 0

	# print label
	mv	a0, t0
	li	a7, 4
	ecall

.pa_loop:
	lw	t3, 0(s0)
	bge	t3, t2, .pa_done
	slli	t4, t3, 2
	add	t5, t1, t4
	lw	a0, 0(t5)
	li	a7, 1
	ecall

	la	a0, spc
	li	a7, 4
	ecall

	addi	t3, t3, 1
	sw	t3, 0(s0)
	j	.pa_loop

.pa_done:
	la	a0, nl
	li	a7, 4
	ecall

	lw	ra, 60(sp)
	lw	s0, 56(sp)
	addi	sp, sp, 64
	jr	ra

# read_array(base_addr, len)
# Caller pushes: len, base
# Frame: 64; Args: len=64(s0), base=68(s0); local i at 0(s0)
read_array:
	addi	sp, sp, -64
	sw	ra, 60(sp)
	sw	s0, 56(sp)
	addi	s0, sp, 0

	lw	t0, 68(s0)	# base
	lw	t1, 64(s0)	# len
	li	t2, 0
	sw	t2, 0(s0)

.ra_loop:
	lw	t2, 0(s0)
	bge	t2, t1, .ra_done

	la	a0, msg_enter_ai
	li	a7, 4
	ecall

	mv	a0, t2
	li	a7, 1
	ecall

	la	a0, msg_close_ai
	li	a7, 4
	ecall

	li	a7, 5		# read int
	ecall

	slli	t3, t2, 2
	add	t4, t0, t3
	sw	a0, 0(t4)

	addi	t2, t2, 1
	sw	t2, 0(s0)
	j	.ra_loop

.ra_done:
	lw	ra, 60(sp)
	lw	s0, 56(sp)
	addi	sp, sp, 64
	jr	ra

# read_int(prompt_addr) -> a0
# Caller pushes: prompt
# Frame: 32; Arg: 32(s0)
read_int:
	addi	sp, sp, -32
	sw	ra, 28(sp)
	sw	s0, 24(sp)
	addi	s0, sp, 0

	lw	t0, 32(s0)	# prompt
	mv	a0, t0
	li	a7, 4
	ecall
	li	a7, 5		# read int
	ecall

	lw	ra, 28(sp)
	lw	s0, 24(sp)
	addi	sp, sp, 32
	jr	ra

# read_int_range(prompt_addr, min, max) -> a0
# Caller pushes: max, min, prompt
# Frame: 48; Args: max=48(s0), min=52(s0), prompt=56(s0); local tmp at 0(s0)
read_int_range:
	addi	sp, sp, -48
	sw	ra, 44(sp)
	sw	s0, 40(sp)
	addi	s0, sp, 0

.rr_again:
	lw	t0, 56(s0)	# prompt
	mv	a0, t0
	li	a7, 4
	ecall

	li	a7, 5
	ecall			# a0 = x
	sw	a0, 0(s0)

	lw	t1, 52(s0)	# min
	lw	t2, 48(s0)	# max
	lw	t3, 0(s0)	# x

	blt	t3, t1, .rr_bad
	bgt	t3, t2, .rr_bad

	mv	a0, t3
	j	.rr_done

.rr_bad:
	la	a0, msg_bad_n
	li	a7, 4
	ecall
	j	.rr_again

.rr_done:
	lw	ra, 44(sp)
	lw	s0, 40(sp)
	addi	sp, sp, 48
	jr	ra
