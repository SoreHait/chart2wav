#include <string.h>
#include <stdlib.h>
#include "../util/declare.h"

MIXERDATA* analyzeOSU(char* affContent, size_t contentLength, size_t* output_mixerDataLength) {
    // Get number of lines and max char count of all lines
    // Q: why initial value of lineCount is 1? A: number of '\n's is always lineCount - 1.
    int lineCount = 1, maxCharCount = 0, charCount = 0;
    int is_begins = 0;
    for (int i = 0; i < contentLength; i++) {
        charCount++;
        char currentChar = affContent[i];
        if (currentChar == '\n') {
            lineCount++;
            if (charCount > maxCharCount) {
                maxCharCount = charCount;
            }
            charCount = 0;
        }
    }

    // Declare a char array to store each line
    char** lineContent = (char**)malloc(sizeof(char*) * lineCount);
    for (int i = 0; i < lineCount; i++) {
        lineContent[i] = (char*)malloc(sizeof(char) * maxCharCount);
    }

    // Split each line and store
    char* token;
    int lineIndex = 0;
    token = strtok(affContent, "\n");
    while (token != NULL) {
        strcpy(lineContent[lineIndex], token);
        lineIndex++;
        token = strtok(NULL, "\n");
    }

    // Extract objects
    MIXERDATA* mixerDataTmp = (MIXERDATA*)malloc(sizeof(MIXERDATA) * lineCount); // maybe long enough
    *output_mixerDataLength = 0;
    for (int i = 0; i < lineCount; i++) {
        char* currentLine = lineContent[i];
        if (strncmp(currentLine, "[HitObjects]", 12) == 0) {  // HitObjects Begins
            is_begins = 1;
            continue;
        }

        if (is_begins) {
            if (strlen(currentLine) == 0) {
                break;
            }
            (mixerDataTmp + *output_mixerDataLength) -> timing = strtol(splittext(currentLine, ",", 2), NULL, 10);
            (mixerDataTmp + *output_mixerDataLength) -> type = 0;
            (*output_mixerDataLength)++;
        }
    }
    MIXERDATA* mixerData = (MIXERDATA*)malloc(sizeof(MIXERDATA) * *output_mixerDataLength);
    memcpy(mixerData, mixerDataTmp, sizeof(MIXERDATA) * *output_mixerDataLength);
    free(mixerDataTmp);
    for (int i = 0; i < lineCount; i++) {
        free(lineContent[i]);
    }
    free(lineContent);
    return mixerData;
}
