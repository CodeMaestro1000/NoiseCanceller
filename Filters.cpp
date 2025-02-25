//
// Created by Akoji Timothy on 22/02/2025.
//

#include "Filters.h"



void RNNFilter::apply(const float *inputBuffer, float *outputBuffer) {
    constexpr  int FRAME_SIZE = 480;
    constexpr  float SCALE_FACTOR = 32767.0f;

    float tempFrame[FRAME_SIZE];

    // PortAudio has data in the range -1 to 1 while rnnoise expects data in raw 16 bit PCM format
    // -32768 to +32767. Hence, we must scale the input data from the mic (as if it were 16-bit PCM)
    for (size_t i = 0; i < FRAME_SIZE; ++i)
        tempFrame[i] = inputBuffer[i] * SCALE_FACTOR;

    // Apply RNNoise
    rnnoise_process_frame(this->filterDenoiseState, tempFrame, tempFrame);

    // Scale back to float (-1.0 to 1.0 range) before outputting
    for (size_t i = 0; i < FRAME_SIZE; ++i)
        outputBuffer[i] = tempFrame[i] / SCALE_FACTOR;
}


RNNFilter::~RNNFilter() {
    rnnoise_destroy(this->filterDenoiseState);
}


