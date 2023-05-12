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
.type _Z25cInternalInterruptRoutinev, @function
.type _Z25cExternalInterruptRoutinev, @function
.extern _ZN6Thread15pRunningContextE

.align 4
trapRoutine:
    jump newAsmTrap, x0 # 0
    jump externalInterruptRoutine, x0 # 1
    nop # 2
    nop # 3
    nop # 4
    nop # 5
    nop # 6
    nop # 7
    nop # 8
    jump externalInterruptRoutine, x0 # 9

newAsmTrap:

    csrw sscratch, x1
    ld x1, _ZN6Thread15pRunningContextE

    # intentionaly missing x10, x0, x1
    .irp index 2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    sd x\index, \index * 8(x1)
    .endr

    # saves sepc+4 in the Thread:pRunning->context[32]
    csrr x2, sepc
    addi x2, x2, 4
    sd x2, 32*8(x1)

    # saves x1 in the Thread::pRunning->context[1]
    mv x2, x1
    csrr x1, sscratch
    sd x1, 1*8(x2)

    call _Z25cInternalInterruptRoutinev

    # x1 = Thread::pRunning->context
    ld x1, _ZN6Thread15pRunningContextE

    # x2 "=" thread->sepc, sepc = x2
    ld x2, 32*8(x1)
    csrw sepc, x2

    # intentionaly missing x10
    .irp index 2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
    ld x\index, \index * 8(x1)
    .endr

    # restore x1
    ld x1, 1*8(x1)

    sret


#internalInterruptRoutine:
# addi sp, sp, -256 # -256 = 32 * 8, to store all the registers

# # intentionaly missing x10
# .irp index 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
# sd x\index, \index * 8(sp)
# .endr

# call _Z25cInternalInterruptRoutinev

# # intentionaly missing x10
# .irp index 1,2,3,4,5,6,7,8,9,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31
# ld x\index, \index * 8(sp)
# .endr

# addi sp, sp, 256

# sret


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
