#set memory byte 0x10000000 = ["Hello World", 10, 0]

li $v0, 4
li $a0, 0x10000000
syscall
