
/* Converts decimal string to hexadecimal string.
* e.g.   256 -> 100
*  len specifies the length of decimal string(dec)
*  length of the output buffer(hex) should be at least len bytes.
*/
void dec2hex(char* dec, char* hex, int len){
    /* 1248 encoding -> hex */
    char hexmap[] = {
        '0', '8', '4', 'C', '2', 'A', '6', 'E',
        '1', '9', '5', 'D', '3', 'B', '7', 'F'
    };

    int bits = 0, part = 0;
    char* g = dec;
    char* p = hex;
    char* const z = dec + len;
    for (int rlen = len; rlen;){

        int carry = 0, remainder = 0;
        for (char* s = g; s != z; ++s){
            int d = *s - '0';
            remainder = d & 0x01;
            *s = (char)((d >> 1) + carry + '0');
            carry = remainder ? 5 : 0;
        }
        if (*g == '0'){
            ++g;
            --rlen;
        }

        part <<= 1;
        part |= remainder;
        ++bits;

        /* 4 bits per group */
        if (!(bits & 3)){
            *(p++) = hexmap[part];
            part = 0;
        }
    }

    if (bits & 3){
        part <<= 4 - (bits & 3);
        *(p++) = hexmap[part];
    }
    *p = 0;

    /* Reverse output string */
    for (char* left = hex, *right = p - 1; left < right; ++left, --right){
        char t = *left;
        *left = *right;
        *right = t;
    }
}
