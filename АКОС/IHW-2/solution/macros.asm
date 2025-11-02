# ------------------ Basic I/O -------------------------
.macro PRINT_STR(%label)
	la	a0, %label
	li	a7, 4
	ecall
.end_macro


.macro PRINT_INT(%reg)
	mv	a0, %reg
	li	a7, 1
	ecall
.end_macro


.macro PRINT_CH(%imm)
	li	a0, %imm
	li	a7, 11
	ecall
.end_macro


.macro PRINT_NL()
	la	a0, nl
	li	a7, 4
	ecall
.end_macro


.macro READ_INT()
	li	a7, 5
	ecall
.end_macro


.macro PRINT_DBL(%fsrc)
	fmv.d	fa0, %fsrc
	li	a7, 3
	ecall
.end_macro


.macro READ_DBL(%fdst)
	li	a7, 7
	ecall
	fmv.d	%fdst, fa0
.end_macro


.macro PRINT_DBL_WITH_LABEL(%label, %fsrc)
	PRINT_STR %label
	PRINT_DBL %fsrc
	PRINT_NL
.end_macro


.macro PRINT_INT_WITH_LABEL(%label, %reg)
	PRINT_STR %label
	PRINT_INT %reg
	PRINT_NL
.end_macro

# --------- Calls with stack-passed arguments ----------
# These helpers push arguments to the stack in REVERSE order, call the function, then clean up the stack. 

.macro CALL1(%fn, %a0_)
	addi	sp, sp, -4
	sw	%a0_, 0(sp)
	jal	%fn
	addi	sp, sp, 4
.end_macro

.macro CALL2(%fn, %a0_, %a1_)
	addi	sp, sp, -8
	sw	%a1_, 0(sp)
	sw	%a0_, 4(sp)
	jal	%fn
	addi	sp, sp, 8
.end_macro

.macro CALL3(%fn, %a0_, %a1_, %a2_)
	addi	sp, sp, -12
	sw	%a2_, 0(sp)
	sw	%a1_, 4(sp)
	sw	%a0_, 8(sp)
	jal	%fn
	addi	sp, sp, 12
.end_macro

.macro CALL4(%fn, %a0_, %a1_, %a2_, %a3_)
	addi	sp, sp, -16
	sw	%a3_, 0(sp)
	sw	%a2_, 4(sp)
	sw	%a1_, 8(sp)
	sw	%a0_, 12(sp)
	jal	%fn
	addi	sp, sp, 16
.end_macro


# ------------- Stack & aligned buffers ----------------

# Set a deterministic stack top.
.macro INIT_STACK()
	li	sp, 0x7ffffffc
.end_macro


# Reserve exactly 10 x .word (aligned). Use inside .data.
.macro ALLOC_I32_ARRAY10(%name)
	.align	2
%name:
	.space	40			# 10 * 4 bytes
.end_macro


# Reserve `bytes_literal` bytes (aligned). Use inside .data.
.macro ALLOC_I32_ARRAY_BYTES(%name, %bytes_lit)
	.align	2
%name:
	.space	%bytes_lit
.end_macro


# -------- Task-specific wrappers / helpers ------------

# Call: double sinh_series(double x, double eps_rel, int max_iter)
# IN : fa0<=fx, fa1<=feps, a0<=rmax
# OUT: fa0 = result
.macro CALL_SINH(%fx, %feps, %rmax)
	fmv.d	fa0, %fx
	fmv.d	fa1, %feps
	mv	a0, %rmax
	jal	ra, sinh_series
.end_macro


# Put 1.0 (double) into FP register `fdst` without using .data.
.macro MAKE_ONE(%fdst)
	li	t0, 1
	fcvt.d.w %fdst, t0
.end_macro


# Compute relative error:
#   fout = |fcalc - fref| / max(1.0, |fref|)
# Uses temporaries: ft0, ft1, ft2 and t0.
.macro REL_ERR(%fcalc, %fref, %fone, %fout)
	fsub.d		ft0, %fcalc, %fref
	fsgnjx.d	ft0, ft0, ft0		# |calc - ref|
	fsgnjx.d	ft1, %fref, %fref	# |ref|
	fle.d		t0, %fone, ft1		# (1.0 <= |ref|)?
	beqz		t0, rel_err_zero
	fmv.d		ft2, ft1		# max = |ref|
	j			rel_err_one
rel_err_zero:
	fmv.d		ft2, %fone		# max = 1.0
rel_err_one:
	fdiv.d		%fout, ft0, ft2
.end_macro


# Print "<status_lbl> OK" or "<status_lbl> FAIL" depending on `ok_reg` (nonzero = OK).
.macro PRINT_STATUS(%status_lbl, %ok_lbl, %fail_lbl, %ok_reg)
	PRINT_STR %status_lbl
	beqz	%ok_reg, print_status_zero
	PRINT_STR %ok_lbl
	j	print_status_one
print_status_zero:
	PRINT_STR %fail_lbl
print_status_one:
.end_macro
