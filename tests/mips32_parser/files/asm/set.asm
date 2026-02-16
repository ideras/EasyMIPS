; Set register
#set $t0 = 10
#set $t0 = 0xdeadbeef
#set $t0 = -125
#set $t0 = 0b11001101
#set $t0 = $t1
#set $t0 = 4($s0)

; Set memory without size directive
#set 0($s0) = $t0
#set 0($s0) = 4($s0)
#set 0($s0) = 10
#set 0($s0) = 0xdeadbeef
#set 0($s0) = -125
#set 0($s0) = 0b11001101

; Set byte on memory
#set byte 0($s0) = 10
#set byte 0($s0) = 0xdeadbeef
#set byte 0($s0) = -125
#set byte 0($s0) = 0b11001101

; Set hword on memory
#set hword 0($s0) = 10
#set hword 0($s0) = 0xdeadbeef
#set hword 0($s0) = -125
#set hword 0($s0) = 0b11001101

; Set word on memory
#set word 0($s0) = 10
#set word 0($s0) = 0xdeadbeef
#set word 0($s0) = -125
#set word 0($s0) = 0b11001101

; Set array on memory
#set byte 0($s0) = [10, 0xaa, 'H', -115, 0b11001101]
#set hword 0($s0) = [10, 0xaabb, 'H', -6545, 0b11001101]
#set word 0($s0) = [10, 0xaabbccdd, 'H', -437823, 0b11001101]

; Set array on memory with size specifier
#set byte 0($s0):4 = [10, 0xaa, 'H', -115, 0b11001101]
#set hword 0($s0):4 = [10, 0xaabb, 'H', -6545, 0b11001101]
#set word 0($s0):4 = [10, 0xaabbccdd, 'H', -437823, 0b11001101]