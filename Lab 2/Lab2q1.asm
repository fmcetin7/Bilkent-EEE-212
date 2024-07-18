
timer:
	clr p2.6
	clr p2.7
	clr tr0
	clr tr1
	mov p1, #0FFh ;make p1 input port
	mov tmod, #11h ;both timers in 16-bit timer mode / mode 1
	jnb p1.0, second_case
	sjmp first_case

first_case: ;switch is 0
	clr p2.6
	clr p2.7
	clr tf0
	clr tf1
	mov th0, #0fbh 
	mov tl0, #0f4h
	setb tr0
	
	jnb tf0, $
	clr tf0
	setb p2.7
	mov th0, #0fbh 
	mov tl0, #0f4h

	jnb tf0, $
	clr tf0
	setb p2.6
	clr p2.7
	mov th0, #0fbh 
	mov tl0, #0f4h

	jnb tf0, $
	clr tf0
	setb p2.7
	mov th0, #0fbh 
	mov tl0, #0f4h

	jnb tf0, $
	
	jnb p1.0, second_case
	sjmp first_case

second_case: ;switch is 1
	clr p2.6
	clr p2.7
	clr tf0
	clr tf1
	mov th1, #0fdh 
	mov tl1, #0fah
	setb tr1
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	setb p2.6
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	clr p2.6
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	setb p2.7
	
	jnb tf1, $
	clr tf1
	mov th1, #0fdh  
	mov tl1, #0fah
	setb p2.6
	
	jnb tf1, $
	
	jnb p1.0, second_case
	ljmp first_case
	