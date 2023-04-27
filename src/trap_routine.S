.global trapRoutine
.extern cTrapRoutine
.align 4
trapRoutine:
    #addi sp, sp, -256 # -256 = 32 * 8
    #sd r0, sp(0) # jel double dovoljan?

    call cTrapRoutine

    ret # koji ret?