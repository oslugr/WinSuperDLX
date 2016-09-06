	.data

.global a

	.align 4
a:
	.float 0.000000000000
.global xtops
	.align 4
xtop:
	.word 1

	.text

.global start

start:
   ld      f2,a
   addi    r2,r0,#192
   add     r1,r0,xtop
   
loop:

   ld      f0,0(r1)
   ld      f6,-8(r1)


   eqf     f0,f6
   bfpf    et2

   multd   f4,f6,f2
   eqf	   f4,f6
   bfpf    et1
   addd    f6,f4,f2
   addd    f4,f0,f2
et1:
   sd      0(r1),f4
   sd      -8(r1),f6
et2: 
   sub     r2,r2,#16
   bnez    r2,loop
   add     r1,r1,#1
   add     r1,r1,#1
   add     r1,r1,#1
   add     r1,r1,#1
   add     r1,r1,#1
   nop
   trap     #0
