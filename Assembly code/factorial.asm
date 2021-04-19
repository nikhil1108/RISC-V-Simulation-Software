addi x10,x0,7
jal x1, factorial
beq x0, x0, EXIT
factorial:addi sp, sp, -8				
    sw x1, 4(sp)				
    sw x10, 0(sp)				
    addi x5, x10, -1			
    bge x5, x0, loop      			
    addi x10, x0, 1			
    addi sp, sp, 8				
    jalr x0, 0(x1)			
loop:   addi x10, x10, -1		# n -> n-1
        jal x1, factorial
        addi x6, x10, 0			# store n in x6
        lw x10, 0(sp)			
        lw x1, 4(sp)			
        mul x10, x10, x6		# n*fact(n-1)
        addi sp, sp, 8			
        jalr x0, 0(x1)			
EXIT: