; 32 x 32 => 64 bits

#set $t0 = -100
#set $t1 = 3

divu $t0, $t1
; Divu utiliza 2 registros especiales
; hi y lo para guardar el conciente y
; el residuo

#show $hi unsigned decimal
#show $lo unsigned decimal

