//----------------------------------------------------------------------
// bigintaddopt.s
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
        // Return the larger of lLength1 and lLength2
        // long BigInt_larger(long lLength1, long lLength2)
        //--------------------------------------------------------------

        // Must be a multiple of 16
        .equ    LARGER_STACK_BYTECOUNT, 32

        // Local variable registers:
        LLARGER         .req x21

        // Paremeter registers:
        LLENGTH2        .req x20
        LLENGTH1        .req x19

BigInt_larger:
        // Prolog
        sub     sp, sp, LARGER_STACK_BYTECOUNT
        str     x30, [sp]
        str     x19, [sp, 8]
        str     x20, [sp, 16]
        str     x21, [sp, 24]

        // Store parameters in registers
        mov     LLENGTH1, x0
        mov     LLENGTH2, x1

        // long lLarger

        // if (iLength1 <= iLength2) goto else1
        cmp     x0, x1
        ble     else1

        // lLarger = lLength1
        mov     LLARGER, LLENGTH1

        // goto endif1
        b       endif1

else1:
        // lLarger = lLength2
        mov     LLARGER, LLENGTH2
        
endif1:
        // Epilog and return
        mov     x0, LLARGER
        ldr     x30, [sp]
        ldr     x19, [sp, 8]
        ldr     x20, [sp, 16]
        ldr     x21, [sp, 24]
        add     sp, sp, LARGER_STACK_BYTECOUNT
        ret

        .size   BigInt_larger, (. - BigInt_larger)

        //--------------------------------------------------------------
        // Assign the sum of oAddend1 and oAddend2 to oSum. oSum should 
        // be distinct from oAddend1 and oAddend2. Return 0 (FALSE) if 
        // an overflow occurred, and 1 (TRUE) otherwise.
        // int BigInt_add(BigInt_T oAddend1, BigInt_T oAddend2, BigInt_T oSum)
        //--------------------------------------------------------------

        // Must be a multiple of 16
        .equ    ADD_STACK_BYTECOUNT, 64

        // Local variable registers:
        LSUMLENGTH      .req x25
        LINDEX          .req x24
        ULSUM           .req x23
        ULCARRY         .req x22

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

        // Store parameters in registers
        mov     OADDEND1, x0
        mov     OADDEND2, x1
        mov     OSUM, x2

        // unsigned long ulCarry
        // unsigned long ulSum
        // long lIndex
        // long lSumLength

        // lSumLength = BigInt_larger(oAddend1->lLength, oAddend2->lLength)
        ldr     x0, [OADDEND1]
        ldr     x1, [OADDEND2]
        bl      BigInt_larger
        mov     LSUMLENGTH, x0

        // if (oSum->lLength <= lSumLength) goto endif2
        ldr     x0, [OSUM]
        cmp     x0, LSUMLENGTH
        ble     endif2

        // memset(oSum->aulDigits, 0, MAX_DIGITS * sizeof(unsigned long))
        add     x0, OSUM, 8
        mov     x1, 0
        mov     x2, MAX_DIGITS * 8
        bl      memset

endif2:
        // ulCarry = 0
        mov     ULCARRY, 0

        // lIndex = 0
        mov     LINDEX, 0

loop1:
        // if (lIndex >= lSumLength) goto endloop1
        cmp     LINDEX, LSUMLENGTH
        bge     endloop1

        // ulSum = ulCarry
        mov     ULSUM, ULCARRY

        // ulCarry = 0
        mov     ULCARRY, 0

        // ulSum += oAddend1->aulDigits[lIndex]
        add     x0, OADDEND1, 8
        ldr     x0, [x0, LINDEX, lsl 3]

        add     ULSUM, ULSUM, x0

        // if (ulSum >= oAddend1->aulDigits[lIndex]) goto endif3
        cmp     ULSUM, x0
        bhs     endif3

        // ulCarry = 1
        mov     ULCARRY, 1

endif3:
        // ulSum += oAddend2->aulDigits[lIndex]
        add     x0, OADDEND2, 8
        ldr     x0, [x0, LINDEX, lsl 3]

        add     ULSUM, ULSUM, x0

        // if (ulSum >= oAddend2->aulDigits[lIndex]) goto endif4
        cmp     ULSUM, x0
        bhs     endif4

        // ulCarry = 1
        mov     ULCARRY, 1

endif4:
        // oSum->aulDigits[lIndex] = ulSum
        add     x0, OSUM, 8

        str     ULSUM, [x0, LINDEX, lsl 3]

        // lIndex++
        add     LINDEX, LINDEX, 1

        // goto loop1
        b       loop1

endloop1:
        // if (ulCarry != 1) goto endif5
        cmp     ULCARRY, 1
        bne     endif5

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
        add     sp, sp, ADD_STACK_BYTECOUNT
        ret

endif6:
        // oSum->aulDigits[lSumLength] = 1
        add     x0, OSUM, 8

        mov     x2, 1
        
        str     x2, [x0, LSUMLENGTH, lsl 3]

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
        add     sp, sp, ADD_STACK_BYTECOUNT
        ret

        .size   BigInt_add, (. - BigInt_add)
