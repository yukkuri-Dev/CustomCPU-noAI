Start:
    ; Register-based IN/OUT:
    ;   IN  op1,op2,op3  => mode=R[op1], mount=R[op2], data=R[op3]
    ;   OUT op1,op2,op3  => mode=R[op1], mount=R[op2], data=R[op3]
    ; fakevgq packed write:
    ;   data = (addr << 4) | (color & 0xF)

    ; NOTE: CPU_GPR[0..3] are overwritten by fetch each instruction.
    ; Use R5+ to keep values across instructions.

    MOVI R5,0xFFFF    ; MODE = INIT
    MOVI R6,0x0000
    MOVI R7,0x0000
    IN R5,R6,R7

    MOVI R5,0x0000    ; MODE = WRITE
    MOVI R6,0x0200    ; DEVICE = fakevgq

    ; First 16 cells: addr=i, color=i => data = (i<<4)|i
    MOVI R7,0x0000
    OUT R5,R6,R7
    MOVI R7,0x0011
    OUT R5,R6,R7
    MOVI R7,0x0022
    OUT R5,R6,R7
    MOVI R7,0x0033
    OUT R5,R6,R7
    MOVI R7,0x0044
    OUT R5,R6,R7
    MOVI R7,0x0055
    OUT R5,R6,R7
    MOVI R7,0x0066
    OUT R5,R6,R7
    MOVI R7,0x0077
    OUT R5,R6,R7
    MOVI R7,0x0088
    OUT R5,R6,R7
    MOVI R7,0x0099
    OUT R5,R6,R7
    MOVI R7,0x00AA
    OUT R5,R6,R7
    MOVI R7,0x00BB
    OUT R5,R6,R7
    MOVI R7,0x00CC
    OUT R5,R6,R7
    MOVI R7,0x00DD
    OUT R5,R6,R7
    MOVI R7,0x00EE
    OUT R5,R6,R7
    MOVI R7,0x00FF
    OUT R5,R6,R7

Loop:
    JMP Loop,0x00000000
