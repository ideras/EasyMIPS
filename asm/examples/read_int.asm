#set memory byte 0x10000000 = ["Enter a number: ", 0]

; Imprimir el mensaje
li $v0, 4
li $a0, 0x10000000
syscall

; Leer un entero
li $v0, 5
syscall

; Imprimir el numero leido
move $a0, $v0
li $v0, 1
syscall

; Imprimir un salto de linea
li $a0, 10
li $v0, 11
syscall
