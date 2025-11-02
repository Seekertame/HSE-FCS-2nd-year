.include "macros.asm"

.data
# --- UI strings ---
nl:          .asciz "\n"
sep:         .asciz "-----\n"
test_lbl:    .asciz "Test #"
x_lbl:       .asciz "x="
calc_lbl:    .asciz "calc="
ref_lbl:     .asciz "ref="
rel_lbl:     .asciz "rel="	   # relative error
status_lbl:  .asciz "status: "
ok_str:      .asciz "OK\n"
fail_str:    .asciz "FAIL\n"
sum_lbl:     .asciz "Passed: "
slash_lbl:   .asciz " / "

# --- constants ---
.align 3
eps_rel:    .double 1.0e-3         # target 0.1%
.align 3
thr_rel:    .double 1.0e-3         # pass threshold

# --- test inputs and Python math.sinh references ---
.align 3
xs:
	.double 0.0, 0.001, -0.001, 0.5
	.double -0.5, 1.0, -1.0, 2.0
	.double -2.0, 5.0, -5.0, 10.0
	.double -10.0, 15.0, -15.0, 20.0
	.double -20.0

.align 3
ref_y:
	.double 0.0, 0.0010000001666666751, -0.0010000001666666751, 0.52109530549374738
	.double -0.52109530549374738, 1.1752011936438014, -1.1752011936438014, 3.626860407847019
	.double -3.626860407847019, 74.20321057778875, -74.20321057778875, 11013.232874703393
	.double -11013.232874703393, 1634508.6862359024, -1634508.6862359024, 242582597.70489514
	.double -242582597.70489514

n:          .word 17

.text
.globl main
main:
	la    s2, xs               # s2 = base xs
	la    s3, ref_y            # s3 = base ref
	la    t6, n
	lw    s4, 0(t6)            # s4 = n

	la    t0, eps_rel
	fld   fs1, 0(t0)           # fs1 = eps_rel
	la    t1, thr_rel
	fld   fs3, 0(t1)           # fs3 = threshold
	MAKE_ONE fs4               # fs4 = 1.0 for max(1,|ref|)

	li    s0, 0                # i = 0
	li    s1, 0                # passed = 0
	li    t2, 4000             # t2 = max_iter

TEST_LOOP:
	bge   s0, s4, SUMMARY

	# --- Load input element x[i] ---
	slli  t6, s0, 3
	add   t0, s2, t6           # &xs[i]
	fld   fs0, 0(t0)           # fs0 = x

	# --- Compute calc = sinh_series(x, eps_rel, max_iter) ---
	CALL_SINH fs0, fs1, t2     # -> fa0
	fmv.d fs2, fa0             # save calc

	# --- Load reference value ref_y[i] ---
	slli  t6, s0, 3
	add   t4, s3, t6           # &ref[i]
	fld   ft0, 0(t4)           # ft0 = ref
	fmv.d fs5, ft0

	# --- Compute relative error: |calc - ref| / max(1, |ref|) ---
	REL_ERR fs2, fs5, fs4, ft4

	# --- Pass/fail decision: rel <= threshold ? ---
	fle.d t3, ft4, fs3

	# === Pretty-print the current test case ===
	PRINT_STR sep
	PRINT_STR test_lbl
	addi  t5, s0, 1
	PRINT_INT t5
	PRINT_NL

	PRINT_STR x_lbl
	PRINT_DBL fs0
	PRINT_NL

	PRINT_STR calc_lbl
	PRINT_DBL fs2
	PRINT_NL

	PRINT_STR ref_lbl
	PRINT_DBL fs5
	PRINT_NL

	PRINT_STR rel_lbl
	PRINT_DBL ft4
	PRINT_NL

	PRINT_STATUS status_lbl, ok_str, fail_str, t3

	# --- Tally passed tests ---
	beqz  t3, NEXT_TEST
	addi  s1, s1, 1

NEXT_TEST:
	addi  s0, s0, 1
	j     TEST_LOOP

SUMMARY:
	PRINT_STR sep
	PRINT_STR sum_lbl
	PRINT_INT s1
	PRINT_STR slash_lbl
	PRINT_INT s4
	PRINT_NL

	li a7, 10
	ecall
