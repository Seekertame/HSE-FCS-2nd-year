.include "macros.asm"

.data
nl:        .asciz "\n"
ask_x:     .asciz "Enter x (double): "
res_hdr:   .asciz "sinh(x) = "
warn_rng:  .asciz "Note: |x| > 20 may slow convergence or overflow.\n"
eps_info:  .asciz "Using eps_rel = 1e-3, max_iter = 200\n"
.align 3
eps_rel:   .double 1.0e-3    # relative tolerance

.text
.globl main
main:
	# Note and prompt
	PRINT_STR warn_rng
	PRINT_STR ask_x

	# Read x -> fs0
	READ_DBL fs0

	# Info line
	PRINT_STR eps_info

	# Load eps_rel -> fs1 ; max_iter -> t1
	la     t0, eps_rel
	fld    fs1, 0(t0)
	li     t1, 200

	# Call: sinh_series(fs0, fs1, t1) ; result in fa0
	CALL_SINH fs0, fs1, t1

	# Print result
	PRINT_STR res_hdr
	PRINT_DBL fa0
	PRINT_NL

	# Exit
	li a7, 10
	ecall
