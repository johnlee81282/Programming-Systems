#include <stdio.h>
#include <ctype.h>

enum
{
    FALSE,
    TRUE
};

static long lLineCount = 0;
static long lWordCount = 0;
static long lCharCount = 0;
static int iChar;
static int iInWord = FALSE;

int main(void)
{
loop1:
    /* inside while loop */
    if ((iChar = getchar()) == EOF)
        goto endloop1;
    lCharCount++;
    /* if controls */
    if (!isspace(iChar))
        goto else1;
    if (!iInWord)
        goto endif1;
    lWordCount++;
    iInWord = FALSE;
endif1:
    goto endif2;

else1:
    if (iInWord)
        goto endif2;
    iInWord = TRUE;
endif2:

    if (iChar != '\n')
        goto endif3;
    lLineCount++;
endif3:

    goto loop1;
endloop1:

    if (!iInWord)
        goto endif4;
    lWordCount++;
endif4:

    printf("%7ld %7ld %7ld\n", lLineCount, lWordCount, lCharCount);
    return 0;
}
