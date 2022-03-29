	lw	0	4	mcand	r4=M[mcand]=mcand val
	lw	0	3	mask	r3=M[mask]=mask val
	lw	0	2	mplier	r2=M[mplier]=mplier val
	nor	0	2	2	r2=~(r2|0)=~(mplier|0)
	add	0	0	7	r7=0
loop	lw	0	5	num	r5=16384 
	beq	3	5	final	//mask done -> halt
	nor	0	3	3	r3=~(r3|0)=~(mask|0)
	nor	2	3	6	r6=~(r2|r3)=now_mask&mplier=current multi bit (0/1)
	nor	0	3	3	r3=~(r3|0)=~(mask|0)->back to mask val
	beq	0	7	1
	add	4	4	4	r4(mcand) r4<<1
	add	0	3	7
	add	3	3	3	r3=r3+r3 mask<<1
	beq	0	6	loop	//current mplier bit=0->loop to next mplier bit
	add	1	4	1	r1(result)+=r4(now_mcand)
	lw	0	5	start	r5=loop
	jalr	5	7
	halt
final	nor	0	3	3	r3=~(r3|0)=~(mask|0)
	nor	2	3	6	r6=~(r2|r3)=now_mask&mplier=current multi bit (0/1)
	nor	0	3	3	r3=~(r3|0)=~(mask|0)->back to mask val
	add	4	4	4	r4(mcand) r4<<1
	beq	0	6	1
	add	1	4	1	r1(result)+=r4(now_mcand)
	halt
mcand	.fill	1103	at most 15-bits; positive
mplier	.fill	7043	at most 15-bits; positive
mask	.fill	1	mask begins at 1
start	.fill	loop
num	.fill	16384