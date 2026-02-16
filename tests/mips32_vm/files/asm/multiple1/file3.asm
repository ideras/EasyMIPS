.global main

main:
    move $s0, $ra
    li $a0, 100
    jal add_fn
    move $a0, $v0
    jal print

    jr $s0