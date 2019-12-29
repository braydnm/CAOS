#ifndef STRING_H
#define STRING_H

#include "../kernel/kernel.h"

#define BITOP(A, B, OP) \
    ((A)[(size_t)(B)/(8*sizeof *(A))] OP (size_t)1<<((size_t)(B)%(8*sizeof *(A))))


#define ALIGN (sizeof(size_t))
#define ONES ((size_t)-1/UCHAR_MAX)
#define HIGHS (ONES * (UCHAR_MAX/2+1))
#define HASZERO(X) (((X)-ONES) & ~(X) & HIGHS)

void itoa(int num, unsigned base, char *str);

int atoi(char* str);

size_t strlen(char str[]);

void strcpy(char* dest, char* source);

char capitalize(char letter);

void appendStr(char *str, char *c);

int strcmp(char str[], char str2[]);

void deleteChar(char str[], int num);

void hexToAscii(int n, char str[]);

list_t * split(char* str, char* del);

int count(char* str, char* del);

bool contains(char* str, char* check);

char* strdup(const char* str);

char* strstr(const char* ch1, const char* ch2);

char* strchr(const char* p, int ch);

int strncmp(const char *s1, const char *s2, size_t n);

int strbackspace(char* str, char back);

extern char *strtok(char *, const char *);

size_t strspn(const char * s, const char * c);

char * strpbrk(const char * s, const char * b);

int tokenize(char* str, char* sep, char** buf);

extern char * strtok_r(char * str, const char * delim, char ** saveptr);

extern uint8_t startswith(const char * str, const char * accept);

extern int endswith(const char* str, const char* accept);

#endif