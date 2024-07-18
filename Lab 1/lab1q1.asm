ORG 0000H

    MOV R0, #23H

    MOV A, R0
    MOV B, #10
    DIV AB

    MOV R2, B
    MOV B, #10
    DIV AB

    MOV R3, B
    ADD A, R2
    ADD A, R3
    MOV R1, A

    MOV R2, #0H
    MOV R3, #0H
   

END