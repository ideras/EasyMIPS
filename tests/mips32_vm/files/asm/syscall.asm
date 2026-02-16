; Print integer
#set $v0 = 1
#set $a0 = 94786
syscall

; Print char
#set $v0 = 11
#set $a0 = 'H'
syscall

; Print string
#set byte (0x10000000) = ["Hello World", 0]
#set $v0 = 4
#set $a0 = 0x10000000
syscall

#stop