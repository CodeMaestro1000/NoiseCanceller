//
// Created by Akoji Timothy on 03/03/2025.
//

#ifndef GUI_H
#define GUI_H

#include <atomic>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Window.H>

#include "AudioRouter.h"

class MicrophoneSelect: public Fl_Choice {
public:
    MicrophoneSelect(
        int x, int y, const char* label, const std::vector<Microphone> &choices,
        int defaultChoice = -1, int w = 200, int h = 25
        );
    ~MicrophoneSelect() override = default;
};

class VolumeBar: public Fl_Progress {
public:
    VolumeBar(int x, int y, const char* label, int w = 200, int h = 10);
    ~VolumeBar() override = default;
};

class NoiseCancellerGUI: public Fl_Window {
private:
    int padX, padY;
    MicrophoneSelect *inputMicSelect, *outputMicSelect;
    Fl_Button *captureBtn;
    VolumeBar *inputVolumeBar, *outputVolumeBar;

    PortAudioRouter paRouter;

    std::vector<Microphone> inputMics = PortAudioRouter::listAvailableInputMicrophones();
    std::vector<Microphone> outputMics = PortAudioRouter::listAvailableRoutableMicrophones();

    std::atomic<bool> isCapturing = false;

    void updateVolumeBar() const;

    static void timeoutCallback(void *);
public:
    NoiseCancellerGUI();

    static int run();
    static void buttonCallback(Fl_Widget*, void* user_data) {
        static_cast<NoiseCancellerGUI*>(user_data)->onBtnClick();
    }

    void onBtnClick();

    ~NoiseCancellerGUI() override;
};


#endif //GUI_H
