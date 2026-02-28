  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
	.globl hex2asc

hex2asc:
	li	a0, 17		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr      ra	

  # You can write your own code for hexasc here
  #
  hexasc:
  	andi a0, a0, 0xF #removes all but the 4 lowest bits (binary)
  	
  	li t0, 10 # t0 set to 10
  	li t1, 0x30 # character "0" in ascii
  	blt a0, t0, hexa_done #branch to hexa_done if a0 is less than 10 (decimal)
  	
  	# whether a0 is equal to or greater than 10 , we convert to A-F
	#
  	addi a0, a0, -10 # go from 10 back down to zero
  	li t1, 0x41 # character "A" in ascii
  	
  hexa_done: 
  	add a0, a0, t1 # either "0" + a0 or "A" + (a0 - 10)
  	jr ra # jump back to hexa2asc
  	
