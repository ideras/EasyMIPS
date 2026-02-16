#set $t0 = 10
#set $t1 = 10
add $t2, $t0, $t1
loop: beq $t0, $zero, end_loop
addi $t0, $t0, -1
j loop
end_loop:
#stop