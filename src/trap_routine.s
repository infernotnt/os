# 1 "src/trap_routine.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/riscv64-linux-gnu/include/stdc-predef.h" 1 3
# 32 "<command-line>" 2
# 1 "src/trap_routine.S"
.global trapRoutine

.extern _Z25cInternalInterruptRoutinev
.extern _Z25cExternalInterruptRoutinev
.extern _Z15__exeptionErrorv
.extern _ZN7IThread10pRunningSpE

#.extern _Z24cConsoleInterruptRoutinev


.align 4
trapRoutine:
    j internalInterruptRoutine # 0
    j externalInterruptRoutine # 1
    j _Z15__exeptionErrorv # 2
    j _Z15__exeptionErrorv # 3
    j _Z15__exeptionErrorv # 4
    j _Z15__exeptionErrorv # 5
    j _Z15__exeptionErrorv # 6
    j _Z15__exeptionErrorv # 7
    j _Z15__exeptionErrorv # 8
    j externalInterruptRoutine # 9
    j _Z15__exeptionErrorv # 10
    j _Z15__exeptionErrorv # 11
    j _Z15__exeptionErrorv # 12


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


externalInterruptRoutine:

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

    call _Z25cExternalInterruptRoutinev

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
