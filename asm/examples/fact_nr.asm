; n = 5
#set $a0 = 5

#set $v0 = 1

loop_start:
    beq $a0, $zero, loop_end
    multu $v0, $a0
    mflo $v0
    addi $a0, $a0, -1
    j loop_start

loop_end:
    #show $v0 decimal
