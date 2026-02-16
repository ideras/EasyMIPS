#set $t1 = 20
#set $t2 = 10
add $t0, $t1, $t2
#show $t0
addu $t0, $t1, $t2
#show $t0
sub $t0, $t1, $t2
#show $t0
subu $t0, $t1, $t2
#show $t0

#set $t0 = 0xfeaa
#set $t1 = 0x70000000
#set $t2 = 0x70000000
addu $t0, $t1, $t2
#show $t0

#set $t0 = 0x0
#set $t1 = 0x80000000
addi $t0, $t1, 0x00000fff
#show $t0

#set $t1 = 0x80000000
addiu $t0, $t1, 0x00000fff
#show $t0

#set $t0 = 0xcacafeaa
#set $t1 = 0x80000000
addiu $t0, $t1, 0x0000ffff
#show $t0

#set $t0 = 0xdeadbeef
#set $t1 = 0x80000000
#set $t2 = 1
subu $t0, $t1, $t2
#show $t0

#set $t0 = 45
#set $t1 = 0xffffffff
mult $t0, $t1
#show $lo hex
#show $hi hex

#set $t0 = 45
#set $t1 = 0xffffffff
multu $t0, $t1
#show $hi hex
#show $lo hex

#set $t0 = 0xffffffd3
#set $t1 = 7
div $t0, $t1
#show $lo hex
#show $hi hex

#set $v0 = 0xffffffd3
#set $v1 = 7
divu $v0, $v1
#show $lo
#show $hi

; mfhi
#set $v0 = 0xaaaabbcc
#set $hi = 0xdeadbeef
mfhi $v0
#show $v0 hex

; mflo
#set $v0 = 0xaaaabbcc
#set $lo = 0xdeadbeef
mflo $v0
#show $v0 hex

; mthi
#set $v0 = 0xdeadbeef
#set $hi = 0xaaaabbcc
mthi $v0 
#show $hi hex

; mtlo
#set $v0 = 0xdeadbeef
#set $lo = 0xaaaabbcc
mtlo $v0
#show $lo hex