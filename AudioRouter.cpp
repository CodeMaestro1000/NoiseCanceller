//
// Created by Akoji Timothy on 19/02/2025.
//
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

#include "AudioRouter.h"

#define NUM_CHANNELS 1
#define MAX_AUDIO_VOLUME 1 // max value of calculated signal volume

static float max(const float a, const float b) {
    return a > b ? a : b;
}

const PaDeviceInfo* configureDefaultAudioParams(PaStreamParameters &params, const PaDeviceIndex deviceIdx, const bool isInput) {
    const PaDeviceInfo *deviceInformation = Pa_GetDeviceInfo(deviceIdx);
    if (deviceInformation == nullptr) throw std::runtime_error("Invalid Device for selection");

    PaTime suggestedLatency;

    if (isInput) {
        if (deviceInformation->maxInputChannels < 1)
            throw std::runtime_error(std::string("No input channels for device!"));

        suggestedLatency = deviceInformation->defaultLowInputLatency;
    }
    else {
        if (deviceInformation->maxOutputChannels < 1)
            throw std::runtime_error(std::string("No output channels for device!"));

        suggestedLatency = deviceInformation->defaultLowOutputLatency;
    }

    params.device = deviceIdx;
    params.channelCount = NUM_CHANNELS;
    params.sampleFormat = paFloat32;
    params.suggestedLatency = suggestedLatency;
    params.hostApiSpecificStreamInfo = nullptr;

    return deviceInformation; // DO NOT ATTEMPT TO FREE, leave to PortAudio
}

int send (
    const void *inputBuffer, void *outputBuffer, unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData
    )
{
    const auto *inputData = static_cast<const float *>(inputBuffer);
    auto *outputData = static_cast<float *>(outputBuffer);
    auto *paRouter = static_cast<PortAudioRouter *>(userData);

    // denoise if there is input, else fill output with silence
    if (inputBuffer) paRouter->noiseFilter.apply(inputData, outputData);
    else std::fill_n(outputData, framesPerBuffer, 0.0f);

    paRouter->inputSignal = inputData;
    paRouter->outputSignal = outputData;
    return paContinue;
}

PortAudioRouter::PortAudioRouter() {
    if (const PaError err = Pa_Initialize() != paNoError) {
        std::cerr << "Port Audio Initialization Failed. Error: " << Pa_GetErrorText(err) << std::endl;
    }

    std::vector<Microphone> availableMics = listAvailableInputMicrophones();
    if (availableMics.empty())
        throw std::runtime_error("No available input microphones found. Check that devices are connected");

    configureDefaultAudioParams(this->inputParameters, availableMics[0].deviceID, true);
    this->inputMic = availableMics[0];

    availableMics = listAvailableRoutableMicrophones();
    if (availableMics.empty())
        throw std::runtime_error("No available routable microphones found. Check that devices are connected or install a virtual mic like Blackhole");

    configureDefaultAudioParams(this->outputParameters, availableMics[0].deviceID, false);
    this->outputMic = availableMics[0];

    std::clog << "Port Audio initialized! Now running, use CTRL + C to terminate. "<<std::endl;
}

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

    std::clog << "Capture started. Routing " << this->inputMic.deviceInfo.name << " => " <<this->outputMic.deviceInfo.name<< std::endl;
}

void PortAudioRouter::stopCapture() {
    if (!this->is_active) throw std::runtime_error("No capture in progress");
    const PaError error = Pa_StopStream(this->audioStream);
    if (error != paNoError) {
        const std::string err_str = "Error stopping stream: " + std::string(Pa_GetErrorText(error));
        throw std::runtime_error(err_str);
    }
    this->is_active = false;

    this->inputSignal = nullptr;
    this->outputSignal = nullptr;

    std::clog << std::endl; // use this so that a new line is shown after signal viz
    std::clog << "Capture ended." << std::endl;
}

std::vector<Microphone> PortAudioRouter::listAvailableInputMicrophones() {
    int numDevices = Pa_GetDeviceCount();
    std::vector<Microphone> microphoneInfo;

    for (auto i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);

        if (deviceInfo->maxInputChannels > 0) {
            Microphone mic;
            mic.deviceID = i;
            mic.deviceInfo = *deviceInfo;
            microphoneInfo.push_back((mic));
        }
    }

    return microphoneInfo; // we return by value since this will be moved on return from C++ 11
}

std::vector<Microphone> PortAudioRouter::listAvailableRoutableMicrophones() {
    int numDevices = Pa_GetDeviceCount();
    std::vector<Microphone> microphoneInfo;

    for (auto i = 0; i < numDevices; i++) {
        const PaDeviceInfo *deviceInfo = Pa_GetDeviceInfo(i);

        if (deviceInfo->maxInputChannels > 0 && deviceInfo->maxOutputChannels > 0) {
            Microphone mic;
            mic.deviceID = i;
            mic.deviceInfo = *deviceInfo;
            microphoneInfo.push_back((mic));
        }
    }

    return microphoneInfo; // we return by value since this will be moved on return from C++ 11
}


void PortAudioRouter::selectInputDevice(PaDeviceIndex deviceIdx) {
    if (this->is_active) this->stopCapture();

    const PaDeviceInfo *deviceInfo = configureDefaultAudioParams(this->inputParameters, deviceIdx, true);
    this->inputMic.deviceID = deviceIdx;
    this->inputMic.deviceInfo = *deviceInfo;
}

void PortAudioRouter::selectOutputDevice(PaDeviceIndex deviceIdx) {
    if (this->is_active) this->stopCapture();

    const PaDeviceInfo *deviceInfo = configureDefaultAudioParams(this->outputParameters, deviceIdx, false);
    this->outputMic.deviceID = deviceIdx;
    this->outputMic.deviceInfo = *deviceInfo;
}

float PortAudioRouter::computeSampleVolume(const float *sample) const {
    // volume is the max of the amplitude (readings in the sample) in either direction: +ve or -ve
    // Why? Because the digital samples consist of +ve and -ve vals. These rep the flunctuation of
    // natural sound waves

    if (sample == nullptr) return 0.0;

    float volume = 0.0;
    for (int i = 0; i < this->framesPerBuffer; i++) {
        volume = max(volume, std::abs(sample[i]));
    }
    return volume; // ranges from 0 to 1. Original data ranges from -1 to +1.
}

Volumes PortAudioRouter::getSignalVolumes() const {
    Volumes v;
    v.in = this->computeSampleVolume(this->inputSignal);
    v.out = this->computeSampleVolume(this->outputSignal);

    return v;
}

void  PortAudioRouter::info() {
    std::cout<<"Backend: PortAudio"<<std::endl;
}

PortAudioRouter::~PortAudioRouter() {
    if (this->is_active) this->stopCapture();

    if (const PaError err = Pa_Terminate() != paNoError) {
        std::cerr << "Port Audio Termination Failed. Error: " << Pa_GetErrorText(err) << std::endl;
    }

    this->is_active = false;
    std::clog<<"Goodbye :)" <<std::endl;
}





