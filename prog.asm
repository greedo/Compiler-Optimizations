	# standard Decaf preamble 
	  .text
	  .align 2
	  .globl main
  _tester:
	# BeginFunc 188
	  subu $sp, $sp, 8	# decrement sp to make space to save ra, fp
	  sw $fp, 8($sp)	# save fp
	  sw $ra, 4($sp)	# save ra
	  addiu $fp, $sp, 8	# set up new fp
	  subu $sp, $sp, 188	# decrement sp to make space for locals/temps
	# _tmp0 = 1
need to spill
-1073757529
Register is: 0	  sw $zero, -16($fp)	# spill _tmp0 from $zero to $fp-16
	  li $t0, 1		# load constant value 1 into $t0
	# _tmp1 = 0
need to spill
-1073757529
Register is: 0	  sw $zero, -20($fp)	# spill _tmp1 from $zero to $fp-20
	  li $t1, 0		# load constant value 0 into $t1
	# _tmp2 = _tmp0 < _tmp1
	  lw $t0, -16($fp)	# fill _tmp0 to $t0 from $fp-16
	  lw $t1, -20($fp)	# fill _tmp1 to $t1 from $fp-20
	  slt $t2, $t0, $t1	
	  sw $t2, -24($fp)	# spill _tmp2 from $t2 to $fp-24
	# IfZ _tmp2 Goto _L0
	  lw $t0, -24($fp)	# fill _tmp2 to $t0 from $fp-24
	  beqz $t0, _L0	# branch if _tmp2 is zero 
	# _tmp3 = "Decaf runtime error: Array size is <= 0\n"
	  .data			# create string constant marked with label
	  _string1: .asciiz "Decaf runtime error: Array size is <= 0\n"
	  .text
	  la $t2, _string1	# load label
	# PushParam _tmp3
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -28($fp)	# fill _tmp3 to $t0 from $fp-28
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# LCall _Halt
	  jal _Halt          	# jump to function
  _L0:
	# _tmp4 = 1
need to spill
-1073757529
Register is: 0	  sw $zero, -32($fp)	# spill _tmp4 from $zero to $fp-32
	  li $t2, 1		# load constant value 1 into $t2
	# _tmp5 = _tmp4 + _tmp0
	  lw $t0, -32($fp)	# fill _tmp4 to $t0 from $fp-32
	  lw $t1, -16($fp)	# fill _tmp0 to $t1 from $fp-16
	  add $t2, $t0, $t1	
	  sw $t2, -36($fp)	# spill _tmp5 from $t2 to $fp-36
	# _tmp6 = 4
need to spill
-1073757529
Register is: 0	  sw $zero, -40($fp)	# spill _tmp6 from $zero to $fp-40
	  li $t2, 4		# load constant value 4 into $t2
	# _tmp7 = _tmp5 * _tmp6
	  lw $t0, -36($fp)	# fill _tmp5 to $t0 from $fp-36
	  lw $t1, -40($fp)	# fill _tmp6 to $t1 from $fp-40
	  mul $t2, $t0, $t1	
	  sw $t2, -44($fp)	# spill _tmp7 from $t2 to $fp-44
	# PushParam _tmp7
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -44($fp)	# fill _tmp7 to $t0 from $fp-44
	  sw $t0, 4($sp)	# copy param value to stack
	# _tmp8 = LCall _Alloc
	  jal _Alloc         	# jump to function
	  move $t2, $v0		# copy function return value from $v0
	  sw $t2, -48($fp)	# spill _tmp8 from $t2 to $fp-48
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# *(_tmp8) = _tmp0
	  lw $t2, -16($fp)	# fill _tmp0 to $t2 from $fp-16
	  lw $t3, -48($fp)	# fill _tmp8 to $t3 from $fp-48
	  sw $t0, 0($t2) 	# store with offset
	# _tmp9 = _tmp8 + _tmp6
	  lw $t0, -48($fp)	# fill _tmp8 to $t0 from $fp-48
	  lw $t1, -40($fp)	# fill _tmp6 to $t1 from $fp-40
	  add $t2, $t0, $t1	
	  sw $t2, -52($fp)	# spill _tmp9 from $t2 to $fp-52
	# b = _tmp9
	  lw $t2, -52($fp)	# fill _tmp9 to $t2 from $fp-52
	# _tmp10 = 0
