li $t0, 10
li $t1, 20
loop1: add $t0, $t0, $t1
    j loop1

loop2:
    addi $a0, $t0, 100
    beq $a0, $zero, loop2