	.eqv N_MIN, 1
	.eqv N_MAX, 10

	.include "io.asm"		# strings and reusable I/O subroutines
	.include "macros.asm"	# macro library (used throughout this file)
	.include "alg.asm"		# filtering algorithm

	.data
	ALLOC_I32_ARRAY10(A)
	ALLOC_I32_ARRAY10(B)

	.text
	.globl main

main:
	# Deterministic stack init (safety)
	INIT_STACK()

	# ---- Read N in [1..10]
	la	t0, msg_enter_n
	li	t1, N_MIN
	li	t2, N_MAX
	CALL3(read_int_range, t0, t1, t2)	# a0 <- N
	mv	s1, a0

	# ---- Read X
	la	t0, msg_enter_x
	CALL1(read_int, t0)			# a0 <- X
	mv	s2, a0

	# ---- Read A[N]
	la	t0, A
	mv	t1, s1
	CALL2(read_array, t0, t1)

	# ---- B = filter_ne(A, N, X, B)
	la	t0, A
	mv	t1, s1
	mv	t2, s2
	la	t3, B
	CALL4(filter_ne, t0, t1, t2, t3)	# a0 <- lenB
	mv	s3, a0

	# ---- Print A
	la	t0, lbl_A
	la	t1, A
	mv	t2, s1
	CALL3(print_array, t0, t1, t2)

	# ---- Print B
	la	t0, lbl_B
	la	t1, B
	mv	t2, s3
	CALL3(print_array, t0, t1, t2)

	# ---- Exit
	li	a7, 10
	ecall
