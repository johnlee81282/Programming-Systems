#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    const int mod = 0x7E;
    const int lower = 0x20;
    const int upper = 0x7E;
    int count = 0;
    int random;

    while (count < 10000)
    {
        random = rand();
        random %= mod;
        if (random == 0x09 || random == 0x0A)
        {
            printf("%d", random);
            count++;
        }
        else if (random >= lower && random <= upper)
        {
            printf("%d", random);
            count++;
        }
    }

    return 0;
}
