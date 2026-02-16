; 32 x 32 => 64 bits

#set $t0 = -1
#set $t1 = 40

mult $t0, $t1
; Mult utiliza 2 rgistros especiales
; hi y lo, juntos forman un resultado ; de 64 bits

; mflo $t2
#show $hi hex
#show $lo hex

