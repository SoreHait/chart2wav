#ifndef AFF2WAV_C_DECLARE_H
#define AFF2WAV_C_DECLARE_H
#include <string.h> // For size_t definition

typedef struct {
    long timing;
    int type;
} MIXERDATA;

// Module
MIXERDATA* analyzeOSU(char* affContent, size_t contentLength, size_t* output_mixerDataLength);
MIXERDATA* analyzeAff(char* affContent, size_t contentLength, size_t* output_mixerDataLength);

// Util
short validateShort(short x, short y);
short makeWord(unsigned char low, unsigned char high);
float bits2time(int bitCount);
const unsigned char* getData(int dataType);
int getDataLen(int dataType);
int getTiming(const void* a, const void* b);
char* getMiddleText(char* text, char* beginWith, char* endWith);
unsigned char* mixKeysound(MIXERDATA* mixerData, size_t mixerDataLength, size_t* output_keysoundDataLength, int offset);
void* packWav(unsigned char* keysoundData, size_t keysoundDataLength, size_t* output_wavDataLength);
char* splittext(char* text, char* split, int retcount);

// Const data
extern const int AUDIOTAP_LEN;
extern const int AUDIOARC_LEN;
extern const unsigned char WAVHEADER[];
extern const unsigned char AUDIOTAP[];
extern const unsigned char AUDIOARC[];
extern const int ARC;
extern const int OSU;

#endif
