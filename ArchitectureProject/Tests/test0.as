# this is the document example
        lw 1,0,five	    # o
        lw 2,1,2	    # l
start   add 1,1,2	    # l
        beq 0,1,done    # k
        j start		    # l
done	halt		    # k
five	.fill	5	    # k
neg1	.fill	-1	    # k
stAddr	.fill	start	# l