need to spill
-1073757529
Register is: 0	  sw $zero, -56($fp)	# spill _tmp10 from $zero to $fp-56
	  li $t4, 0		# load constant value 0 into $t4
	# _tmp11 = sz < _tmp10
	  lw $t0, 4($fp)	# fill sz to $t0 from $fp+4
	  lw $t1, -56($fp)	# fill _tmp10 to $t1 from $fp-56
	  slt $t2, $t0, $t1	
	  sw $t2, -60($fp)	# spill _tmp11 from $t2 to $fp-60
	# IfZ _tmp11 Goto _L1
	  lw $t0, -60($fp)	# fill _tmp11 to $t0 from $fp-60
	  beqz $t0, _L1	# branch if _tmp11 is zero 
	# _tmp12 = "Decaf runtime error: Array size is <= 0\n"
	  .data			# create string constant marked with label
	  _string2: .asciiz "Decaf runtime error: Array size is <= 0\n"
	  .text
	  la $t2, _string2	# load label
	# PushParam _tmp12
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -64($fp)	# fill _tmp12 to $t0 from $fp-64
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# LCall _Halt
	  jal _Halt          	# jump to function
  _L1:
	# _tmp13 = 1
need to spill
-1073757529
Register is: 0	  sw $zero, -68($fp)	# spill _tmp13 from $zero to $fp-68
	  li $t2, 1		# load constant value 1 into $t2
	# _tmp14 = _tmp13 + sz
	  lw $t0, -68($fp)	# fill _tmp13 to $t0 from $fp-68
	  lw $t1, 4($fp)	# fill sz to $t1 from $fp+4
	  add $t2, $t0, $t1	
	  sw $t2, -72($fp)	# spill _tmp14 from $t2 to $fp-72
	# _tmp15 = 4
need to spill
-1073757529
Register is: 0	  sw $zero, -76($fp)	# spill _tmp15 from $zero to $fp-76
	  li $t2, 4		# load constant value 4 into $t2
	# _tmp16 = _tmp14 * _tmp15
	  lw $t0, -72($fp)	# fill _tmp14 to $t0 from $fp-72
	  lw $t1, -76($fp)	# fill _tmp15 to $t1 from $fp-76
	  mul $t2, $t0, $t1	
	  sw $t2, -80($fp)	# spill _tmp16 from $t2 to $fp-80
	# PushParam _tmp16
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -80($fp)	# fill _tmp16 to $t0 from $fp-80
	  sw $t0, 4($sp)	# copy param value to stack
	# _tmp17 = LCall _Alloc
	  jal _Alloc         	# jump to function
	  move $t2, $v0		# copy function return value from $v0
	  sw $t2, -84($fp)	# spill _tmp17 from $t2 to $fp-84
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# *(_tmp17) = sz
	  lw $t2, 4($fp)	# fill sz to $t2 from $fp+4
	  lw $t5, -84($fp)	# fill _tmp17 to $t5 from $fp-84
	  sw $t0, 0($t2) 	# store with offset
	# _tmp18 = _tmp17 + _tmp15
	  lw $t0, -84($fp)	# fill _tmp17 to $t0 from $fp-84
	  lw $t1, -76($fp)	# fill _tmp15 to $t1 from $fp-76
	  add $t2, $t0, $t1	
	  sw $t2, -88($fp)	# spill _tmp18 from $t2 to $fp-88
	# result = _tmp18
	  lw $t2, -88($fp)	# fill _tmp18 to $t2 from $fp-88
	# _tmp19 = 0
need to spill
-1073757529
Register is: 0	  sw $zero, -92($fp)	# spill _tmp19 from $zero to $fp-92
	  li $t6, 0		# load constant value 0 into $t6
	# i = _tmp19
need to spill
  _L2:
	# _tmp20 = i < sz
	  lw $t0, -8($fp)	# fill i to $t0 from $fp-8
	  lw $t1, 4($fp)	# fill sz to $t1 from $fp+4
	  slt $t2, $t0, $t1	
	  sw $t2, -96($fp)	# spill _tmp20 from $t2 to $fp-96
	# IfZ _tmp20 Goto _L3
	  lw $t0, -96($fp)	# fill _tmp20 to $t0 from $fp-96
	  beqz $t0, _L3	# branch if _tmp20 is zero 
	# _tmp21 = 0
