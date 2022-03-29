combi	lw	0	6	pos1	r6=1
	sw	5	7	Stack	M[r5+Stack]=r7
	beq	1	2	l_ret
	beq	0	2	r_ret
	add	5	6	5	r5+=1
	sw	5	1	Stack	M[r5+Stack]=r1
	add	5	6	5	r5+=1
	sw	5	2	Stack	M[r5+Stack]=r2
	add	5	6	5	r5+=1
left	lw	0	6	neg1	r6=-1
	add	1	6	1	r1-=1
	lw	0	4	Caddr
	jalr	4	7
right	lw	0	6	neg1	r6=-1
	add	2	6	2	r2-=1
	lw	0	6	pos1	r6=1
	add	5	6	5	r5+=1
	lw	0	4	Caddr
	jalr	4	7		r7=return
return	lw	5	3	Stack	r3=M[r5+Stack]
	add	5	6	5	r5-=1
rcont	lw	5	4	Stack	r4=M[r5+Stack]
	add	3	4	3	r3+=r4
	add	5	6	5	r5-=1
	lw	5	2	Stack	r2=M[r5+Stack]
	add	5	6	5	r5-=1
	lw	5	1	Stack	r1=M[r5+Stack]
	add	5	6	5	r5-=1
	lw	5	7	Stack	r7=M[r5+Stack]
	sw	5	3	Stack	M[r5+Stack]=r3
	jalr	7	4
r_ret	beq	0	5	end
	lw	5	6	Stack
	lw	0	3	pos1	r3=1
	lw	0	6	neg1	r6=-1
	add	5	6	5	r5-=1
	beq	0	2	rcont
l_ret	beq	0	5	end
	lw	0	3	pos1	r3=1
	sw	5	3	Stack	M[r5+Stack]=r3
	beq	1	2	right
end	lw	0	3	pos1
	jalr	7	4
Caddr	.fill	combi
pos1	.fill	1
neg1	.fill	-1