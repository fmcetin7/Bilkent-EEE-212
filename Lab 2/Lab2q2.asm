start:
	jnb 52h, initialize
	acall keyboard

initialize:
	setb 52h; for initialization purposes
	mov r0, #3h
	acall configure_lcd

KEYBOARD_LOOP:
	acall KEYBOARD
	acall send_data
	mov @r0, A; input values will be stored in r3, r2 depending on how many strings there are
	dec r0
	cjne A, #23h, keyboard_loop; 23h is ASCII representation of #

digit_loop_1:
	cjne r2, #23h, digit_loop_2; check if input is 1 digit
	sjmp one_decimal
digit_loop_2:
	cjne r1, #23h, digit_loop_1; check if input is 2 digit
	sjmp two_decimal

one_decimal:
	mov a, r3
	anl a, #0fh
	sjmp ceren
	
two_decimal:
	mov a, r3
	anl a, #0fh
	mov b, #10d
	mul ab
	mov r3, a
	mov a, r2
	anl a, #0fh
	add a, r3
	sjmp ceren

ceren: ;day number finalised
	add a, #1h ;I added and later subtracted 1 from r7 to see the 00 seconds on the screen
	mov r7, a
	mov 31h, #0h ;31h is my flag for half seconds
	sjmp print


print:
	mov a, #01h
	acall send_command
	mov a, r7
	subb a, #1h
	mov b, #10d
	div ab
	jz one_digit ;don't want to see 09 08 07...; want to see 9 8 7 on LCD
	sjmp two_digit

two_digit:
	mov dptr, #digits
	movc a, @a+dptr
	acall send_data
	mov a, b
	movc a, @a+dptr
	acall send_data
	jnb 31h, full_sec
	sjmp half_sec

one_digit:
	mov dptr, #digits
	mov a, b
	movc a, @a+dptr
	acall send_data
	jnb 31h, full_sec
	sjmp half_sec

half_sec:
	mov a, #2eh
	acall send_data
	mov a, #35h
	acall send_data
	mov a, #20h
	acall send_data
	mov a, #53h
	acall send_data
	mov a, #45h
	acall send_data
	mov a, #43h
	acall send_data
	sjmp again

full_sec:
	mov a, #20h
	acall send_data
	mov a, #53h
	acall send_data
	mov a, #45h
	acall send_data
	mov a, #43h
	acall send_data
	sjmp again

again:
	mov r6, #0fh
	mov tmod, #10h
loop:	clr tf1
	mov th1, #98h ;high byte of 34817
	mov tl1, #01h ;low byte of 34817
	setb tr1
	jnb tf1, $
	djnz r6, loop
	cpl 31h
	jnb 31h, print
	djnz r7, print
	sjmp launch_print

launch_print:
	mov a, #01h
	acall send_command
	mov a, #4ch
	acall send_data
	mov a, #41h
	acall send_data
	mov a, #55h
	acall send_data
	mov a, #4eh
	acall send_data
	mov a, #43h
	acall send_data
	mov a, #48h
	acall send_data
	mov a, #21h
	acall send_data
	sjmp $




CONFIGURE_LCD:	;THIS SUBROUTINE SENDS THE INITIALIZATION COMMANDS TO THE LCD
	mov a,#38H	;TWO LINES, 5X7 MATRIX
	acall SEND_COMMAND
	mov a,#0FH	;DISPLAY ON, CURSOR BLINKING
	acall SEND_COMMAND
	mov a,#06H	;INCREMENT CURSOR (SHIFT CURSOR TO RIGHT)
	acall SEND_COMMAND
	mov a,#01H	;CLEAR DISPLAY SCREEN
	acall SEND_COMMAND
	mov a,#80H	;FORCE CURSOR TO BEGINNING OF THE FIRST LINE
	acall SEND_COMMAND
	ret



SEND_COMMAND:
	mov p1,a		;THE COMMAND IS STORED IN A, SEND IT TO LCD
	clr p3.5		;RS=0 BEFORE SENDING COMMAND
	clr p3.6		;R/W=0 TO WRITE
	setb p3.7	;SEND A HIGH TO LOW SIGNAL TO ENABLE PIN
	acall DELAY
	clr p3.7
	ret


SEND_DATA:
	mov p1,a		;SEND THE DATA STORED IN A TO LCD
	setb p3.5	;RS=1 BEFORE SENDING DATA
	clr p3.6		;R/W=0 TO WRITE
	setb p3.7	;SEND A HIGH TO LOW SIGNAL TO ENABLE PIN
	acall DELAY
	clr p3.7
	ret

DELAY:
	push 0
	push 1
	mov r0,#50
DELAY_OUTER_LOOP:
	mov r1,#20
	djnz r1,$
	djnz r0,DELAY_OUTER_LOOP
	pop 1
	pop 0
	ret


KEYBOARD: ;takes the key pressed from the keyboard and puts it to A
	mov	P0, #0ffh	;makes P0 input

K1:
	mov	P2, #0	;ground all rows
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, K1
K2:
	acall	DELAY
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, KB_OVER
	sjmp	K2
KB_OVER:
	acall DELAY
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, KB_OVER1
	sjmp	K2
KB_OVER1:
	mov	P2, #11111110B
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, ROW_0
	mov	P2, #11111101B
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, ROW_1
	mov	P2, #11111011B
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, ROW_2
	mov	P2, #11110111B
	mov	A, P0
	anl	A, #00001111B
	cjne	A, #00001111B, ROW_3
	ljmp	K2
	
ROW_0:
	mov	DPTR, #KCODE0
	sjmp	KB_FIND
ROW_1:
	mov	DPTR, #KCODE1
	sjmp	KB_FIND
ROW_2:
	mov	DPTR, #KCODE2
	sjmp	KB_FIND
ROW_3:
	mov	DPTR, #KCODE3
KB_FIND:
	rrc	A
	jnc	KB_MATCH
	inc	DPTR
	sjmp	KB_FIND
KB_MATCH:
	clr	A
	movc	A, @A+DPTR; get ASCII code from the table 
	ret

;ASCII look-up table 
KCODE0:	DB	'1', '2', '3', 'A'
KCODE1:	DB	'4', '5', '6', 'B'
KCODE2:	DB	'7', '8', '9', 'C'
KCODE3:	DB	'*', '0', '#', 'D'

digits: db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'