need to spill
-1073757529
Register is: 0	  sw $zero, -100($fp)	# spill _tmp21 from $zero to $fp-100
	  li $t2, 0		# load constant value 0 into $t2
	# _tmp22 = i < _tmp21
	  lw $t0, -8($fp)	# fill i to $t0 from $fp-8
	  lw $t1, -100($fp)	# fill _tmp21 to $t1 from $fp-100
	  slt $t2, $t0, $t1	
	  sw $t2, -104($fp)	# spill _tmp22 from $t2 to $fp-104
	# _tmp23 = *(result + -4)
	  lw $t0, -12($fp)	# fill result to $t0 from $fp-12
	  lw $t2, -4($t0) 	# load with offset
	  sw $t2, -108($fp)	# spill _tmp23 from $t2 to $fp-108
	# _tmp24 = i < _tmp23
	  lw $t0, -8($fp)	# fill i to $t0 from $fp-8
	  lw $t1, -108($fp)	# fill _tmp23 to $t1 from $fp-108
	  slt $t2, $t0, $t1	
	  sw $t2, -112($fp)	# spill _tmp24 from $t2 to $fp-112
	# _tmp25 = _tmp24 == _tmp21
	  lw $t0, -112($fp)	# fill _tmp24 to $t0 from $fp-112
	  lw $t1, -100($fp)	# fill _tmp21 to $t1 from $fp-100
	  seq $t2, $t0, $t1	
	  sw $t2, -116($fp)	# spill _tmp25 from $t2 to $fp-116
	# _tmp26 = _tmp22 || _tmp25
	  lw $t0, -104($fp)	# fill _tmp22 to $t0 from $fp-104
	  lw $t1, -116($fp)	# fill _tmp25 to $t1 from $fp-116
	  or $t2, $t0, $t1	
	  sw $t2, -120($fp)	# spill _tmp26 from $t2 to $fp-120
	# IfZ _tmp26 Goto _L4
	  lw $t0, -120($fp)	# fill _tmp26 to $t0 from $fp-120
	  beqz $t0, _L4	# branch if _tmp26 is zero 
	# _tmp27 = "Decaf runtime error: Array subscript out of bound..."
	  .data			# create string constant marked with label
	  _string3: .asciiz "Decaf runtime error: Array subscript out of bounds\n"
	  .text
	  la $t2, _string3	# load label
	# PushParam _tmp27
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -124($fp)	# fill _tmp27 to $t0 from $fp-124
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# LCall _Halt
	  jal _Halt          	# jump to function
  _L4:
	# _tmp28 = 4
need to spill
-1073757529
Register is: 0	  sw $zero, -128($fp)	# spill _tmp28 from $zero to $fp-128
	  li $t2, 4		# load constant value 4 into $t2
	# _tmp29 = _tmp28 * i
	  lw $t0, -128($fp)	# fill _tmp28 to $t0 from $fp-128
	  lw $t1, -8($fp)	# fill i to $t1 from $fp-8
	  mul $t2, $t0, $t1	
	  sw $t2, -132($fp)	# spill _tmp29 from $t2 to $fp-132
	# _tmp30 = result + _tmp29
	  lw $t0, -12($fp)	# fill result to $t0 from $fp-12
	  lw $t1, -132($fp)	# fill _tmp29 to $t1 from $fp-132
	  add $t2, $t0, $t1	
	  sw $t2, -136($fp)	# spill _tmp30 from $t2 to $fp-136
	# *(_tmp30) = i
	  lw $t2, -8($fp)	# fill i to $t2 from $fp-8
