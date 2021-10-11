#define _CRT_SECURE_NO_WARNINGS
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


short validateShort(short x, short y) {
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

const unsigned char* getData(int dataType) {
    if (dataType) {
        return AUDIOARC;
    }
    else {
        return AUDIOTAP;
    }
}

int getDataLen(int dataType) {
    if (dataType) {
        return AUDIOARC_LEN;
    }
    else {
        return AUDIOTAP_LEN;
    }
}

int getTiming(const void* a, const void* b) {
    long aTiming = ((MIXERDATA*)a) -> timing;
    long bTiming = ((MIXERDATA*)b) -> timing;
    return (int)(aTiming - bTiming);
}

char* getMiddleText(char* text, char* beginWith, char* endWith) {
    char* l_pos, * r_pos;
    size_t beginLength = strlen(beginWith);
    l_pos = strstr(text, beginWith) + beginLength;
    r_pos = strstr(text, endWith);
    *r_pos = '\0'; // Mark end
    long long len = r_pos - l_pos;
    char* retStr = (char*)malloc(len + 1); // Space for tailing '\0'
    strcpy(retStr, l_pos);
    return retStr;
}

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

unsigned char* mixKeysound(MIXERDATA* mixerData, size_t mixerDataLength, size_t* output_keysoundDataLength) {
    float mixTimeLen;
    if (mixerDataLength == 0) {
        mixTimeLen = 0;
    }
    else {
        qsort(mixerData, mixerDataLength, sizeof(MIXERDATA), getTiming);
        long min = mixerData[0].timing;
        for (int i = 0; i < mixerDataLength; i++) {
            mixerData[i].timing -= min;
        }
        mixTimeLen = (float)(mixerData[mixerDataLength - 1].timing) / 1000 + bits2time(getDataLen(mixerData[mixerDataLength - 1].type));
    }
    printf("Audio Time: %f sec.\n", mixTimeLen);

    int mixSampleCount = (int)(mixTimeLen * 44100);
    printf("Sample Count: %d.\n", mixSampleCount);

    // Mix keysound
    short* mixBuffer = (short*)calloc(mixSampleCount, sizeof(short));
    for (int i = 0; i < mixerDataLength; i++) {
        MIXERDATA currentMixerData = mixerData[i];
        int mixSamplePos = (int)((double)(currentMixerData.timing) * 44.1);
        int dataLen = getDataLen(currentMixerData.type);
        unsigned char* data = (unsigned char*)malloc(dataLen);
        memcpy(data, getData(currentMixerData.type), dataLen);
        for (int j = 0; j < dataLen; j += 2) {
            mixSamplePos++;
            if (mixSamplePos >= mixSampleCount) {
                break;
            }
            mixBuffer[mixSamplePos] = validateShort(mixBuffer[mixSamplePos], makeWord(data[j], data[j + 1]));
        }
        free(data);
    }
    *output_keysoundDataLength = mixSampleCount * 2;
    unsigned char* outputBuffer = (unsigned char*)malloc(*output_keysoundDataLength);
    memcpy(outputBuffer, mixBuffer, *output_keysoundDataLength);
    free(mixBuffer);
    return outputBuffer;
}

void* packWav(unsigned char* keysoundData, size_t keysoundDataLength, size_t* output_wavDataLength) {
    unsigned char wavHeader[44];
    memcpy(wavHeader, WAVHEADER, 44);
    *output_wavDataLength = keysoundDataLength + 36;

    for (int i = 0; i < 4; i++) {
        wavHeader[i + 4] = (unsigned char)(((*output_wavDataLength) >> (i * 8)) & 0xFF);
        wavHeader[i + 40] = (unsigned char)(((keysoundDataLength) >> (i * 8)) & 0xFF);
    }

    void* output_wav = malloc(44 + keysoundDataLength);
    memcpy(output_wav, wavHeader, 44);
    memcpy((char*)(output_wav) + 44, keysoundData, keysoundDataLength);
    return output_wav;
}

int main(int argc, char* argv[]) {
    // Check argc
    if (argc < 3) {
        printf("Not enough args.\n");
        return 1;
    }

    // Validate .aff
    char* affFile = argv[1];
    if (strstr(affFile, ".aff") == NULL || strcmp(strstr(affFile, ".aff"), ".aff") != 0) {
        printf("Not aff file.\n");
        return 1;
    }

    // Generate output path
    char* outputFile = (char*)malloc(strlen(argv[2]) + 5);
    strcpy(outputFile, argv[2]);
    strcat(outputFile, ".wav");

    // Load aff
    FILE* afffp = fopen(affFile, "r");
    size_t mallocSize = 100;
    char* affContent = (char*)malloc(mallocSize);
    size_t byteCount = fread((void*)affContent, 1, mallocSize, afffp);
    if (byteCount == 0) {
        printf("Empty file.\n");
        return 1;
    }
    while (byteCount == mallocSize) { // File not done reading
        rewind(afffp);
        mallocSize += 100;
        affContent = (char*)realloc((void*)affContent, mallocSize);
        byteCount = fread((void*)affContent, 1, mallocSize, afffp);
    }
    fclose(afffp);
    memset((char*)affContent + byteCount, '\0', mallocSize - byteCount);

    // Analyze aff
    size_t mixerDataLength = 0;
    size_t* mixerDataLength_ptr = &mixerDataLength;
    MIXERDATA* mixerData = analyzeAff(affContent, byteCount, mixerDataLength_ptr);
    free(affContent);

    // Mix keysound
    size_t keysoundDataLength = 0;
    size_t* keysoundDataLength_ptr = &keysoundDataLength;
    unsigned char* keysoundData = mixKeysound(mixerData, mixerDataLength, keysoundDataLength_ptr);
    free(mixerData);

    // Pack .wav
    size_t wavDataLength = 0;
    size_t* wavDataLength_ptr = &wavDataLength;
    void* wavData = packWav(keysoundData, keysoundDataLength, wavDataLength_ptr);
    free(keysoundData);

    // Output .wav
    FILE* wavfp = fopen(outputFile, "wb");
    fwrite(wavData, wavDataLength, 1, wavfp);
    free(wavData);
    fclose(wavfp);
    free(outputFile);
    printf("Done.\n");
}
