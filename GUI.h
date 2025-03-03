//
// Created by Akoji Timothy on 03/03/2025.
//

#ifndef GUI_H
#define GUI_H

#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_Window.H>

#include "AudioRouter.h"

class MicrophoneSelect: public Fl_Choice {
public:
    MicrophoneSelect(
        const int x, const int y, const char* label, const std::vector<Microphone> &choices,
        const int defaultChoice = -1, const int w = 200, const int h = 25
        )
    : Fl_Choice(x, y, w, h, label) {
        this->align(FL_ALIGN_TOP);

        for (const auto& choice: choices) {
            this->add(choice.deviceInfo.name);
        }
        if (defaultChoice > -1)
        this->value(defaultChoice);
    };

    ~MicrophoneSelect() = default;
};

class NoiseCancellerGUI: public Fl_Window {
private:
    int padX, padY;
    MicrophoneSelect *inputMicSelect, *outputMicSelect;
    Fl_Button *captureBtn;

    PortAudioRouter paRouter;

    std::vector<Microphone> inputMics = PortAudioRouter::listAvailableInputMicrophones();
    std::vector<Microphone> outputMics = PortAudioRouter::listAvailableRoutableMicrophones();

    bool isCapturing = false;
public:
    NoiseCancellerGUI();

    static int run();
    static void buttonCallback(Fl_Widget*, void* user_data) {
        static_cast<NoiseCancellerGUI*>(user_data)->onBtnClick();
    }

    void onBtnClick();

    ~NoiseCancellerGUI();
};



#endif //GUI_H
