start:
	jnb 31h, initialize
	acall keyboard

initialize:
	setb 31h; for initialization purposes
	mov r0, #4h
	acall configure_lcd

KEYBOARD_LOOP:
	acall KEYBOARD
	acall send_data
	mov @r0, A; input values will be stored in r4, r3, r2 depending on how many strings there are
	dec r0
	cjne A, #41h, keyboard_loop; 41h is ASCII representation of A
	mov a, r4
	subb a, #30h
	mov r4, a
	mov a, r3
	subb a, #30h
	mov r3, a
	mov a, r2
	subb a, #30h
	mov r2, a


digit_loop_1:
	cjne r3, #11h, digit_loop_2; check if input is 1 digit
	sjmp one_decimal
digit_loop_2:
	cjne r2, #11h, digit_loop_3; check if input is 2 digit
	sjmp two_decimal
digit_loop_3:
	cjne r1, #41h, digit_loop_1; check if input is 3 digit
	sjmp number_256_check


one_decimal:; logic for 1 digit inputs
	mov a, r4
	mov r6, a
	mov r5, #0h
	sjmp ceren

two_decimal:; logic for 2 digit inputs
	mov a, r4
	mov b, #10d
	mul ab
	add a, r3
	mov r6, a
	sjmp ceren


number_256_check:; logic for 3 digit inputs
	cjne r4, #2h, msd_reg_comparison
	cjne r3, #5h, reg_comparison
	cjne r2, #6h, reg_comparison
	mov r5, #01h
	mov r6, #0h
	sjmp ceren

msd_reg_comparison:
	jc below_256
	mov r5, #01h
	mov a, r3
	mov b, #10d
	mul ab
	add a, r2
	add a, #44d
	mov r6, a
	sjmp ceren

reg_comparison:
	jc below_256
	jnc between_257_299

below_256:
	mov a, r3
	mov b, #10d
	mul ab
	mov r3, a
	mov a, r4
	mov b, #100d
	mul ab
	mov r4, a
	add a, r3
	add a, r2
	mov r6, a
	sjmp ceren

between_257_299:
	mov r5, #01h
	mov a, r3
	mov b, #10d
	mul ab
	add a, r2
	subb a, #56d
	mov r6, a
	sjmp ceren

ceren: ;day number finalised
	mov a, r5
	mov r3, a
	mov a, r6
	mov r2, a
	sjmp day_algorithm_start


day_algorithm_start:
	mov dptr, #month_days
	mov r7, #0h
	sjmp subtract_day_loop

subtract_day_loop:
	clr c
	clr a
	movc a, @a+dptr
	mov b, a
	mov a, r6
	subb a, b
	jc carried; if b>a
	jz carried; if a=0
	sjmp next_month_logic

next_month_logic:
	mov r6, a
	inc dptr
	inc r7
	sjmp subtract_day_loop

double_byte_case:
	mov a, r4
	add a, b
	mov r6, a
	dec b
	mov a, #0ffh
	clr c
	subb a, b
	add a, r6
	mov r5, #0h
	sjmp next_month_logic

carried:
	mov r4, a
	mov a, r5
	jnz double_byte_case
	mov a, r4
	add a, b
	mov r6, a
	sjmp weekday_find


weekday_find:
	mov a, r2
	mov b, #7h
	div ab
	mov a, r3
	jnz two_byte_case
	sjmp one_byte_case

one_byte_case:
	mov r4, b
	sjmp month_print

two_byte_case:
	mov a, b
	add a, #4h; monday(001) to friday(257)
	mov b, #7h
	div ab
	mov r4, b
	sjmp month_print

month_print:
	mov a, #0c0h
	acall send_command; next line command
	mov a, r7
	mov b, #3h
	mul ab
	mov r7, a
	mov dptr, #month_names
	movc a, @a+dptr
	acall send_data
	mov a, r7
	inc a
	movc a, @a+dptr
	acall send_data
	mov a, r7
	inc a
	inc a
	movc a, @a+dptr
	acall send_data
	mov a, #20h
	acall send_data
	sjmp number_print

number_print:
	mov a, r6
	mov b, #10d
	div ab
	mov dptr, #digits
	movc a, @a+dptr
	acall send_data
	mov a, b
	movc a, @a+dptr
	acall send_data
	mov a, #20h
	acall send_data
	sjmp week_print

week_print:
	mov a, r4
	mov b, #3h
	mul ab
	mov r4, a
	mov dptr, #week_names
	movc a, @a+dptr
	acall send_data
	mov a, r4
	inc a
	movc a, @a+dptr
	acall send_data
	mov a, r4
	inc a
	inc a
	movc a, @a+dptr
	acall send_data
	mov r7, #0h
	ajmp start

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
	mov a, #49h
	acall send_data
	mov a, #4eh
	acall send_data
	mov a, #50h
	acall send_data
	mov a, #55h
	acall send_data
	mov a, #54h
	acall send_data
	mov a, #3dh
	acall send_data
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
	mov r1,#255
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


month_days: db 1fh, 1dh, 1fh, 1eh, 1fh, 1eh, 1fh, 1fh, 1eh, 1fh, 1eh, 1fh
week_names: db 'S','U','N','M','O','N', 'T','U','E', 'W','E','D', 'T','H','U', 'F','R','I', 'S','A','T'
month_names: db 'J','A','N', 'F','E','B', 'M','A','R', 'A','P','R', 'M','A','Y', 'J','U','N', 'J','U','L', 'A','U','G', 'S','E','P', 'O','C','T', 'N','O','V', 'D','E','C'
digits: db '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'

end


