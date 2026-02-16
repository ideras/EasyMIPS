.global main

; a     -> $a0
; size  -> $a1
; sum   -> $v0
; i -> $t0

arraySum:
    move $v0, $zero
    move $t0, $zero
.for:
    slt $t1, $t0, $a1
    beq	$t1, $zero, .end_for
    sll $t1, $t0, 2
    add $t1, $a0, $t1
    lw $t1, 0($t1)
    add $v0, $v0, $t1

    addi $t0, $t0, 1
    j .for 

.end_for:
    jr $ra
    
main:
    addi $sp, $sp, -16
    sw $ra, 12($sp)

    li $t0, 10
    sw $t0, 0($sp)
    li $t0, 20
    sw $t0, 4($sp)
    li $t0, 30
    sw $t0, 8($sp)

    move $a0, $sp
    li $a1, 3
    jal arraySum

    move $a0, $v0
    jal print_int

    li $a0, 10
    jal print_char

    lw $ra, 12($sp)
    addi $sp, $sp, 16

    jr $ra