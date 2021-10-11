#ifndef AFF2WAV_C_MAIN_H
#define AFF2WAV_C_MAIN_H

#include <string.h>
typedef struct {
    long timing;
    int type;
} MIXERDATA;

MIXERDATA* analyzeOSU(char* affContent, size_t contentLength, size_t* output_mixerDataLength);

#ifdef MAIN_C
#include "audiodata.h"
#endif

#ifndef MAIN_C
extern const int AUDIOTAP_LEN;
extern const int AUDIOARC_LEN;
extern const unsigned char WAVHEADER[];
extern const unsigned char AUDIOTAP[];
extern const unsigned char AUDIOARC[];
#endif

#endif
