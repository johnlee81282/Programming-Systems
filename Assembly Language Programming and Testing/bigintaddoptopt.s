//----------------------------------------------------------------------
// bigintaddoptopt.s
//----------------------------------------------------------------------

.equ    TRUE, 1
.equ    FALSE, 0
.equ    MAX_DIGITS, 32768

//----------------------------------------------------------------------
        .section .rodata

//----------------------------------------------------------------------
        .section .data

//----------------------------------------------------------------------
        .section .bss

//----------------------------------------------------------------------
        .section .text

        //--------------------------------------------------------------
        // Assign the sum of oAddend1 and oAddend2 to oSum. oSum should 
        // be distinct from oAddend1 and oAddend2. Return 0 (FALSE) if 
        // an overflow occurred, and 1 (TRUE) otherwise.
        // int BigInt_add(BigInt_T oAddend1, BigInt_T oAddend2, BigInt_T oSum)
        //--------------------------------------------------------------

        // Must be a multiple of 16
        .equ    ADD_STACK_BYTECOUNT, 80

        // Local variable registers:
        DIGITSSUM       .req x27
        DIGITS2         .req x26
        DIGITS1         .req x25
        LSUMLENGTH      .req x24
        LINDEX          .req x23
        ULSUM           .req x22

        // Paremeter registers:
        OSUM            .req x21
        OADDEND1        .req x20
        OADDEND2        .req x19

        .global BigInt_add

BigInt_add:
        // Prolog
        sub     sp, sp, ADD_STACK_BYTECOUNT
        str     x30, [sp]
        str     x19, [sp, 8]
        str     x20, [sp, 16]
        str     x21, [sp, 24]
        str     x22, [sp, 32]
        str     x23, [sp, 40]
        str     x24, [sp, 48]
        str     x25, [sp, 56]
        str     x26, [sp, 64]
        str     x27, [sp, 72]

        // Store parameters in registers
        mov     OADDEND1, x0
        mov     OADDEND2, x1
        mov     OSUM, x2

        // digits1 = oAddend1->aulDigits
        // digits2 = oAddend2->aulDigits
        // digitssum = oSum->aulDigits
        mov     DIGITS1, OADDEND1
        mov     DIGITS2, OADDEND2
        mov     DIGITSSUM, OSUM

        // unsigned long ulCarry
        // unsigned long ulSum
        // long lIndex
        // long lSumLength

        ldr     x0, [OADDEND1]
        ldr     x1, [OADDEND2]
        ldr     x2, [OSUM]

        // if (oAddend1->lLength < oAddend2->lLength) goto else1
        cmp     x0, x1
        blt     else1

        // lSumLength = oAddend1->lLength
        mov     LSUMLENGTH, x0

        // goto endif1
        b       endif1

else1:
        // lSumLength = oAddend2->lLength
        mov     LSUMLENGTH, x1

endif1:
        // if (oSum->lLength <= lSumLength) goto endif2
        cmp     x2, LSUMLENGTH
        ble     endif2

        // memset(oSum->aulDigits, 0, MAX_DIGITS * sizeof(unsigned long))
        mov     x0, DIGITSSUM
        mov     x1, 0
        mov     x2, MAX_DIGITS * 8
        bl      memset

endif2:
        // set the carry flag to zero
        adds    xzr, xzr, xzr

        // if (lSumLength == 0) goto endloop1
        cbz     LSUMLENGTH, endloop1

        // lIndex = LSUMLENGTH
        mov     LINDEX, LSUMLENGTH

loop1:  
        // DIGITS1++, DIGITS2++, DIGITSSUM++
        add     DIGITS1, DIGITS1, 8
        add     DIGITS2, DIGITS2, 8
        add     DIGITSSUM, DIGITSSUM, 8

        // ulSum = 0
        mov     ULSUM, 0

        // ulSum += oAddend1->aulDigits[lIndex]
        // ulSum += oAddend2->aulDigits[lIndex]
        // use adcs to keep track of the carry over
        ldr     x0, [DIGITS1]
        ldr     x1, [DIGITS2]

        adcs    ULSUM, ULSUM, x0
        blo     endif3

        add     ULSUM, ULSUM, x1
        b       endif4

endif3:
        adcs    ULSUM, ULSUM, x1

endif4:
        // oSum->aulDigits[lIndex] = ulSum
        str     ULSUM, [DIGITSSUM]

skip:
        // lIndex--
        sub     LINDEX, LINDEX, 1

        // if (lIndex > 0) goto loop1
        cbnz    LINDEX, loop1

endloop1:
        // if there is no carry, goto endif5
        blo     endif5

        // if (lSumLength != MAX_DIGITS) goto endif6
        cmp     LSUMLENGTH, MAX_DIGITS
        bne     endif6

        // Epilog and return FALSE
        mov     w0, FALSE
        ldr     x30, [sp]
        ldr     x19, [sp, 8]
        ldr     x20, [sp, 16]
        ldr     x21, [sp, 24]
        ldr     x22, [sp, 32]
        ldr     x23, [sp, 40]
        ldr     x24, [sp, 48]
        ldr     x25, [sp, 56]
        ldr     x26, [sp, 64]
        ldr     x27, [sp, 72]
        add     sp, sp, ADD_STACK_BYTECOUNT
        ret

endif6:
        // oSum->aulDigits[lSumLength] = 1
        add     DIGITSSUM, DIGITSSUM, 8
        mov     x0, 1
        str     x0, [DIGITSSUM]

        // lSumLength++
        add     LSUMLENGTH, LSUMLENGTH, 1

endif5:
        // oSum->lLength = lSumLength
        str     LSUMLENGTH, [OSUM]

        // Epilog and return TRUE
        mov     w0, TRUE
        ldr     x30, [sp]
        ldr     x19, [sp, 8]
        ldr     x20, [sp, 16]
        ldr     x21, [sp, 24]
        ldr     x22, [sp, 32]
        ldr     x23, [sp, 40]
        ldr     x24, [sp, 48]
        ldr     x25, [sp, 56]
        ldr     x26, [sp, 64]
        ldr     x27, [sp, 72]
        add     sp, sp, ADD_STACK_BYTECOUNT
        ret

        .size   BigInt_add, (. - BigInt_add)
