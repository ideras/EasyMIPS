; sw
#set $t0 = 0xcacafeaa
#set $s0 = 0x10000000
sw $t0, 4($s0)
#show word 4($s0) hex

#set $t0 = 0xdeadbeef
#set $s0 = 0x10000008
sw $t0, 0xfff8($s0)
#show word -8($s0) hex

#set $t0 = 0xbacafeaa
#set $s0 = 0x7fffeefc
sw $t0, 4($s0)
#show word 4($s0) hex

; sh
#set word (0x10000004) = 0xbacadead
#set $t0 = 0xaabbc0de
#set $s0 = 0x10000000
sh $t0, 6($s0)
#show word 4($s0) hex

#set word (0x7fffef00) = 0xbacadead
#set $t0 = 0xaabbbaca
#set $s0 = 0x7fffeefc
sh $t0, 4($s0)
#show word 4($s0) hex

; sb
#set word (0x10000004) = 0xbacadead
#set $t0 = 0x33
#set $s0 = 0x10000000
sb $t0, 6($s0)
#show word 4($s0) hex

#set $t0 = 0x11
sb $t0, 4($s0)
#show word 4($s0) hex

#set $t0 = 0x44
sb $t0, 7($s0)
#show word 4($s0) hex

#set $t0 = 0x22
sb $t0, 5($s0)
#show word 4($s0) hex

; lw
#set word (0x10000004) = 0xdec0de
#set $s0 = 0x10000000
lw $t0, 4($s0)
#show $t0 hex

; lh
#set word (0x10000004) = 0xaa2211bb
#set $s0 = 0x10000000
lh $t0, 4($s0)
#show $t0 hex

lh $t0, 6($s0)
#show $t0 hex

; lhu
#set $s0 = 0x10000000
lhu $t0, 4($s0)
#show $t0 hex

lhu $t0, 6($s0)
#show $t0 hex

; lb
#set $s0 = 0x10000000
lb $t0, 4($s0)
#show $t0 hex

lb $t0, 5($s0)
#show $t0 hex

lb $t0, 6($s0)
#show $t0 hex

lb $t0, 7($s0)
#show $t0 hex

; lbu
#set $s0 = 0x10000000

lbu $t0, 4($s0)
#show $t0 hex

lbu $t0, 5($s0)
#show $t0 hex

lbu $t0, 6($s0)
#show $t0 hex

lbu $t0, 7($s0)
#show $t0 hex

#stop