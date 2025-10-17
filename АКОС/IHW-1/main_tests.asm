# main_tests.asm — autotest runner (9 points): uses macros for printing and calls

	.include "io.asm"
	.include "macros.asm"
	.include "alg.asm"

	.data
# Messages for report
msg_case:	.asciz "CASE "
msg_colon:	.asciz ": "
msg_pass:	.asciz "PASS\n"
msg_fail:	.asciz "FAIL\n"
lbl_sum:	.asciz "PASSED "
lbl_of:		.asciz " / "

# Result buffer
	ALLOC_I32_ARRAY10(B)

# ---- Test cases ----
	.align 2
A1:	.word 5
X1:	.word 7
E1:	.word 5
L1:	.word 1

	.align 2
A2:	.word 7
X2:	.word 7
E2:	.word 0
L2:	.word 0

	.align 2
A3:	.word 1,2,3,4,5,6,7,8,9,10
X3:	.word 99
E3:	.word 1,2,3,4,5,6,7,8,9,10
L3:	.word 10

	.align 2
A4:	.word 3,3,3,3,3
X4:	.word 3
E4:	.word 0
L4:	.word 0

	.align 2
A5:	.word 1,2,3,2,4,2
X5:	.word 2
E5:	.word 1,3,4
L5:	.word 3

	.align 2
A6:	.word -1,0,-1,5
X6:	.word -1
E6:	.word 0,5
L6:	.word 2

	.text
	.globl main
	j main

# memeq(base1, base2, len_words) -> a0: 1/0
# Caller pushes: len, base2, base1
memeq:
	addi sp, sp, -64
	sw ra, 60(sp)
	sw s0, 56(sp)
	addi s0, sp, 0

	lw t0, 72(s0)	# base1
	lw t1, 68(s0)	# base2
	lw t2, 64(s0)	# len
	li t3, 0

.me_loop:
	bge t3, t2, .me_ok
	slli t4, t3, 2
	add t5, t0, t4
	add t6, t1, t4
	lw a0, 0(t5)
	lw a1, 0(t6)
	bne a0, a1, .me_bad
	addi t3, t3, 1
	j .me_loop

.me_ok:
	li a0, 1
	j .me_done
.me_bad:
	li a0, 0
.me_done:
	lw ra, 60(sp)
	lw s0, 56(sp)
	addi sp, sp, 64
	jr ra

# print_case_result: a0=1/0 (PASS/FAIL), a1=case_id
print_case_result:
	addi sp, sp, -16
	sw ra, 12(sp)
	sw a0, 8(sp)
	sw a1, 4(sp)

	PRINT_STR(msg_case)
	lw a0, 4(sp)	# case number
	li a7, 1
	ecall
	PRINT_STR(msg_colon)

	lw t0, 8(sp)
	beqz t0, .fail
	PRINT_STR(msg_pass)
	j .out
.fail:
	PRINT_STR(msg_fail)
.out:
	lw ra, 12(sp)
	addi sp, sp, 16
	jr ra

# ---------------- MAIN ----------------
main:
	INIT_STACK()

	li s0, 0	# passed
	li s1, 6	# total

# ----- CASE 1 -----
	la t0, A1
	li t1, 1
	lw t2, X1
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)	# a0 = lenB
	mv t4, a0
	lw t5, L1
	bne t4, t5, FAIL1
	la a0, B
	la a1, E1
	mv a2, t4
	CALL3(memeq, a0, a1, a2)
	beqz a0, FAIL1
PASS1:
	addi s0, s0, 1
	li a0, 1
	li a1, 1
	jal print_case_result
	j CASE2
FAIL1:
	li a0, 0
	li a1, 1
	jal print_case_result

# ----- CASE 2 -----
CASE2:
	la t0, A2
	li t1, 1
	lw t2, X2
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)
	mv t4, a0
	lw t5, L2
	bne t4, t5, FAIL2
PASS2:
	addi s0, s0, 1
	li a0, 1
	li a1, 2
	jal print_case_result
	j CASE3
FAIL2:
	li a0, 0
	li a1, 2
	jal print_case_result

# ----- CASE 3 -----
CASE3:
	la t0, A3
	li t1, 10
	lw t2, X3
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)
	mv t4, a0
	lw t5, L3
	bne t4, t5, FAIL3
	la a0, B
	la a1, E3
	mv a2, t4
	CALL3(memeq, a0, a1, a2)
	beqz a0, FAIL3
PASS3:
	addi s0, s0, 1
	li a0, 1
	li a1, 3
	jal print_case_result
	j CASE4
FAIL3:
	li a0, 0
	li a1, 3
	jal print_case_result

# ----- CASE 4 -----
CASE4:
	la t0, A4
	li t1, 5
	lw t2, X4
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)
	mv t4, a0
	lw t5, L4
	bne t4, t5, FAIL4
PASS4:
	addi s0, s0, 1
	li a0, 1
	li a1, 4
	jal print_case_result
	j CASE5
FAIL4:
	li a0, 0
	li a1, 4
	jal print_case_result

# ----- CASE 5 -----
CASE5:
	la t0, A5
	li t1, 6
	lw t2, X5
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)
	mv t4, a0
	lw t5, L5
	bne t4, t5, FAIL5
	la a0, B
	la a1, E5
	mv a2, t4
	CALL3(memeq, a0, a1, a2)
	beqz a0, FAIL5
PASS5:
	addi s0, s0, 1
	li a0, 1
	li a1, 5
	jal print_case_result
	j CASE6
FAIL5:
	li a0, 0
	li a1, 5
	jal print_case_result

# ----- CASE 6 -----
CASE6:
	la t0, A6
	li t1, 4
	lw t2, X6
	la t3, B
	CALL4(filter_ne, t0, t1, t2, t3)
	mv t4, a0
	lw t5, L6
	bne t4, t5, FAIL6
	la a0, B
	la a1, E6
	mv a2, t4
	CALL3(memeq, a0, a1, a2)
	beqz a0, FAIL6
PASS6:
	addi s0, s0, 1
	li a0, 1
	li a1, 6
	jal print_case_result
	j SUMMARY
FAIL6:
	li a0, 0
	li a1, 6
	jal print_case_result

SUMMARY:
	PRINT_STR(lbl_sum)
	PRINT_INT(s0)
	PRINT_STR(lbl_of)
	PRINT_INT(s1)
	PRINT_NL()

	li a7, 10
	ecall
