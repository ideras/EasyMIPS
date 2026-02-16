.global print1
.global str

.data
str: .byte "Hello World", 10, 0
num: .word 0xfacefeaa

.text
print1:
    #show word (num) hex
    jr $ra