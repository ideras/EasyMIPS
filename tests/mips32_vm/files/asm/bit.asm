; and
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 0xffff

and $v0, $v1, $t0
#show $v0 hex

; nor
#set $v0 = 0xdade
#set $v1 = 0xdead9449
#set $t0 = 0x2aa6

nor $v0, $v1, $t0
#show $v0 hex

; or
#set $v0 = 0xdade
#set $v1 = 0xdead9449
#set $t0 = 0x2aa6

or $v0, $v1, $t0
#show $v0 hex

; xor
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 0x2aa6
xor $v0, $v1, $t0
#show $v0 hex

;  andi
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
andi $v0, $v1, 0xffff
#show $v0 hex

;  ori
#set $v0 = 0xdade
#set $v1 = 0xdead9449
ori $v0, $v1, 0x2aa6
#show $v0 hex

;  xori
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
xori $v0, $v1, 0x2aa6
#show $v0 hex

;  sll
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
sll $v0, $v1, 16
#show $v0 hex

;  srl
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
srl $v0, $v1, 16
#show $v0 hex

;  sra
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
sra $v0, $v1, 16
#show $v0 hex

;  sllv
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 16
sllv $v0, $v1, $t0
#show $v0 hex

;  srlv
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 16
srlv $v0, $v1, $t0
#show $v0 hex

;  srav
#set $v0 = 0xdade
#set $v1 = 0xdeadbeef
#set $t0 = 16
srav $v0, $v1, $t0
#show $v0 hex

#stop