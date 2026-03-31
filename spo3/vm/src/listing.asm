[section constants]

const_14: dw 47			;	'/'
const_13: dw 42			;	'*'
const_12: dw 45			;	'-'
const_11: dw 43			;	'+'
const_10: dw str_10
str_10: db 61, 32, 0			;	"= "
const_9: dw 113			;	'q'
const_8: dw 1			;	true
const_7: dw 3			;	3
const_6: dw str_6
str_6: db 62, 58, 32, 0			;	">: "
const_5: dw 13			;	13
const_4: dw 48			;	'0'
const_3: dw 10			;	10
const_2: dw 9			;	9
const_1: dw 1			;	1
const_0: dw 0			;	0

[section code]

CALL main 0
HLT

put_char:
load_data 0		;	c
OUT
RET

get_char:
IN
RET

main:
CALL calc 2
RET



calc:
load_const const_6
load_const const_7
store 4		;	call arg size
store 2		;	call arg str
CALL put_str 2
CALL get_int 4
store 0		;	var a
.WHILE_34:
load_const const_8
JZ .after_while_35
load_const const_6
load_const const_7
store 6		;	call arg size
store 4		;	call arg str
CALL put_str 4
CALL get_char 6
store 2		;	var op
CALL get_char 6
CALL get_char 6
load_data 2		;	var op
load_const const_9
EQ
JNZ .then_after_while_35
.after_if_40:
load_const const_6
load_const const_7
store 8		;	call arg size
store 6		;	call arg str
CALL put_str 6
CALL get_int 8
store 4		;	var b
load_const const_10
load_const const_7
store 10		;	call arg size
store 8		;	call arg str
CALL put_str 8
load_data 2		;	var op
load_const const_11
EQ
JNZ .then_EXPRESSION_59
load_data 2		;	var op
load_const const_12
EQ
JNZ .then_EXPRESSION_57
load_data 2		;	var op
load_const const_13
EQ
JNZ .then_EXPRESSION_55
load_data 2		;	var op
load_const const_14
EQ
JNZ .then_EXPRESSION_53
.after_if_51:
.after_if_49:
.after_if_47:
.after_if_45:
load_data 0		;	var a
store 8		;	call arg a
CALL put_int 8
load_const const_3
store 8		;	call arg c
CALL put_char 8
JMP .WHILE_34
.then_EXPRESSION_53:
load_data 0		;	var a
load_data 4		;	var b
DIV
store 0		;	var a
JMP .after_if_51
.then_EXPRESSION_55:
load_data 0		;	var a
load_data 4		;	var b
MUL
store 0		;	var a
JMP .after_if_49
.then_EXPRESSION_57:
load_data 0		;	var a
load_data 4		;	var b
SUB
store 0		;	var a
JMP .after_if_47
.then_EXPRESSION_59:
load_data 0		;	var a
load_data 4		;	var b
ADD
store 0		;	var a
JMP .after_if_45
.then_after_while_35:
.after_while_35:
RET
JMP .after_while_35




put_int:
load_data 0		;	var a
load_const const_2
GT
JNZ .then_EXPRESSION_13
.after_if_11:
load_data 0		;	var a
load_const const_0
NEQ
JNZ .then_EXPRESSION_16
.after_if_14:
RET
.then_EXPRESSION_16:
load_data 0		;	var a
load_const const_3
MOD
load_const const_4
ADD
store 4		;	call arg c
CALL put_char 4
JMP .after_if_14
.then_EXPRESSION_13:
load_data 0		;	var a
load_const const_3
DIV
store 4		;	call arg a
CALL put_int 4
JMP .after_if_11






get_int:
load_const const_0
store 0		;	var a
CALL get_char 6
store 2		;	var c
.WHILE_22:
load_data 2		;	var c
load_const const_5
NEQ
JZ .after_while_23
load_const const_3
load_data 0		;	var a
MUL
load_data 2		;	var c
load_const const_4
SUB
ADD
store 0		;	var a
CALL get_char 6
store 2		;	var c
JMP .WHILE_22
.after_while_23:
CALL get_char 6
load_data 0		;	var a
RET




put_str:
load_const const_0
store 4		;	var i
.WHILE_3:
load_data 4		;	var i
load_data 2		;	var size
LT
JZ .after_while_4
load_data 4		;	var i
load_data_by_index 0
store 8		;	call arg c
CALL put_char 8
load_data 4		;	var i
load_const const_1
ADD
store 4		;	var i
JMP .WHILE_3
.after_while_4:
RET




