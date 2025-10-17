# filter_ne(a_base, len, x, b_base) -> a0 = lenB
# Caller pushes: b_base, x, len, a_base (in that exact order)

	.text

# Frame: 64 bytes
# Args at: b_base=64(s0), x=68(s0), len=72(s0), a_base=76(s0)
# Local k at 0(s0)
filter_ne:
	addi	sp, sp, -64
	sw	ra, 60(sp)
	sw	s0, 56(sp)
	addi	s0, sp, 0

	lw	t0, 76(s0)	# a_base
	lw	t1, 72(s0)	# len
	lw	t2, 68(s0)	# x
	lw	t3, 64(s0)	# b_base

	li	t4, 0
	sw	t4, 0(s0)	# k = 0

	li	t5, 0		# i = 0

.fn_loop:
	bge	t5, t1, .fn_done

	slli	a3, t5, 2	# offset in bytes
	add	a4, t0, a3	# &A[i]
	lw	a5, 0(a4)	# A[i]
	beq	a5, t2, .fn_skip

	# B[k] = A[i]
	lw	t4, 0(s0)	# k
	slli	a3, t4, 2
	add	a3, t3, a3
	sw	a5, 0(a3)
	addi	t4, t4, 1
	sw	t4, 0(s0)	# k++

.fn_skip:
	addi	t5, t5, 1
	j	.fn_loop

.fn_done:
	lw	a0, 0(s0)	# return k

	lw	ra, 60(sp)
	lw	s0, 56(sp)
	addi	sp, sp, 64
	jr	ra
