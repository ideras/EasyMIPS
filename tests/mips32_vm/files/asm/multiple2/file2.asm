
.global main

.data
$LB:  .byte 0xde
$LHW: .hword 0xbaca,  0xbeef

.text

main:
    move $s0, $ra
    jal print1

    #show byte (str):11 ascii
    #show byte ($LB) hex
    #show hword ($LHW):2 hex

    jr $s0