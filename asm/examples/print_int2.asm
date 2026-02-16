.data
num: .word 478, 987, 630

; MIPS32 las direcciones son de 32 bits
.text
    li $s0, num
    move $t0, $zero
    li $t2, 3
    
loop_start:
    li $v0, 1
    lw $a0, 0($s0)
    syscall
    
    li $v0, 11
    li $a0, 32
    syscall
    
    addi $t0, $t0, 1
    addi $s0, $s0, 4
    bne $t0, $t2, loop_start
    
    li $v0, 11
    li $a0, 10
    syscall
