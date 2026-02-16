; lui
#set $v0 = 0xcaca2fea
lui $v0, 0xdead
#show $v0 hex

; move
#set $v0 = 0xba5474
#set $v1 = 0xdeadbeef
move $v0, $v1
#show $v0 hex

; la
#set $v0 = 0xba5474
la $v0, 0xdeadbeef
#show $v0 hex

; li
#set $v0 = 0xba5474
li $v0, 0xdeadbeef
#show $v0 hex