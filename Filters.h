//
// Created by Akoji Timothy on 22/02/2025.
//

#ifndef FILTERS_H
#define FILTERS_H

#include <rnnoise.h>

class RNNFilter {
private:
    DenoiseState *filterDenoiseState = rnnoise_create(nullptr);

public:
    RNNFilter() = default;
    void apply(const float *inputBuffer, float *outputBuffer);
    ~RNNFilter();
};


#endif //FILTERS_H
