.text
.globl sinh_series

# double sinh_series(double x, double eps_rel, int max_iter)
# IN : fa0 = x
#      fa1 = eps_rel
#      a0  = max_iter
# OUT: fa0 = approx sinh(x)

# Stack-frame:
#   [sp+0]   sum   (8B)
#   [sp+8]   term  (8B)
#   [sp+16]  x2    (8B)
#   [sp+24]  k     (4B)
#   [sp+32]  ra    (4B)
# Total frame: 64 bytes

sinh_series:
	addi	sp, sp, -64
	sw	ra, 32(sp)

	fsd	fa0, 0(sp)          # sum
	fsd	fa0, 8(sp)          # term
	fmul.d	ft0, fa0, fa0
	fsd	ft0, 16(sp)         # x2
	sw	zero, 24(sp)        # k = 0

# === Series loop (add next term of sinh power series) ===
L_series_loop:
	# ---- Build next term: term *= x2 / ((2k+2)(2k+3)) ----
	lw	t0, 24(sp)          # k
	slli	t1, t0, 1           # 2k
	addi	t2, t1, 2           # 2k+2
	addi	t3, t1, 3           # 2k+3

	fcvt.d.w ft1, t2          # (double)(2k+2)
	fcvt.d.w ft2, t3          # (double)(2k+3)
	fmul.d   ft1, ft1, ft2    # denom = (2k+2)(2k+3)

	fld	ft3, 8(sp)           # term
	fld	ft4, 16(sp)          # x2
	fmul.d	ft3, ft3, ft4       # term * x2
	fdiv.d	ft3, ft3, ft1       # next_term
	fsd	ft3, 8(sp)           # term = next_term

	# ---- Accumulate: sum += term ----
	fld	ft5, 0(sp)           # sum
	fadd.d	ft5, ft5, ft3
	fsd	ft5, 0(sp)           # sum

	# ---- Convergence check: |term| <= eps_rel * max(1, |sum|) ----
	fsgnjx.d ft6, ft3, ft3    # |term|
	fsgnjx.d ft7, ft5, ft5    # |sum|

	# Prepare max(1.0, |sum|) without global data
	li	t4, 1
	fcvt.d.w ft8, t4          # ft8 = 1.0
	fle.d	t5, ft8, ft7        # (1.0 <= |sum|) ?
	beqz	t5, L_max_use_one
	fmv.d	ft9, ft7            # max_val = |sum|
	j	L_max_ready
L_max_use_one:
	fmv.d	ft9, ft8            # max_val = 1.0
L_max_ready:

	# rhs = eps_rel * max_val
	fmul.d	ft9, ft9, fa1
	fle.d	t6, ft6, ft9        # |term| <= rhs ?
	bne	t6, zero, L_return

	# ---- Iterate: k++ ; stop if k >= max_iter ----
	addi	t0, t0, 1
	sw	t0, 24(sp)
	bge	t0, a0, L_return

	j	L_series_loop

L_return:
	fld	fa0, 0(sp)           # return sum
	lw	ra, 32(sp)
	addi	sp, sp, 64
	ret
