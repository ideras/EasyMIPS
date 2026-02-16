jal main

li $v0, 10
syscall

factorial:
    addi $sp, $sp, -8
    sw $ra, 0($sp)
    sw $s0, 4($sp)

    slti $t0, $a0, 2
    bne $t0, $zero, base_case

    move $s0, $a0
    addi $a0, $a0, -1
    jal factorial

    mult $s0, $v0
    mflo $v0
    j epilog

base_case:
    li $v0, 1

epilog:
    lw $ra, 0($sp)
    lw $s0, 4($sp)
    addi $sp, $sp, 8

    jr $ra


main:
    addi $sp, $sp, -4
    sw $ra, 0($sp)

    li $a0, 7
    jal factorial

    move $a0, $v0
    li $v0, 1
    syscall

    li $a0, 10
    li $v0, 11
    syscall

    lw $ra, 0($sp)
    addi $sp, $sp, 4

    jr $ra
