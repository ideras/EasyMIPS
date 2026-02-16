; EasyASM C runtime functions

.global print_str
.global print_int
.global print_char
.global read_int
.global memset

start:
    jal main
    
    ; Exit program
    li $v0, 10
    syscall

print_str:
    li $v0, 4
    syscall
    jr $ra

print_int:
    li $v0, 1
    syscall
    jr $ra

print_char:
    li $v0, 11
    syscall
    jr $ra
    
read_int:
    li $v0, 5
    syscall
    jr $ra
    
; void * memset ( void * ptr, int value, size_t num );
memset:
.memset_loop:
    beqz $a2, .memset_end
    sb $a1, 0($a0)
    addiu $a0, $a0, 1
    addiu $a2, $a2, -1
    j .memset_loop
.memset_end:
    jr $ra

    
