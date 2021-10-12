#define _CRT_SECURE_NO_WARNINGS
#include "util/declare.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    // Check argc
    if (argc < 3) {
        printf("Not enough args.\n");
        return 1;
    }

    // Validate file suffix
    char* fileName = argv[1];
    char* fileSuffix = fileName + strlen(fileName) - 4;
    int generate_type;
    if (strcmp(fileSuffix, ".aff") == 0) {
        generate_type = ARC;
    }
    else if (strcmp(fileSuffix, ".osu") == 0) {
        generate_type = OSU;
    }
    else {
        printf("Not arcaea or osu file.");
        return 1;
    }

    // Generate output path
    char* outputFile = (char*)malloc(strlen(argv[2]) + 5); // +5 for ".wav\0"
    strcpy(outputFile, argv[2]);
    strcat(outputFile, ".wav");

    // Load file
    FILE* fp = fopen(fileName, "r");
    size_t mallocSize = 100;
    char* fileContent = (char*)malloc(mallocSize);
    size_t byteCount = fread((void*)fileContent, 1, mallocSize, fp);
    if (byteCount == 0) {
        printf("Empty file.\n");
        return 1;
    }
    while (byteCount == mallocSize) { // File not done reading
        rewind(fp);
        mallocSize += 100;
        fileContent = (char*)realloc((void*)fileContent, mallocSize);
        byteCount = fread((void*)fileContent, 1, mallocSize, fp);
    }
    fclose(fp);
    memset((char*)fileContent + byteCount, '\0', mallocSize - byteCount);

    // Analyze file
    size_t mixerDataLength = 0;
    size_t* mixerDataLength_ptr = &mixerDataLength;
    MIXERDATA* (*analyzer)(char*, size_t, size_t*);
    int offset;
    if (generate_type == ARC) {
        analyzer = analyzeAff;
        offset = (int)strtol(fileContent + 12, NULL, 10); // +12 for "AudioOffset:"
    }
    else if (generate_type == OSU) {
        analyzer = analyzeOSU;
        offset = 0;
    }
    else {
        return 1;
    }
    MIXERDATA* mixerData = analyzer(fileContent, byteCount, mixerDataLength_ptr);
    free(fileContent);

    // Mix keysound
    size_t keysoundDataLength = 0;
    size_t* keysoundDataLength_ptr = &keysoundDataLength;
    unsigned char* keysoundData = mixKeysound(mixerData, mixerDataLength, keysoundDataLength_ptr, offset);
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
    return 0;
}
