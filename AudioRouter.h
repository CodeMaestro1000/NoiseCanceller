//
// Created by Akoji Timothy on 19/02/2025.
//

#ifndef AUDIOROUTER_H
#define AUDIOROUTER_H

#include <vector>
#include "portaudio.h"
#include "Filters.h"

struct Microphone {
    int deviceID;
    PaDeviceInfo deviceInfo;
};

class PortAudioRouter {
private:
    PaStreamParameters inputParameters;
    PaStreamParameters outputParameters;
    PaStream *audioStream = nullptr;

    Microphone inputMic, outputMic;

    int framesPerBuffer = 480;
    float sawToothValue = 0.0;
    bool is_active = false;

    RNNFilter noiseFilter;

public:
    PortAudioRouter();

    void startCapture();
    void stopCapture();
    void selectInputDevice(PaDeviceIndex deviceIdx);
    void selectOutputDevice(PaDeviceIndex deviceIdx);
    // read-only funcs
    float computeSampleVolume(const float *sample) const;
    void visualizeSignals(const float *inputSignal, const float *outputSignal) const;

    static std::vector<Microphone> listAvailableInputMicrophones();
    static std::vector<Microphone> listAvailableRoutableMicrophones();
    static void info();


    friend int send(const void *, void *, unsigned long, const PaStreamCallbackTimeInfo*, PaStreamCallbackFlags, void *);

    ~PortAudioRouter();
};



#endif //AUDIOROUTER_H
