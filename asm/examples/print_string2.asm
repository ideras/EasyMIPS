.data
str: .byte "Hello World", 10, 0

.text
li $v0, 4
li $a0, str
syscall


