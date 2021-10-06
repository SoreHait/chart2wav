#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

char* getMiddleText(char* text, char* beginWith, char* endWith) {
    // TODO
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
