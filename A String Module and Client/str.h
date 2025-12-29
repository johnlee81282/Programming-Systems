/*--------------------------------------------------------------------*/
/* str.h                                                              */
/* Author: Seungho Lee                                                */
/*--------------------------------------------------------------------*/

#ifndef STR_INCLUDED
#define STR_INCLUDED

#include <stddef.h>

/* Return the length of string pcSrc, excluding the trailing '\0'. */
size_t Str_getLength(const char pcSrc[]);

/* Copies the string pcSrc to string pcDest and returns pcDest. */
char *Str_copy(char pcDest[], const char pcSrc[]);

/* Concatenates the string pcSrc after the string pcDest (overwrites its '\0').
Returns the concatenated pcDest. */
char *Str_concat(char pcDest[], const char pcSrc[]);

/* Compares the two strings pcStr1 and pcStr2 (based off on ASCII values). 
Returns 0 if pcStr1 and pcStr2 are equal, a negative value if pcStr1 is less than s2,
and a positive value if pcStr1 is greater than pcStr2. */
int Str_compare(const char pcStr1[], const char pcStr2[]);

/* Finds the first occurence of the string pcNeedle in the string pcHaystack 
('\0' is not compared). Returns a pointer to the beginning of the pcNeedle in pcHaystack. 
Returns NULL if pcNeedle is not found in pcHaystack. */
char *Str_search(const char pcHaystack[], const char pcNeedle[]);

#endif
