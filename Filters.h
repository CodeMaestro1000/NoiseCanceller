//
// Created by Akoji Timothy on 22/02/2025.
//

#ifndef FILTERS_H
#define FILTERS_H

#include <rnnoise.h>

class Filter {
public:
    virtual void apply(const float *inputBuffer, float *outputBuffer) = 0;
    virtual ~Filter() = default;
};


class RNNFilter final: public Filter {
private:
    DenoiseState *filterDenoiseState = rnnoise_create(nullptr);

public:
    RNNFilter() = default;
    void apply(const float *inputBuffer, float *outputBuffer) override;
    ~RNNFilter() override;
};



#endif //FILTERS_H
