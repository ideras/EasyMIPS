#set $t0 = 0xfeaa
#set $t1 = 0x70000000
#set $t2 = 0x70000000
add $t0, $t1, $t2
#show $t0

; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+)\},[ ]*[^)]*\);
; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+)\}\);
; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{[ ]*Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+)[ ]*\}\);
; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{[ ]*Reg::([A-Za-z0-9]+)[ ]*\}\);
; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{Reg::([A-Za-z0-9]+),[ ]*Reg::([A-Za-z0-9]+),[ ]*([A-Fa-f0-9xX]+)\}\);

; CHECK\([ ]*reg.([a-zA-Z0-9]+)[ ]*==[ ]*[^\n]*

; reg\.([0-9a-zA-Z])+

; testInst\(ctx,[ ]*opc::([A-Za-z]+),[ ]*\{Reg::([A-Za-z0-9]+),[ ]*([xXA-Fa-f0-9]+),[ ]*Reg::([A-Za-z0-9]+)\}\);