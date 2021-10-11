#include "declare.h"
#include <stdio.h>
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

char* splittext(char* text, char* split, int retcount) {
    char* pt = NULL;
    int count = 0;
    pt = strtok(text, split);
    while (pt != NULL){
        if (count == retcount){
            return (pt);
        }
        pt = strtok(NULL, split);
        count++;
    }
    return NULL;
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
