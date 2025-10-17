# Provides:
#	- PRINT_STR(lbl), PRINT_INT(reg), PRINT_CH(imm), PRINT_NL()
#	- READ_INT()  (result goes to a0)
#	- CALL1/2/3/4(fn, arg0, arg1, ...) — pushes args (in reverse order), calls, then cleans up
#	- INIT_STACK() — deterministic stack pointer initialization
#	- ALLOC_I32_ARRAY10(name) / ALLOC_I32_ARRAY_BYTES(name, bytes) — aligned buffers for .word data (use inside .data)

# --------- Printing / input ----------
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

# Requires that label 'nl' is defined in io.asm
.macro PRINT_NL()
	la	a0, nl
	li	a7, 4
	ecall
.end_macro

.macro READ_INT()
	li	a7, 5
	ecall			# a0 <- int
.end_macro

# --------- Calls with stack-passed arguments ----------
# Macro argument order is natural (arg0, arg1, ...),
# while the macro pushes them in REVERSED order (as our callees expect).
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

# --------- Stack initialization (so we don't depend on RARS settings) ----------
.macro INIT_STACK()
	li	sp, 0x7ffffffc
.end_macro

# --------- Aligned int32 buffers ----------

# Option A: exactly 10 int32 elements (suited for this assignment)
.macro ALLOC_I32_ARRAY10(%name)
	.align	2
%name:
	.space	40			# 10 * 4 bytes
.end_macro

# Option B: arbitrary size in BYTES (pass a literal number of bytes!)
.macro ALLOC_I32_ARRAY_BYTES(%name, %bytes_lit)
	.align	2
%name:
	.space	%bytes_lit		# pass a LITERAL (e.g., 40), not an expression
.end_macro
