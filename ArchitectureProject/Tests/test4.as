# A demonstration of some simple MIPS instructions
# This file demonstrates second error: define a label more than one
main    add 3, 2, 1     # $3 = $2 + $1
        sub 4, 3, 1     # $4 = $3 - $1
        slt 5, 4, 3     # if $4 < $3 : $5 will be 1; otherwise 0
        beq 5, 0, l1    # if $5 == $0 go to label l1; else continue
        addi 5, 5, 100
        j   end
l1      lw 6, 0, one    # $6 = Mem[0+10]
        ori 6, 6, 20
end     halt
one     .fill 1
two     .fill 2
l1      add 1,2,3
