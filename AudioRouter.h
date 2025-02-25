//
// Created by Akoji Timothy on 19/02/2025.
//

#ifndef AUDIOROUTER_H
#define AUDIOROUTER_H

#include <array>

#include "portaudio.h"
#include "Filters.h"

/*
 * AudioRouter interface.
 *
 */

class AudioRouter {
public:

    virtual void startCapture() = 0;
    virtual void stopCapture() = 0;
    static  void info() {
    };
    // Implementations of the AudioRouter interface must implement their own virtual destructor
    // This way, when the AudioRouter goes out of scope, then the correct destructor is called
    // otherwise, what happens is up to the compiler :(
    virtual ~AudioRouter(){};
};



class PortAudioRouter final : public AudioRouter {
private:
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    PaStream *audioStream = nullptr;

    int framesPerBuffer = 480;
    float sawToothValue = 0.0;
    bool is_active = false;

    float tmpBuf[480];
    RNNFilter noiseFilter;

public:
    PortAudioRouter();

    void startCapture() override;
    void stopCapture() override;

    friend int send(const void *, void *, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *);
    static void info();

    ~PortAudioRouter() override;
};



#endif //AUDIOROUTER_H
