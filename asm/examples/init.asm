.data
    .byte "Computer Organization", 0
    .word 0xdeadbeef, 0xbadface, 0xfacefea

.text
    #show [27, "[34mWelcome to Computer Organization", 27, "[0m"] ascii
    #show [27, "[32mPeriod 4 2020", 27, "[0m"] ascii
    #show ["================================"] ascii
    #set $t0 = 6454
    #set $t1 = 342
    #set $t2 = 925
    #set $t3 = 8035
