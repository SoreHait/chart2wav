#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>


short validateShort(int x, int y) {
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
    size_t len = strlen(text);
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
    char* l_pos, * r_pos;
    size_t beginLength = strlen(beginWith);
    l_pos = strstr(text, beginWith) + beginLength;
    r_pos = strstr(text, endWith);
    long offset = r_pos - l_pos;
    char* retStr = (char*)malloc(offset);
    strncpy(retStr, l_pos, offset);
    return retStr;
}

MIXERDATA* analyzeAff(void* affContent, size_t contentLength, size_t* output_mixerDataLength) {
    // TODO
}

char* mixKeysound(MIXERDATA* mixerData, size_t mixerDataLength) {
    // TODO
}

void* packWav(char* data) {
    // TODO
}

int main(int argc, char* argv[]) {
    // Check argc
    if (argc < 3) {
        printf("Not enough args.");
        return 1;
    }

    // Validate .aff
    char* affFile = argv[1];
    char* affFileName = basename(affFile);
    if (strstr(affFileName, ".aff") == NULL || strcmp(strstr(affFileName, ".aff"), ".aff") != 0) {
        printf("Not aff file.");
        return 1;
    }

    // Generate output path
    char* outputFileName = argv[2];
    strcat(outputFileName, ".wav");
    char* outputFile = dirname(affFile);
    strcat(outputFile, "/");
    strcat(outputFile, outputFileName);

    // Load aff
    FILE* afffp = fopen(affFile, "r");
    size_t mallocSize = 100;
    void* affContent = malloc(mallocSize);
    size_t byteCount = fread(affContent, 1, mallocSize, afffp);
    while (byteCount == mallocSize) { // File not done reading
        rewind(afffp);
        free(affContent);
        mallocSize += 100;
        affContent = malloc(mallocSize);
        byteCount = fread(affContent, 1, mallocSize, afffp);
    }

    // Analyze aff
    size_t mixerDataLength = 0;
    size_t* mixerDataLength_ptr = &mixerDataLength;
    MIXERDATA* mixerData = analyzeAff(affContent, byteCount, mixerDataLength_ptr);

    // Mix keysound
    char* keysoundData = mixKeysound(mixerData, mixerDataLength);

    // Pack .wav
    void* wavData = packWav(keysoundData);

    // Output .wav
    // TODO
}
