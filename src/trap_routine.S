.global trapRoutine

.extern _Z25cInternalInterruptRoutinev
.extern _Z22cTimerInterruptRoutinev
.extern _Z24cConsoleInterruptRoutinev

.extern _ZN7IThread10pRunningSpE

.align 4
trapRoutine:
    j internalInterruptRoutine  # 0
    j timerInterruptRoutine     # 1
    nop                         # 2
    nop                         # 3
    nop                         # 4
    nop                         # 5
    nop                         # 6
    nop                         # 7
    UNIMP                       # 8
    j timerInterruptRoutine     # 9
    UNIMP

    #j consoleInterruptRoutine   # 9
    #j timerInterruptRoutine     # 1

externalGay:
    csrc sip, 0xA
    csrc sstatus, 0x02
    sret

internalInterruptRoutine:

    addi sp, sp, -34*8 # 34 because we need 33 but 33 is not divisible by 16

    # intentionaly missing x0, x2
    .irp index 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    # store sepc+4 on stack
    csrr x1, sepc
    addi x1, x1, 4
    sd x1, 32*8(sp)

    # store sp in PCB
    ld x1, _ZN7IThread10pRunningSpE
    sd sp, 0(x1)

    call _Z25cInternalInterruptRoutinev

    # restore sp from PCB
    ld x1, _ZN7IThread10pRunningSpE
    ld sp, 0(x1)

    # restore sepc from stack
    ld x1, 32*8(sp)
    csrw sepc, x1

    # intentionaly missing x0, x2
    .irp index 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 34*8

    sret


timerInterruptRoutine: # WARNING: should be the same as consoleInterruptRoutine, except the "call"

    addi sp, sp, -34*8 # 34 because we need 33 but 33 is not divisible by 16

    # intentionaly missing x0, x2
    .irp index 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(sp)
    .endr

    # store sp in PCB
    ld x1, _ZN7IThread10pRunningSpE
    sd sp, 0(x1)

    # store sepc on stack
    csrr x1, sepc
    sd x1, 32*8(sp)

    call _Z22cTimerInterruptRoutinev

    # restore sp from PCB
    ld x1, _ZN7IThread10pRunningSpE
    ld sp, 0(x1)

    # restore sepc from stack
    ld x1, 32*8(sp)
    csrw sepc, x1

    # intentionaly missing x0, x2
    .irp index 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(sp)
    .endr

    addi sp, sp, 34*8

    sret