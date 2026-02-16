; Show register
#show $t0 
#show $t0 hex
#show $t0 hexadecimal
#show $t0 decimal
#show $t0 signed decimal
#show $t0 unsigned decimal
#show $t0 binary

; Show memory without size directive
#show 0($s0) 
#show 0($s0) hex
#show 0($s0) hexadecimal
#show 0($s0) decimal
#show 0($s0) signed decimal
#show 0($s0) unsigned decimal
#show 0($s0) binary

; Show memory byte
#show byte 0($s0) 
#show byte 0($s0) hex
#show byte 0($s0) hexadecimal
#show byte 0($s0) decimal
#show byte 0($s0) signed decimal
#show byte 0($s0) unsigned decimal
#show byte 0($s0) binary

; Show memory half word
#show hword 0($s0) 
#show hword 0($s0) hex
#show hword 0($s0) hexadecimal
#show hword 0($s0) decimal
#show hword 0($s0) signed decimal
#show hword 0($s0) unsigned decimal
#show hword 0($s0) binary

; Show memory word
#show word 0($s0) 
#show word 0($s0) hex
#show word 0($s0) hexadecimal
#show word 0($s0) decimal
#show word 0($s0) signed decimal
#show word 0($s0) unsigned decimal
#show word 0($s0) binary

; Show memory byte array
#show byte 0($s0):16 
#show byte 0($s0):16 hex
#show byte 0($s0):16 hexadecimal
#show byte 0($s0):16 decimal
#show byte 0($s0):16 signed decimal
#show byte 0($s0):16 unsigned decimal
#show byte 0($s0):16 binary

; Show memory half word array
#show hword 0($s0):16 
#show hword 0($s0):16 hex
#show hword 0($s0):16 hexadecimal
#show hword 0($s0):16 decimal
#show hword 0($s0):16 signed decimal
#show hword 0($s0):16 unsigned decimal
#show hword 0($s0):16 binary

; Show memory word array
#show word 0($s0):16 
#show word 0($s0):16 hex
#show word 0($s0):16 hexadecimal
#show word 0($s0):16 decimal
#show word 0($s0):16 signed decimal
#show word 0($s0):16 unsigned decimal
#show word 0($s0):16 binary

; Show memory using immediate address 
#show word (0xffff0008):16 
#show word (0xffff0008):16 hex
#show word (0xffff0008):16 hexadecimal
#show word (0xffff0008):16 decimal
#show word (0xffff0008):16 signed decimal
#show word (0xffff0008):16 unsigned decimal
#show word (0xffff0008):16 binary