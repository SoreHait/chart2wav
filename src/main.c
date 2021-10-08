#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>


short mixFunc(int x, int y) {
    int sum = x + y;
    if (sum > 32767) {
        return 32767;
    }
    else if (sum < -32768) {
        return -32768;
    }
    else {
        return (short)sum;
    }
}

short makeWord(unsigned char low, unsigned char high) {
    short word = (short)((high << 8) + low);
    return word;
}

float bits2time(int bitCount) {
    return ((float)bitCount) / 88200;
}

char* strreverse(char* text) {
    unsigned int len = strlen(text);
    int i, n;
    char test;
    char* rev = (char*)malloc(len * sizeof(char));
    for (i = (int)len - 1, n = 0; i >= 0; i--, n++) {
        test = text[i];
        rev[n] = test;
    }
    rev[len] = '\0';
    return rev;
}

char* getMiddleText(char* text, char* beginWith, char* endWith) {
    char* src = text;
    char* p1, * p2;
    p1 = strstr(src, beginWith);
    p2 = strstr(src, endWith);
    char* dest = (char*)malloc((p2 - p1) * sizeof(char));

    if (p1 == NULL || p2 == NULL) {
        return("");
    }
    else if (p1 > p2) {
        //TODO: 取"12345612"中"34" 和 第二个"12"中间的数
        return("");

    }
    else {
        p1 += strlen(beginWith);
        memcpy(dest, p1, p2 - p1);
        dest[strlen(dest)] = '\0';
    }
    return(dest);
}

MIXERDATA* analyzeAff(char* affContent) {
    // TODO
}

int* mixKeysound(MIXERDATA* mixerData) {
    // TODO
}

void* packWav(int* data) {
    // TODO
}

int main() {
    // TODO
}
