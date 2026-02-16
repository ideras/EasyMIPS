addi $t1, $zero, 5

loop:
slti $t0, $t1, -5
bne $t0, $zero, end
#show $t1 signed decimal
addi $t1, $t1, -1
j loop
end:
