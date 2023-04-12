# A demonstration of some simple MIPS instructions
# This file demonstrates or,nand,slti,lui,sw,jalr insts.
main    or 3, 2, 1      # $3 = $2 or $1
        nand 4, 3, 1    # $4 = $3 nand $1
        slti 5, 4, 300  # if $4 < 300 : $5 will be 1; otherwise 0
        lui 5, two      # $5 = imm << 16
        sw 5, 0, two    # save value of reg $5 in Mem[0+13]
        lw 6, 0, foo    # load address of 'foo' in $6
        jalr 4, 6       # $4 = pc + 1 , pc = $6
end     halt
foo     lw 6, 0, one    # $6 = Mem[0+12]
        ori 6, 6, 20
one     .fill 1
two     .fill 2
mainAd  .fill main
