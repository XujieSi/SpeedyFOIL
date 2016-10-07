#F: malloc,good1,good2,good3,bad1,bad2,bad3,bad4,bad5,notbad,notbad2,free,hello,hi.
#R: GT,LTE,NEQ,EQ.
#B: true,false.
#Z: ZERO,ONE.
L: 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,21,22,23,24,25,31,32,33,34,35,101,102,201,202,301,302,303,304,401,402,403,404,501,502.
#V: ptr_l1,ptr_l4,x_l5,ptr_l11,ptr_l21,ptr_l31,ptr_l201,y_l101,z_l102,x_l202,x_l10,x_l303,ptr_l403,x_l501,ptr_l502.

*is_zero(Z)
ZERO
.
*is_one(Z)
ONE
.
*is_eq(R)
EQ
.
*is_neq(R)
NEQ
.
*is_gt(R)
GT
.
*is_lte(R)
LTE
.
*fnCall(L,F)
1,malloc
11,malloc
21,malloc
31,malloc
4,malloc
5,malloc
201,malloc
303,malloc
403,malloc
501,hi
502,malloc
3,free
10,hello
101,hello
102,hello
202,hi
.
*fnCallRet(L,V)
1,ptr_l1
11,ptr_l11
21,ptr_l21
31,ptr_l31
4,ptr_l4
5,x_l5
10,x_l10
101,y_l101
102,z_l102
201,ptr_l201
202,x_l202
303,x_l303
403,ptr_l403
501,x_l501
502,ptr_l502
.
*fnRetV(F,L,V)
notbad,6,x_l5
bad2,8,ptr_l4
bad4,402,ptr_l403
notbad2,302,x_l303
notbad2,304,x_l303
.
*opSucc0(L,L)
1,2
2,3
3,9
7,8
8,4
4,10
5,101
101,102
102,6
11,12
11,14
21,22
22,23
21,24
24,25
31,32
32,33
31,34
34,35
201,202
301,302
302,303
303,304
401,402
402,403
403,404
501,502
.
opSucc(L,L)
1,2
1,3
1,9
2,3
2,9
3,9
7,8
7,4
7,10
8,4
8,10
4,10
5,101
5,102
5,6
101,102
101,6
102,6
11,12
11,14
21,22
21,23
21,24
21,25
22,23
24,25
31,32
31,33
31,34
31,35
32,33
34,35
201,202
301,302
301,303
301,304
302,303
302,304
303,304
401,402
401,403
401,404
402,403
402,404
403,404
501,502
.
*Check(L,R,V,Z)
12,NEQ,ptr_l11,ZERO
14,EQ,ptr_l11,ZERO
22,NEQ,ptr_l21,ZERO
24,EQ,ptr_l21,ZERO
32,NEQ,ptr_l31,ZERO
34,EQ,ptr_l31,ZERO
2,GT,ptr_l1, ONE
7,NEQ,ptr_l4,ZERO
301,GT,x_l303,ZERO
401,EQ,ptr_l403,ZERO
.
Err(L)
1
4
201
403
502
.
Ok(L)
11
21
31
5
303
.
