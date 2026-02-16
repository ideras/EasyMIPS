; Jal
#set $a0 = 20
jal test0
#show $v0

; Jalr
#set $t0 = test0
#set $a0 = 200
jalr $t0
#show $v0

; J
j $LC0
#set $v0 = 0xdec0de
#show $v0 hex

$LC0:
    #set $v0 = 0xaabbccdd
    #show $v0 hex
    
; Beq
#set $v0 = 0xdeadbeef
#set $v1 = 0xdeadbeef
beq $v0, $v1, $LC1
#set $v0 = 0xdec0de
#show $v0 hex

$LC1:
    #set $v0 = 0x11223344
    #show $v0 hex

#set $v1 = 0xeeadbeef
beq $v0, $v1, $LC2

#set $v0 = 0xdec0de
#show $v0 hex

$LC2:
    #set $v0 = 0x0badface
    #show $v0 hex

; Bne
#set $v0 = 0xdeadbeef
#set $v1 = 0xdeadbeef
bne $v0, $v1, $LC11
#set $v0 = 0xdec0de
#show $v0 hex

$LC11:
    #set $v0 = 0x11223344
    #show $v0 hex

#set $v1 = 0xeeadbeef
bne $v0, $v1, $LC21

#set $v0 = 0xdec0de
#show $v0 hex
$LC21:
    #set $v0 = 0x0badface
    #show $v0 hex

; Bltz
#set $v0 = 0xdeadbeef
bltz $v0, $LC3
#set $v0 = 0xdec0de
#show $v0 hex
$LC3:
    #set $v0 = 0x0badface
    #show $v0 hex

#set $v0 = 0x7eadbeef
bltz $v0, $LC4
#set $v0 = 0xdec0de
#show $v0 hex
$LC4:
    #set $v0 = 0x0badface
    #show $v0 hex

; Bgtz
#set $v0 = 0xdeadbeef
bgtz $v0, $LC31
#set $v0 = 0xdec0de
#show $v0 hex
$LC31:
    #set $v0 = 0x0badface
    #show $v0 hex

#set $v0 = 0x7eadbeef
bgtz $v0, $LC41
#set $v0 = 0xdec0de
#show $v0 hex
$LC41:
    #set $v0 = 0x0badface
    #show $v0 hex

; Bgez
#set $v0 = 0xdeadbeef
bgez $v0, $LC32
#set $v0 = 0xdec0de
#show $v0 hex
$LC32:
    #set $v0 = 0x0badface
    #show $v0 hex

#set $v0 = 0x7eadbeef
bgez $v0, $LC42
#set $v0 = 0xdec0de
#show $v0 hex
$LC42:
    #set $v0 = 0x0badface
    #show $v0 hex

; Blez
#set $v0 = 0xdeadbeef
blez $v0, $LC33
#set $v0 = 0xdec0de
#show $v0 hex
$LC33:
    #set $v0 = 0x0badface
    #show $v0 hex

#set $v0 = 0x7eadbeef
blez $v0, $LC43
#set $v0 = 0xdec0de
#show $v0 hex
$LC43:
    #set $v0 = 0x0badface
    #show $v0 hex

#stop

test0:
    addi $v0, $a0, 100
    jr $ra