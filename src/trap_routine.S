.global trapRoutine
.extern _Z12cTrapRoutinev
.align 4
trapRoutine:
    addi sp, sp, -256 # -256 = 32 * 8, to store all the registers

    sd x0, 0x00(sp)
    sd x1, 0x08(sp)
    sd x2, 0x16(sp)
    sd x3, 0x20(sp)

    call _Z12cTrapRoutinev

    ld x0, 0x00(sp)
    ld x1, 0x08(sp)
    ld x2, 0x16(sp)
    ld x3, 0x20(sp)

    addi sp, sp, 256

    sret