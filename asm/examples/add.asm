jal main

li $v0, 10
syscall

add:
    add $v0, $a0, $a1
    jr $ra
    
main:
    addi $sp, $sp, -4
    sw $ra, 0($sp)
    
    li $a0, 10
    li $a1, 40
    jal add
    
    move $a0, $v0
    li $v0, 1
    syscall
    
    li $v0, 11
    li $a0, 10
    syscall
    
    ; pop $ra
    lw $ra, 0($sp)
    addi $sp, $sp, 4
    
    jr $ra
