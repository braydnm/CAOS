//
// Created by x3vikan on 2/18/18.
//
#include "string.h"

const char hexToAsciiMap[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

void reverse(char str[], int length)
{
    char temp;
    int start = 0;
    int end = length -1;
    while (start < end) {
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}

int strncmp(const char *s1, const char *s2, size_t n) {
    unsigned char u1, u2;
    while (n-- > 0)
    {
        u1 = (unsigned char) *s1++;
        u2 = (unsigned char) *s2++;
        if (u1 != u2)
            return u1 - u2;
        if (u1 == '\0')
            return 0;
    }
    return 0;
}

char * strchrnul(const char * s, int c) {
    size_t * w;
    size_t k;

    c = (unsigned char)c;
    if (!c) {
        return (char *)s + strlen(s);
    }

    for (; (uintptr_t)s % ALIGN; s++) {
        if (!*s || *(unsigned char *)s == c) {
            return (char *)s;
        }
    }

    k = ONES * c;
    for (w = (void *)s; !HASZERO(*w) && !HASZERO(*w^k); w++);
    for (s = (void *)w; *s && *(unsigned char *)s != c; s++);
    return (char *)s;
}

char * strchr(const char * s, int c) {
    char *r = strchrnul(s, c);
    return *(unsigned char *)r == (unsigned char)c ? r : 0;
}

char* strstr (const char *s1, const char *s2) {
    const char *p = s1;
    const size_t len = strlen (s2);

    for (; (p = strchr (p, *s2)) != 0; p++) {
        if (strncmp (p, s2, len) == 0)
            return (char *)p;
    }
    return (0);
}


int count(char* str, char* search){
    int count = 0;
    const char *tmp = str;
    while(tmp = strstr(tmp, search)) {
        count++;
        tmp++;
    }
    return count;
}

//
//int count(char* str, char* search){
//    int found = 0, count = 0;
//    int stringLen = strlen(str), searchLen = strlen(search);
//
//    for (int counter = 0; counter<stringLen-searchLen; counter++){
//        found = 1;
//        for (int counter2 = 0; counter2<searchLen; counter2++)
//            if (str[counter+counter2]!=search[counter2]){
//                found = 0;
//                break;
//            }
//
//        if (found==1) count++;
//    }
//
//    return count;
//}

size_t strcspn(const char * s, const char * c) {
    const char *a = s;
    if (c[0] && c[1]) {
        size_t byteset[32/sizeof(size_t)] = { 0 };
        for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
        for (; *s && !BITOP(byteset, *(unsigned char *)s, &); s++);
        return s-a;
    }
    return strchrnul(s, *c)-a;
}


void strcpy_s(char* dest, char* source, size_t size){
    int index = 0;
    while((dest[index]=source[index])!='\0' && index<size)
        index++;
}

list_t* split(char* str, char* del){
    list_t* ret = list_create();
    char * pch_i;
    char * save_i;
    pch_i = strtok_r(str,del,&save_i);
    if (!pch_i) {
        list_insert(ret, pch_i);
        return ret;
    }
    while (pch_i != NULL) {
        list_insert(ret, pch_i);
        pch_i = strtok_r(NULL,del,&save_i);
    }
    return ret;
}

void _itoa(unsigned int num, unsigned base, char* str){
    int pos = 0;
    char reverseBuf[32];
    if (num == 0 || base>16){
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    num*=base;
    while ((num/=base)!=0)
        reverseBuf[pos++] = hexToAsciiMap[num % base];
    int size = pos--;
    for (int counter = 0; counter<size; pos--, counter++)
        str[counter] = reverseBuf[pos];

    str[size] = '\0';

}

void itoa(int num, unsigned base, char* str){
    if (base>16) return;

    if (num<0){
        *str++ = '-';
        num*=-1;
    }
    _itoa(num, base, str);
}

size_t strlen(char str[]){
    size_t i = 0;
    while (str[i++]!='\0');
    return i-1;
}

void strcpy(char* dest, char* source){
    int index = 0;
    while((dest[index]=source[index])!='\0')
        index++;
}

char capitalize(char letter){return letter-32;}

void appendStr(char *str, char *c){
    int len = strlen(str);
    if (len+ strlen(c)>=256) return;
    for (int i = 0; i<strlen(c); i++)
        str[len+i] = c[i];
    str[len+strlen(c)] = '\0';
}

void appendChar(char* str, char c){
    int len = strlen(str);
    if (len>=255) return;
    str[len] = c;
    str[len+1] = '\0';
}

void deleteChar(char str[], int num){
    int len = strlen(str);
    str[len-num]='\0';
}

// Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2
int strcmp(char s1[], char s2[]) {
    int ptr;
    for (ptr = 0; s1[ptr] == s2[ptr]; ptr++) {
        if (s1[ptr] == '\0') return 0;
    }
    return s1[ptr] - s2[ptr];
}

void hexToAscii(int n, char str[]){
    appendStr(str, "0x");
    char ret[10];
    int mask = 0xF;
    for (int magnitude = 0; magnitude<8; magnitude++) {
        appendChar(ret, hexToAsciiMap[(n & mask) / (0x1*(mask/0xF))]);
        mask*=0x10;
    }
    reverse(ret, strlen(ret));
    memcpy(ret, str+2, strlen(ret));
}

char* strdup(const char* str){
    size_t len = strlen(str)+1;
    void* new = kmalloc(len);
    if (new == null) return null;
    return (char*)memcpy(new, str, len);
}

bool contains(char* str, char* check){
    int found = 0, count = 0;
    int stringLen = strlen(str), searchLen = strlen(check);

    for (int counter = 0; counter<stringLen-searchLen; counter++){
        found = 1;
        for (int counter2 = 0; counter2<searchLen; counter2++)
            if (str[counter+counter2]!=check[counter2]){
                found = 0;
                break;
            }

        if (found==1) return true;
    }
    return false;
}

int atoi(char* str){
    int res = 0;

    for (int i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    return res;
}

int strbackspace(char* str, char back){
    size_t i = strlen(str)-1;
    while (i--){
        if (str[i]==back) {
            str[i + 1] = 0;
            return 1;
        }
    }
    return 0;
}

static char* sp = 0;

char* strtok(char *str, const char *delim) {
    int i = 0;
    int len = strlen(delim);

    if (len == 0)
        return 0;

    if (!str && !sp)
        return 0;

    if (str && !sp)
        sp = str;

    char *p_start = sp;
    while(1) {
        for(i = 0; i < len; i++) {
            if (*p_start == delim[i]) {
                p_start ++;
                break;
            }
        }

        if (i == len) {
            sp = p_start;
            break;
        }
    }

    if (*sp == 0) {
        sp = 0;
        return sp;
    }

    while (*sp != 0) {
        for (i = 0; i < len; i++) {
            if (*sp == delim[i]) {
                *sp = 0;
                break;
            }
        }
        sp ++;
        if (i < len)
            break;
    }

    return p_start;
}

size_t strspn(const char * s, const char * c) {
    const char * a = s;
    size_t byteset[32/sizeof(size_t)] = { 0 };

    if (!c[0]) {
        return 0;
    }
    if (!c[1]) {
        for (; *s == *c; s++);
        return s-a;
    }

    for (; *c && BITOP(byteset, *(unsigned char *)c, |=); c++);
    for (; *s && BITOP(byteset, *(unsigned char *)s, &); s++);

    return s-a;
}


char * strpbrk(const char * s, const char * b) {
    s += strcspn(s, b);
    return *s ? (char *)s : 0;
}

size_t lfind(const char * str, const char accept) {
    return (size_t)strchr(str, accept);
}

char * strtok_r(char * str, const char * delim, char ** saveptr) {
    char * token;
    if (str == NULL) {
        str = *saveptr;
    }
    str += strspn(str, delim);
    if (*str == '\0') {
        *saveptr = str;
        return NULL;
    }
    token = str;
    str = strpbrk(token, delim);
    if (str == NULL) {
        *saveptr = (char *)lfind(token, '\0');
    } else {
        *str = '\0';
        *saveptr = str + 1;
    }
    return token;
}

int tokenize(char * str, char * sep, char **buf) {
    char * pch_i;
    char * save_i;
    int    argc = 0;
    pch_i = strtok_r(str,sep,&save_i);
    if (!pch_i) { return 0; }
    while (pch_i != NULL) {
        buf[argc] = (char *)pch_i;
        ++argc;
        pch_i = strtok_r(NULL,sep,&save_i);
    }
    buf[argc] = NULL;
    return argc;
}

uint8_t startswith(const char * str, const char * accept) {
    return strstr(str, accept) == str;
}

int endswith(const char* str, const char* accept){
    int str_len = strlen(str);
    int suffix_len = strlen(accept);

    return
            (str_len >= suffix_len) &&
            (0 == strcmp(str + (str_len-suffix_len), accept));
}