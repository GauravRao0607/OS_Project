/*
 * (C) 2026, Cornell University
 * All rights reserved.
 *
 * Description: starter code for implementing printf()
 */

void terminal_write(const char *str, int len) {
    for (int i = 0; i < len; i++) {
        *(char*)(0x10000000) = str[i];
    }
}


#include <stdlib.h>  // for itoa() and utoa()
#include <string.h>  // for strlen() and strcat()
#include <stdarg.h>  // for va_start(), va_end(), va_arg() and va_copy()

char* ulltoa(unsigned long long value, char* str, int base){
    const char digits [] = "0123456789abcdef";
    int i = 0;
    if(value == 0){
        str[i] = '0';
        i++;
    }

    while(value != 0){
        str[i] = digits[value % base];
        value /= base;
        i++;
    }
    str[i]='\0';

    for( int lo = 0, hi = i-1; lo<hi; lo++, hi-- ){
        char tmp = str[lo];
        str[lo] = str[hi];
        str[hi] = tmp;
    }
    
    return str;

}

void format_to_str(char* out, const char* fmt, va_list args) {
    for(out[0] = 0; *fmt != '\0'; fmt++) {
        if (*fmt != '%') {
            strncat(out, fmt, 1);
        } else {
            fmt++;
            if (*fmt == 's') {
                strcat(out, va_arg(args, char*));
            } else if (*fmt == 'd') {
                itoa(va_arg(args, int), out + strlen(out), 10);
            } else if(*fmt == 'u'){
                utoa(va_arg(args, unsigned int), out + strlen(out), 10);
            } else if(*fmt == 'l' && fmt[1] =='l' && fmt[2] == 'u'){
                 fmt += 2;
                ulltoa(va_arg(args, unsigned long long), out + strlen(out), 10);
            } else if (*fmt == 'c') {
                char c = (char)va_arg(args, int);
                strncat(out, &c, 1);
            } else if (*fmt == 'x') {
                utoa(va_arg(args, unsigned int), out + strlen(out), 16);
            } else if (*fmt == 'p') {
                strcat(out, "0x");
                utoa((unsigned int)va_arg(args, void*), out + strlen(out), 16);
            }

        }
    }
}

unsigned int format_to_str_len(const char* fmt, va_list args){
    int len = 0;
    char tmp[24];

    for(;*fmt!='\0'; fmt++){
        if(*fmt != '%'){
            len++;
        } else{
            fmt++;
            if (*fmt == 's') {
                strcat(tmp, va_arg(args, char*));
                len += strlen(tmp);
            } else if (*fmt == 'd') {
                itoa(va_arg(args, int), tmp, 10);
                len += strlen(tmp);
            } else if(*fmt == 'u'){
                utoa(va_arg(args, unsigned int), tmp, 10);
                len += strlen(tmp);
            } else if(*fmt == 'l' && fmt[1] =='l' && fmt[2] == 'u'){
                 fmt += 2;
                ulltoa(va_arg(args, unsigned long long), tmp, 10);
                len += strlen(tmp);
            } else if (*fmt == 'c') {
                char c = (char)va_arg(args, int);
                strncat(tmp, &c, 1);
                len += strlen(tmp);
            } else if (*fmt == 'x') {
                utoa(va_arg(args, unsigned int), tmp, 16);
                len += strlen(tmp);
            } else if (*fmt == 'p') {
                utoa((unsigned int)va_arg(args, void*), tmp, 16);
                len += strlen(tmp) + 2;
            }
        }

    }
}

int printf(const char* format, ...) {
    va_list args;
    va_start(args, format);

    va_list args_copy;
    va_copy(args_copy, args);
    unsigned int len = format_to_str_len(format, args_copy);
    char *buf = malloc(len);

    format_to_str(buf, format, args);
    va_end(args);
    terminal_write(buf, strlen(buf));

    va_end(args_copy);
    free(buf);

    return 0;
}



extern char __heap_start, __heap_end;
static char* brk = &__heap_start;
char* _sbrk(int size) {
    if (brk + size > (char*)&__heap_end) {
        terminal_write("_sbrk: heap grows too large\r\n", 29);
        return NULL;
    }

    char* old_brk = brk;
    brk += size;
    return old_brk;
}


int main() {
    char* msg = "Hello, World!\n\r";
    terminal_write(msg, 15);

    printf("%s-%d is awesome!\n\r", "egos", 2000); 
    printf("%c is character $\n", '$');
    printf("%c is character 0\n", (char)48);
    printf("%x is integer 1234 in hexadecimal\n", 1234);
    printf("%u is the maximum of unsigned int\n", (unsigned int)0xFFFFFFFF);
    printf("%p is the hexadecimal address of the hello-world string\n", msg);
    printf("%llu is the maximum of unsigned long long\n", 0xFFFFFFFFFFFFFFFFULL);
    return 0;
}
