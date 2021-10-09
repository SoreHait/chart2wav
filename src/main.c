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
    long long len = r_pos - l_pos;
    char* retStr = (char*)malloc(len + 1); // Space for tailing '\0'
    strncpy(retStr, l_pos, len);
    return retStr;
}

MIXERDATA* analyzeAff(void* affContent, size_t contentLength, size_t* output_mixerDataLength) {
    // Get number of lines and max char count of all lines
    // Q: why initial value of lineCount is 1? A: number of '\n's is always lineCount - 1.
    int lineCount = 1, maxCharCount = 0, charCount = 0;
    for (int i = 0; i < contentLength; i++) {
        charCount++;
        char currentChar = *((char*)affContent + i);
        if (currentChar == '\n') {
            lineCount++;
            if (charCount > maxCharCount) {
                maxCharCount = charCount;
            }
            charCount = 0;
        }
    }

    // Declare a char array to store each line
    char lineContent[lineCount][maxCharCount + 1]; // maxCharCount + 1 for safety reasons (ex. '\0')

    // Split each line and store
    char* token;
    int lineIndex = 0;
    token = strtok((char*)affContent, "\n");
    while (token != NULL) {
        strcpy(lineContent[lineIndex], token);
        token = strtok(NULL, (char*)affContent);
    }

    // Extract objects
    MIXERDATA* mixerDataTmp = (MIXERDATA*)malloc(sizeof(MIXERDATA) * lineCount * 2); // maybe long enough
    *output_mixerDataLength = 0;
    for (int i = 0; i < lineCount; i++) {
        char* currentLine = lineContent[i];
        if (strncmp(currentLine, "arc", 3) == 0) { // Is ARC
            if ((strstr(currentLine, "false") != NULL) && (strstr(currentLine, "arctap") == NULL)) { // not black line / arctap
                (mixerDataTmp + *output_mixerDataLength) -> timing = strtol(getMiddleText(currentLine, "arc(", ","), NULL, 10);
                (mixerDataTmp + *output_mixerDataLength) -> type = 1;
                (*output_mixerDataLength)++;
            }
            if (strstr(currentLine, "arctap") != NULL) { // is arctap
                char* arctapStr = getMiddleText(currentLine, "[", "]");
                token = strtok(arctapStr, ",");
                while (token != NULL) {
                    (mixerDataTmp + *output_mixerDataLength) -> timing = strtol(getMiddleText(token, "(", ")"), NULL, 10);
                    (mixerDataTmp + *output_mixerDataLength) -> type = 1;
                    (*output_mixerDataLength)++;
                    token = strtok(NULL, ",");
                }
            }
        }
        else if (strncmp(currentLine, "hold", 4) == 0) { // Is HOLD
            (mixerDataTmp + *output_mixerDataLength) -> timing = strtol(getMiddleText(currentLine, "hold(", ","), NULL, 10);
            (mixerDataTmp + *output_mixerDataLength) -> type = 0;
            (*output_mixerDataLength)++;
        }
        else if (strncmp(currentLine, "(", 1) == 0) { // Normal TAP
            (mixerDataTmp + *output_mixerDataLength) -> timing = strtol(getMiddleText(currentLine, "(", ","), NULL, 10);
            (mixerDataTmp + *output_mixerDataLength) -> type = 0;
            (*output_mixerDataLength)++;
        }
    }
    MIXERDATA* mixerData = (MIXERDATA*)malloc(sizeof(MIXERDATA) * *output_mixerDataLength);
    memcpy(mixerData, mixerDataTmp, sizeof(MIXERDATA) * *output_mixerDataLength);
    free(mixerDataTmp);
    return mixerData;
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
    if (byteCount == 0) {
        printf("Empty file.");
        return 1;
    }
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
    free(affContent);

    // Mix keysound
    char* keysoundData = mixKeysound(mixerData, mixerDataLength);

    // Pack .wav
    void* wavData = packWav(keysoundData);

    // Output .wav
    // TODO
}
