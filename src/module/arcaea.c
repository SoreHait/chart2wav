#include <stdlib.h>
#include <string.h>
#include "../util/declare.h"

MIXERDATA* analyzeAff(char* affContent, size_t contentLength, size_t* output_mixerDataLength) {
    // Get number of lines and max char count of all lines
    // Q: why initial value of lineCount is 1? A: number of '\n's is always lineCount - 1.
    int lineCount = 1, maxCharCount = 0, charCount = 0;
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
    for (int i = 0; i < lineCount; i++) {
        free(lineContent[i]);
    }
    free(lineContent);
    return mixerData;
}
