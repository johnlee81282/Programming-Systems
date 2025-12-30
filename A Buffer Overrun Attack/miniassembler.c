/*--------------------------------------------------------------------*/
/* miniassembler.c                                                    */
/* Author: Bob Dondero, Donna Gabai                                   */
/*--------------------------------------------------------------------*/

#include "miniassembler.h"
#include <assert.h>
#include <stddef.h>

/*--------------------------------------------------------------------*/
/* Modify *puiDest in place,
   setting uiNumBits starting at uiDestStartBit (where 0 indicates
   the least significant bit) with bits taken from uiSrc,
   starting at uiSrcStartBit.
   uiSrcStartBit indicates the rightmost bit in the field.
   setField sets the appropriate bits in *puiDest to 1.
   setField never unsets any bits in *puiDest.                        */
static void setField(unsigned int uiSrc, unsigned int uiSrcStartBit,
                     unsigned int *puiDest, unsigned int uiDestStartBit,
                     unsigned int uiNumBits)
{
   /* Your code here */
   unsigned int clear = 0xffffffff;
   const unsigned int bytesInt = 32;
   assert(puiDest != NULL);
   /* discard the values not used and shift uiSrc to the rightmost */
   uiSrc = uiSrc >> uiSrcStartBit;
   uiSrc = uiSrc << (unsigned int)(bytesInt - uiNumBits);
   uiSrc = uiSrc >> (unsigned int)(bytesInt - uiNumBits);
   /* Clear puiDest */
   clear = clear >> uiDestStartBit;
   clear = clear << (unsigned int)(bytesInt - uiNumBits);
   clear = clear >> (unsigned int)(bytesInt - uiNumBits);
   clear = clear << uiDestStartBit;
   *puiDest = *puiDest & (0xffffffff ^ clear);
   /* Merge uiSrc and puiDest */
   *puiDest = *puiDest | (uiSrc << uiDestStartBit);
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_mov(unsigned int uiReg, int iImmed)
{
   /* Your code here */
   unsigned int uiInstr;

   /* Base instruction code */
   uiInstr = 0x52800000;

   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);

   /* immediate value to be inserted */
   setField((unsigned int)iImmed, 0, &uiInstr, 5, 16);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_adr(unsigned int uiReg, unsigned long ulAddr,
                               unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   unsigned int uiDisp;

   /* Base Instruction Code */
   uiInstr = 0x10000000;

   /* register to be inserted in instruction */
   setField(uiReg, 0, &uiInstr, 0, 5);

   /* displacement to be split into immlo and immhi and inserted */
   uiDisp = (unsigned int)(ulAddr - ulAddrOfThisInstr);

   setField(uiDisp, 0, &uiInstr, 29, 2);
   setField(uiDisp, 2, &uiInstr, 5, 19);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_strb(unsigned int uiFromReg,
                                unsigned int uiToReg)
{
   /* Your code here */
   unsigned int uiInstr;

   /* Base instruction code */
   uiInstr = 0x39000000;

   /* Source register (really destination) inserted */
   setField(uiToReg, 0, &uiInstr, 5, 5);

   /* Destination register (really source) inserted */
   setField(uiFromReg, 0, &uiInstr, 0, 5);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_b(unsigned long ulAddr,
                             unsigned long ulAddrOfThisInstr)
{
   /* Your code here */
   unsigned int uiInstr;
   int ulDisp;

   /* Base instruction code */
   uiInstr = 0x14000000;

   /* displacement to be inserted */
   ulDisp = (int)ulAddr - (int)ulAddrOfThisInstr;

   setField((unsigned int)ulDisp, 2, &uiInstr, 0, 26);

   return uiInstr;
}

/*--------------------------------------------------------------------*/

unsigned int MiniAssembler_bl(unsigned long ulAddr,
                              unsigned long ulAddrOfThisInstr)
{
   unsigned int uiInstr;
   int ulDisp;

   /* Base instruction code */
   uiInstr = 0x94000000;

   /* displacement to be inserted */
   ulDisp = (int)ulAddr - (int)ulAddrOfThisInstr;

   setField((unsigned int)ulDisp, 2, &uiInstr, 0, 26);

   return uiInstr;
}
