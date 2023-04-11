add $t2, $t1, $s1	# $t1 + B[0] -> now $t2 is address of B[i]
lw $t3, 0($t2)		# load B[i] in $t3

add $t4, $t3, $t0 	# $t4 = B[i] + i
add $t2, $t1, $s0	# $t1 + A[0] -> now $t2 is address of A[i]
sw $t4, 0($t2)		# save $t4 in address of A[i]

addi $t0, $t0, 1	# i = i+1
slti $t5, $t0, 10	# check if i < 10
dalam
