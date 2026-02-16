; slt
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 0
slt $v0, $v1, $t0
#show $v0

#set $v0 = 0xdade
#set $v1 = 0x7eadbeef
#set $t0 = 0
slt $v0, $v1, $t0
#show $v0

; sltu
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 0
sltu $v0, $v1, $t0
#show $v0

#set $v0 = 0xdade
#set $v1 = 15
#set $t0 = 40958
sltu $v0, $v1, $t0
#show $v0

; slti
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
slti $v0, $v1, 0x0
#show $v0

#set $v0 = 0xdade
#set $v1 = 0x7eadbeef
slti $v0, $v1, 0x0
#show $v0

; sltiu
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
sltiu $v0, $v1, 0x0
#show $v0

#set $v0 = 0xdade
#set $v1 = 0x7eadbeef
sltiu $v0, $v1, 0xbeef
#show $v0