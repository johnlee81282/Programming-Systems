/*--------------------------------------------------------------------*/
/* strp.c                                                             */
/* Author: Seungho Lee                                                */
/*--------------------------------------------------------------------*/

#include <stdio.h>
#include <assert.h>

#include "str.h"

size_t Str_getLength(const char *pcSrc) {
   const char *pcEnd;
   assert(pcSrc != NULL);
   pcEnd = pcSrc;
   while (*pcEnd != '\0')
      pcEnd++;
   return (size_t)(pcEnd - pcSrc);
}

char *Str_copy(char *pcDest, const char *pcSrc) {
   char *pcEnd;
   assert(pcDest != NULL);
   assert(pcSrc != NULL);
   pcEnd = pcDest;
   while (*pcSrc != '\0') {
      *pcEnd = *pcSrc;
      pcEnd++;
      pcSrc++;
   }
   *pcEnd = '\0';
   return pcDest;
}

char *Str_concat(char *pcDest, const char *pcSrc) {
   char *pcEnd;
   assert(pcDest != NULL);
   assert(pcSrc != NULL);
   pcEnd = pcDest;
   while (*pcEnd != '\0') {
      pcEnd++;
   }
   while (*pcSrc != '\0') {
      *pcEnd = *pcSrc;
      pcEnd++;
      pcSrc++;
   }
   *pcEnd = '\0';
   
   return pcDest;
}

int Str_compare(const char *pcStr1, const char *pcStr2) {
   assert(pcStr1 != NULL);
   assert(pcStr2 != NULL);
   while (*pcStr1 != '\0' && *pcStr2 != '\0') {
      if (*pcStr1 != *pcStr2) {
         return (int)(*pcStr1 - *pcStr2);
      }
      pcStr1++;
      pcStr2++;
   }
   return (int)(*pcStr1 - *pcStr2);
}

char *Str_search(const char *pcHaystack, const char *pcNeedle) {
   const char *pcEnd;
   const char *pcEnd2;
   assert(pcHaystack != NULL);
   assert(pcNeedle != NULL);
   if (*pcNeedle == '\0') {
      return (char*)(pcHaystack);
   }
   while (*pcHaystack != '\0') {
      pcEnd = pcNeedle;
      pcEnd2 = pcHaystack;
      while (*pcEnd != '\0') {
         if (*pcEnd != *pcEnd2) {
            break;
         }
         pcEnd++;
         pcEnd2++;
      }
      if (*pcEnd == '\0') {
            return (char*)(pcHaystack);
         }
      pcHaystack++;
   }
   
   return NULL;
}