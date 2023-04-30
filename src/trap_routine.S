.global trapRoutine
.extern _Z25cInternalInterruptRoutinev
.extern _Z25cExternalInterruptRoutinev
.type _Z25cInternalInterruptRoutinev, @function
.type _Z25cExternalInterruptRoutinev, @function

.align 4
trapRoutine:
    jump internalInterruptRoutine, x0   # 0
    jump externalInterruptRoutine, x0   # 1
    nop                                 # 2
    nop                                 # 3
    nop                                 # 4
    nop                                 # 5
    nop                                 # 6
    nop                                 # 7
    nop                                 # 8
    jump externalInterruptRoutine, x0   # 9


internalInterruptRoutine:
    addi sp, sp, -256 # -256 = 32 * 8, to store all the registers

    # intentionaly missing x10
    .irp index 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _Z25cInternalInterruptRoutinev

    # intentionaly missing x10
    .irp index 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 256

    sret


externalInterruptRoutine:
    addi sp, sp, -256 # -256 = 32 * 8, to store all the registers

    .irp index 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    call _Z25cExternalInterruptRoutinev

    .irp index 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 256

    sret