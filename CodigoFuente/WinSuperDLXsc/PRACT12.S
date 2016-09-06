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
LC0:
	.float 4.000000000000
.global _cnd
_cnd:	.space 8
.global _ii
_ii:	.word  10
.global _n
_n:	.space 8
.global _m
_m:	.space 8
.global _l
_l:	.space 8
.global _k
_k:	.space 8
.global _j
_j:	.space 8
.global _i
_i:	.space 8
.global _h
_h:	.space 8
.global _g
_g:	.space 8
.global _f
_f:	.space 8
.global _e
_e:	.space 8

	.align 4

	.text
.global _main
_main:
	lhi r3,(_ii>>16)&0xffff
	addui r3,r3,(_ii&0xffff)
	addi r6,r0,#1
	sw 0(r3),r6
	lhi r3,(_cnd>>16)&0xffff
	addui r3,r3,(_cnd&0xffff)
	sw 0(r3),r0
L2:
	lhi r3,(_ii>>16)&0xffff
	addui r3,r3,(_ii&0xffff)
	lw r3,0(r3)
	addi r4,r0,#9
		;cmpsi	r3,r4
	sgt	r1,r3,r4
	bnez	r1,L3
	nop
	lhi r3,(_cnd>>16)&0xffff
	addui r3,r3,(_cnd&0xffff)
	lw r3,0(r3)
	addi r4,r0,#0
		;cmpsi	r3,r4
	sne	r1,r3,r4
	bnez	r1,L3
	nop
	lhi r3,(_e>>16)&0xffff
	addui r3,r3,(_e&0xffff)
	lhi r4,(_a>>16)&0xffff
	addui r4,r4,(_a&0xffff)
	lhi r5,(_b>>16)&0xffff
	addui r5,r5,(_b&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	addf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_f>>16)&0xffff
	addui r3,r3,(_f&0xffff)
	lhi r4,(_a>>16)&0xffff
	addui r4,r4,(_a&0xffff)
	lhi r5,(_b>>16)&0xffff
	addui r5,r5,(_b&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	multf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_g>>16)&0xffff
	addui r3,r3,(_g&0xffff)
	lhi r4,(_c>>16)&0xffff
	addui r4,r4,(_c&0xffff)
	lhi r5,(_d>>16)&0xffff
	addui r5,r5,(_d&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	addf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_h>>16)&0xffff
	addui r3,r3,(_h&0xffff)
	lhi r4,(_c>>16)&0xffff
	addui r4,r4,(_c&0xffff)
	lhi r5,(_d>>16)&0xffff
	addui r5,r5,(_d&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	multf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_i>>16)&0xffff
	addui r3,r3,(_i&0xffff)
	lhi r4,(_e>>16)&0xffff
	addui r4,r4,(_e&0xffff)
	lhi r5,(_g>>16)&0xffff
	addui r5,r5,(_g&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	addf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_j>>16)&0xffff
	addui r3,r3,(_j&0xffff)
	lhi r4,(_e>>16)&0xffff
	addui r4,r4,(_e&0xffff)
	lhi r5,(_f>>16)&0xffff
	addui r5,r5,(_f&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	multf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_k>>16)&0xffff
	addui r3,r3,(_k&0xffff)
	lhi r4,(_g>>16)&0xffff
	addui r4,r4,(_g&0xffff)
	lhi r5,(_h>>16)&0xffff
	addui r5,r5,(_h&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	multf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_l>>16)&0xffff
	addui r3,r3,(_l&0xffff)
	lhi r4,(_f>>16)&0xffff
	addui r4,r4,(_f&0xffff)
	lhi r5,(_h>>16)&0xffff
	addui r5,r5,(_h&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	multf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_m>>16)&0xffff
	addui r3,r3,(_m&0xffff)
	lhi r4,(_j>>16)&0xffff
	addui r4,r4,(_j&0xffff)
	lhi r5,(_k>>16)&0xffff
	addui r5,r5,(_k&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	addf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_n>>16)&0xffff
	addui r3,r3,(_n&0xffff)
	lhi r4,(_i>>16)&0xffff
	addui r4,r4,(_i&0xffff)
	lhi r5,(_l>>16)&0xffff
	addui r5,r5,(_l&0xffff)
	lf f4,0(r4)
	lf f5,0(r5)
	addf f4,f4,f5
	sf 0(r3),f4
	lhi r3,(_a>>16)&0xffff
	addui r3,r3,(_a&0xffff)
	lhi r4,(_i>>16)&0xffff
	addui r4,r4,(_i&0xffff)
	lw r6,0(r4)
	sw 0(r3),r6
	lhi r3,(_b>>16)&0xffff
	addui r3,r3,(_b&0xffff)
	lhi r4,(_m>>16)&0xffff
	addui r4,r4,(_m&0xffff)
	lw r6,0(r4)
	sw 0(r3),r6
	lhi r3,(_c>>16)&0xffff
	addui r3,r3,(_c&0xffff)
	lhi r4,(_k>>16)&0xffff
	addui r4,r4,(_k&0xffff)
	lw r6,0(r4)
	sw 0(r3),r6
	lhi r3,(_d>>16)&0xffff
	addui r3,r3,(_d&0xffff)
	lhi r4,(_n>>16)&0xffff
	addui r4,r4,(_n&0xffff)
	lw r6,0(r4)
	sw 0(r3),r6
	lhi r3,(_a>>16)&0xffff
	addui r3,r3,(_a&0xffff)
	lhi r4,(LC0>>16)&0xffff
	addui r4,r4,(LC0&0xffff)
	lf f4,0(r3)
	lf f5,0(r4)
		;cmpsf	f4,f5
	lef	f4,f5
	bfpf	L5
	nop
	lhi r3,(_b>>16)&0xffff
	addui r3,r3,(_b&0xffff)
	lhi r4,(LC0>>16)&0xffff
	addui r4,r4,(LC0&0xffff)
	lf f4,0(r3)
	lf f5,0(r4)
		;cmpsf	f4,f5
	lef	f4,f5
	bfpf	L5
	nop
	lhi r3,(_c>>16)&0xffff
	addui r3,r3,(_c&0xffff)
	lhi r4,(LC0>>16)&0xffff
	addui r4,r4,(LC0&0xffff)
	lf f4,0(r3)
	lf f5,0(r4)
		;cmpsf	f4,f5
	lef	f4,f5
	bfpf	L5
	nop
	lhi r3,(_d>>16)&0xffff
	addui r3,r3,(_d&0xffff)
	lhi r4,(LC0>>16)&0xffff
	addui r4,r4,(LC0&0xffff)
	lf f4,0(r3)
	lf f5,0(r4)
		;cmpsf	f4,f5
	lef	f4,f5
	bfpf	L5
	nop
	j L4
	nop
L5:
	lhi r3,(_cnd>>16)&0xffff
	addui r3,r3,(_cnd&0xffff)
	addi r6,r0,#1
	sw 0(r3),r6
L4:
	lhi r3,(_ii>>16)&0xffff
	addui r3,r3,(_ii&0xffff)
	lhi r4,(_ii>>16)&0xffff
	addui r4,r4,(_ii&0xffff)
	lw r4,0(r4)
	add r4,r4,#1
	sw 0(r3),r4
	j L2
	nop
L3:
L1:
	;; HALT
	trap #0
	nop
