/*--------------------------------------------------------------------*/
/* stra.c                                                             */
/* Author: Seungho Lee                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#include "str.h"


size_t Str_getLength(const char pcSrc[]) {
    size_t uLength = 0;
    assert(pcSrc != NULL);
    while (pcSrc[uLength] != '\0')
        uLength++;
    return uLength;
}

char *Str_copy(char pcDest[], const char pcSrc[]) {
    size_t uLength = 0;
    assert(pcSrc != NULL);  
    assert(pcDest != NULL);  
    while (pcSrc[uLength] != '\0') 
    {
        pcDest[uLength] = pcSrc[uLength];
        uLength++;
    }
    pcDest[uLength] = '\0';
    return pcDest;
}


char *Str_concat(char pcDest[], const char pcSrc[]) {
    size_t size_Dest = Str_getLength(pcDest);
    size_t uLength = 0;
    assert(pcSrc != NULL);
    assert(pcDest != NULL);
    while (pcSrc[uLength] != '\0') {
        pcDest[uLength + size_Dest] = pcSrc[uLength];
        uLength++;
    }
    pcDest[uLength + size_Dest] = '\0';
    return pcDest;
}

int Str_compare(const char pcStr1[], const char pcStr2[]) {
    size_t uLength = 0;
    assert(pcStr1 != NULL);
    assert(pcStr2 != NULL);
    while (pcStr1[uLength] != '\0' && pcStr2[uLength] != '\0') {
        if (pcStr1[uLength] != pcStr2[uLength]) {
            return (int) (pcStr1[uLength] - pcStr2[uLength]);
        }
        uLength++;
    }
    return (int) (pcStr1[uLength] - pcStr2[uLength]);
}



char *Str_search(const char pcHaystack[], const char pcNeedle[]) {
    size_t i, j;
    size_t size_Haystack = Str_getLength(pcHaystack);
    size_t size_Needle = Str_getLength(pcNeedle);
    assert(pcHaystack != NULL);
    assert(pcNeedle != NULL);
    if (pcNeedle[0] == '\0') {
        return (char*) pcHaystack;
    }
    for (i = 0; i <  size_Haystack; i++) {
        for (j = 0; j < size_Needle + 1; j++) {
            if (pcNeedle[j] == '\0') {
                return (char*) &pcHaystack[i];
            }
            if (pcHaystack[i+j] != pcNeedle[j]) {
                break;
            }
        }
        
    }
    return NULL;
}