	.data

.global _a

	.align 4
_a:
	.float 0.000000000000
.global _b
	.align 4
_b:
	.float 1.000000000000
.global _c
	.align 4
_c:
	.float 1.000000000000
.global _d
	.align 4
_d:
	.float -1.000000000000


	.align 4

	.text

.global _main
_main:
        lf f4,_a		;a se almacena en f4
	lf f5,_b		;b se almacena en f5
       	lf f8,_c		;c se almacena en f8
	multf f7,f4,f5	;d se almacena en f9
	lf f9,_d
	addf f6,f4,f5
	addf f10,f8,f9
	multf f11,f8,f9
	nop
	addf f12,f6,f10
	multf f13,f6,f7
	nop
	nop
	nop
	nop
	multf f15,f7,f11
        sf    _a,f12	
	nop
	nop
	multf f14,f10,f11
	nop
	nop
	nop
	addf f17,f12,f15	
	sf    _c,f14
	addf  f16,f13,f14
	;nop
	sf    _d,f17	
	sf    _b,f16


L1:
	trap #0
	nop
