//
// Created by Akoji Timothy on 19/02/2025.
//

#include "AudioRouter.h"
#include <iostream>

void configureDefaultAudioParams(PaStreamParameters &params, const PaDeviceIndex deviceIdx, const bool isInput) {
    const PaDeviceInfo *deviceInformation = Pa_GetDeviceInfo(deviceIdx);
    int numChannels;
    PaTime suggestedLatency;

    if (isInput)suggestedLatency = deviceInformation->defaultLowInputLatency;
    else suggestedLatency = deviceInformation->defaultLowOutputLatency;

    params.device = deviceIdx;
    params.channelCount = 1;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = suggestedLatency;
    params.hostApiSpecificStreamInfo = nullptr;
}

int send (
    const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData
    )
{
    const auto *inputData = static_cast<const float *>(inputBuffer);
    auto *outputData = static_cast<float *>(outputBuffer);
    auto *paRouter = static_cast<PortAudioRouter *>(userData);

    float gain = 5.0f;

    paRouter->noiseFilter.apply(inputData, paRouter->tmpBuf);

    for (int i = 0; i < framesPerBuffer; i++) {
        *outputData = gain * (paRouter->tmpBuf[i]);
        outputData++;
    }
    return 0;

}

PortAudioRouter::PortAudioRouter() {
    if (const PaError err = Pa_Initialize() != paNoError) {
        std::cerr << "Port Audio Initialization Failed. Error: " << Pa_GetErrorText(err) << std::endl;
    }
    // TODO: Add functionality for selecting input and output devices.
    const PaDeviceIndex inputMicrophone = 1; // Macbook mic
    const PaDeviceIndex outputMicrophone = 0; // blackhole mic
    configureDefaultAudioParams(this->inputParameters, inputMicrophone, true);
    configureDefaultAudioParams(this->outputParameters, outputMicrophone, false);

    // since it is routing, use the same number of channels for I/O
    this->outputParameters.channelCount = this->inputParameters.channelCount;
}

// # TODO: Add option to change input/output devices

void PortAudioRouter::startCapture() {
    if (this->is_active) throw std::runtime_error("Capture already in progress");

    const PaDeviceInfo *deviceInformation = Pa_GetDeviceInfo(this->inputParameters.device);
    const double sampleRate = deviceInformation->defaultSampleRate;

    PaError error = Pa_OpenStream(
        &(this->audioStream),
        &(this->inputParameters),
        &(this->outputParameters),
        sampleRate,
        this->framesPerBuffer,
        paClipOff,
        send,
        this
        );

    if (error != paNoError) {
        const std::string err_str = "Error opening stream: " + std::string(Pa_GetErrorText(error));
        throw std::runtime_error(err_str);
    }

    error = Pa_StartStream(this->audioStream);
    if (error != paNoError)  {
        const std::string err_str = "Error starting stream: " + std::string(Pa_GetErrorText(error));
        throw std::runtime_error(err_str);
    }
    this->is_active = true;
}

void PortAudioRouter::stopCapture() {
    if (!this->is_active) throw std::runtime_error("No capture in progress");
    const PaError error = Pa_StopStream(this->audioStream);
    if (error != paNoError) {
        const std::string err_str = "Error stopping stream: " + std::string(Pa_GetErrorText(error));
        throw std::runtime_error(err_str);
    }
}


void  PortAudioRouter::info() {
    std::cout<<"Backend: PortAudio"<<std::endl;
}

PortAudioRouter::~PortAudioRouter() {
    if (this->is_active) this->stopCapture();

    if (const PaError err = Pa_Terminate() != paNoError) {
        std::cerr << "Port Audio Termination Failed. Error: " << Pa_GetErrorText(err) << std::endl;
        throw std::runtime_error("Pa_Term failed");
    }

    this->is_active = false;
}





