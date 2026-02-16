.global main

.data
str1: .byte "iseven(45) ", 0
str2: .byte "iseven(44) ", 0

.text

iseven:
    andi $t0, $a0, 1
    bne $t0, $zero, .false
    li $v0, 1
    jr $ra
.false:
    li $v0, 0
    jr $ra

main:
    addi $sp, $sp, -4
    sw $ra, 0($sp)

    la $a0, str1
    jal print_str

    li $a0, 45
    jal iseven

    move $a0, $v0
    jal print_int

    li $a0, ' '
    jal print_char

    la $a0, str2
    jal print_str

    li $a0, 44
    jal iseven

    move $a0, $v0
    jal print_int

    li $a0, 10
    jal print_char

    li $v0, 0

    lw $ra, 0($sp)
    addi $sp, $sp, 4
    
    jr $ra