need to spill
	  sw $t0, 0($t2) 	# store with offset
	# _tmp31 = 1
	  li (null), 1		# load constant value 1 into (null)
	# _tmp32 = i + _tmp31
	  lw $t0, -8($fp)	# fill i to $t0 from $fp-8
	  lw $t1, -140($fp)	# fill _tmp31 to $t1 from $fp-140
	  add $t2, $t0, $t1	
	  sw $t2, -144($fp)	# spill _tmp32 from $t2 to $fp-144
	# i = _tmp32
	  lw $t2, -144($fp)	# fill _tmp32 to $t2 from $fp-144
	# Goto _L2
	  b _L2		# unconditional branch
  _L3:
	# _tmp33 = 0
	  li (null), 0		# load constant value 0 into (null)
	# _tmp34 = 0
	  li (null), 0		# load constant value 0 into (null)
	# _tmp35 = _tmp33 < _tmp34
	  lw $t0, -148($fp)	# fill _tmp33 to $t0 from $fp-148
	  lw $t1, -152($fp)	# fill _tmp34 to $t1 from $fp-152
	  slt $t2, $t0, $t1	
	  sw $t2, -156($fp)	# spill _tmp35 from $t2 to $fp-156
	# _tmp36 = *(b + -4)
	  lw $t0, 4($gp)	# fill b to $t0 from $gp+4
	  lw $t2, -4($t0) 	# load with offset
	  sw $t2, -160($fp)	# spill _tmp36 from $t2 to $fp-160
	# _tmp37 = _tmp33 < _tmp36
	  lw $t0, -148($fp)	# fill _tmp33 to $t0 from $fp-148
	  lw $t1, -160($fp)	# fill _tmp36 to $t1 from $fp-160
	  slt $t2, $t0, $t1	
	  sw $t2, -164($fp)	# spill _tmp37 from $t2 to $fp-164
	# _tmp38 = _tmp37 == _tmp34
	  lw $t0, -164($fp)	# fill _tmp37 to $t0 from $fp-164
	  lw $t1, -152($fp)	# fill _tmp34 to $t1 from $fp-152
	  seq $t2, $t0, $t1	
	  sw $t2, -168($fp)	# spill _tmp38 from $t2 to $fp-168
	# _tmp39 = _tmp35 || _tmp38
	  lw $t0, -156($fp)	# fill _tmp35 to $t0 from $fp-156
	  lw $t1, -168($fp)	# fill _tmp38 to $t1 from $fp-168
	  or $t2, $t0, $t1	
	  sw $t2, -172($fp)	# spill _tmp39 from $t2 to $fp-172
	# IfZ _tmp39 Goto _L5
	  lw $t0, -172($fp)	# fill _tmp39 to $t0 from $fp-172
	  beqz $t0, _L5	# branch if _tmp39 is zero 
	# _tmp40 = "Decaf runtime error: Array subscript out of bound..."
	  .data			# create string constant marked with label
	  _string4: .asciiz "Decaf runtime error: Array subscript out of bounds\n"
	  .text
	  la $t2, _string4	# load label
	# PushParam _tmp40
	  subu $sp, $sp, 4	# decrement sp to make space for param
	  lw $t0, -176($fp)	# fill _tmp40 to $t0 from $fp-176
	  sw $t0, 4($sp)	# copy param value to stack
	# LCall _PrintString
	  jal _PrintString   	# jump to function
	# PopParams 4
	  add $sp, $sp, 4	# pop params off stack
	# LCall _Halt
	  jal _Halt          	# jump to function
  _L5:
	# _tmp41 = 4
need to spill
-1073757529
Register is: 0	  sw $zero, -180($fp)	# spill _tmp41 from $zero to $fp-180
	  li $t2, 4		# load constant value 4 into $t2
	# _tmp42 = _tmp41 * _tmp33
	  lw $t0, -180($fp)	# fill _tmp41 to $t0 from $fp-180
	  lw $t1, -148($fp)	# fill _tmp33 to $t1 from $fp-148
	  mul $t2, $t0, $t1	
	  sw $t2, -184($fp)	# spill _tmp42 from $t2 to $fp-184
	# _tmp43 = b + _tmp42
	  lw $t0, 4($gp)	# fill b to $t0 from $gp+4
	  lw $t1, -184($fp)	# fill _tmp42 to $t1 from $fp-184
	  add $t2, $t0, $t1	
	  sw $t2, -188($fp)	# spill _tmp43 from $t2 to $fp-188
	# _tmp44 = "Done"
	  .data			# create string constant marked with label
	  _string5: .asciiz "Done"
	  .text
	  la $t2, _string5	# load label
	# *(_tmp43) = _tmp44
	  lw $t2, -192($fp)	# fill _tmp44 to $t2 from $fp-192
need to spill
