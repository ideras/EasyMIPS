addi $t0, $zero, 0
addi $t1, $zero, 10

start_loop:
beq $t0, $t1, end_loop
; #show $t0 signed decimal

; Imprimir $t0
addi $v0, $zero, 1
add $a0, $zero, $t0
syscall

; Mostrar nueva linea
addi $v0, $zero, 11
addi $a0, $zero, 10
syscall

addi $t0, $t0, 1
j start_loop

end_loop:

#show start_loop hex
#show end_loop hex
