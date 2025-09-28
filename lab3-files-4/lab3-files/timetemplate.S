# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
.macro	PUSH (%reg)
	addi	sp,sp,-4
	sw	%reg,0(sp) 
.end_macro

.macro	POP (%reg)
	lw	%reg,0(sp)
	addi	sp,sp,4
.end_macro
###################
# Macros for DTEK #
###################
#.macro	PUSH reg
#	addi sp,sp,-4
#	sw \reg,0(sp) 
#.endm

#.macro	POP reg
#	lw	\reg,0(sp)
#	addi	sp,sp,4
#.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 20000		# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################

time2string:
    # save caller context
    PUSH (ra)
    PUSH (s0)
    PUSH (s1)

    # save arguments into saved registers
    mv   s0, a0           # buffer pointer -> s0
    mv   s1, a1           # packed time value -> s1

    # extract nibbles from s1
    srli t2, s1, 12
    andi t2, t2, 0xF
    srli t3, s1, 8
    andi t3, t3, 0xF
    srli t4, s1, 4
    andi t4, t4, 0xF
    andi t5, s1, 0xF

    # convert each nibble into ASCII
    mv a0, t2
    jal hexasc
    mv t2, a0

    mv a0, t3
    jal hexasc
    mv t3, a0

    mv a0, t4
    jal hexasc
    mv t4, a0

    mv a0, t5
    jal hexasc
    mv t5, a0

    # write into buffer using s0
    sb t2, 0(s0)
    sb t3, 1(s0)
    li t1, 0x3A        # ':'
    sb t1, 2(s0)
    sb t4, 3(s0)
    sb t5, 4(s0)
    sb zero, 5(s0)     # null terminator

    # restore caller context
    POP (s1)
    POP (s0)
    POP (ra)

    jr ra
        
hex2asc:
	li	a0, 17		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr ra	

  # You can write your own code for hexasc here
  #
  hexasc:
  	andi a0, a0, 0xF #removes all but the 4 lowest bits (binary)
  	
  	li t0, 10 # t0 set to 10
  	li t1, 0x30 # character "0" in ascii
  	blt a0, t0, hexa_done #branch to hexa_done if a0 is less than 10 (decimal)
  	
  	# whether a0 is equal to or greater than 10 , we convert to A-F
  	addi a0, a0, -10 # go from 10 back down to zero
  	li t1, 0x41 # character "A" in ascii
  	
  hexa_done: 
  	add a0, a0, t1 # either "0" + a0 or "A" + (a0 - 10)
  	jr ra # jump back to hexa2asc

delay:
	bge x0, a0, delay_done # if zero (x0) is greater than or equal to a0, we branch to delay_done.
	
outloop:
	addi a0, a0, -1 # we decrement a0 (ms)
	li t0, 20000 # set t0 to 4711 at first, this is our i and corresponds to a 1ms delay 
	
inloop:
	addi t0, t0, -1 # decrement i each time
	bnez t0, inloop # if i isnt zero, we loop back to the inner loop.
	bnez a0, outloop # if ms isnt zero loop back to outer
	
delay_done:
	jr ra # returns to where it was called from
