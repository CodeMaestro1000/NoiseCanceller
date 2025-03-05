//
// Created by Akoji Timothy on 03/03/2025.
//
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>


#include "GUI.h"

#define WINDOW_W 480
#define WINDOW_H 320
#define MAX_VOLUME 100

MicrophoneSelect::MicrophoneSelect(
        const int x, const int y, const char* label, const std::vector<Microphone> &choices,
        const int defaultChoice, const int w, const int h
        )
        : Fl_Choice(x, y, w, h, label)
{
    this->labelcolor(fl_rgb_color(255, 255, 255));
    this->color(fl_rgb_color(220, 220, 220));
    this->color2(fl_rgb_color(13, 125, 181));
    this->align(FL_ALIGN_TOP);

    for (const auto& choice: choices) {
        this->add(choice.deviceInfo.name);
    }
    if (defaultChoice > -1)
        this->value(defaultChoice);
}

VolumeBar::VolumeBar(const int x, const int y, const char* label, const int w, const int h)
    :Fl_Progress(x, y, w, h, label)
{
    this->align(FL_ALIGN_LEFT);
    this->minimum(0);
    this->maximum(100);
    this->labelcolor(fl_rgb_color(200, 200, 200));
    this->color2(fl_rgb_color(13, 125, 181));
}

void NoiseCancellerGUI::timeoutCallback(void *userData) {
    auto gui = static_cast<NoiseCancellerGUI*>(userData);
    gui->updateVolumeBar();
    Fl::repeat_timeout(0.1, timeoutCallback, gui);
}


NoiseCancellerGUI::NoiseCancellerGUI(): Fl_Window(WINDOW_W, WINDOW_H, "Noise Canceller")
{
    this->padX = 10;
    this->padY = 60;

    this->color(fl_rgb_color(66, 66, 66));

    this->inputMicSelect = new MicrophoneSelect(0 + this->padX, 0 + this->padY, "Input", this->inputMics, 0);
    this->outputMicSelect = new MicrophoneSelect(
        WINDOW_W - padX - 200, 0 + this->padY, "Output", this->outputMics, 0
        );

    this->inputVolumeBar = new VolumeBar(10 + this->padX, 95 + this->padY, "In", 190);
    this->outputVolumeBar = new VolumeBar(WINDOW_W - padX - 200, 95 + this->padY, "Out", 190);

    this->captureBtn = new Fl_Button(180 + this->padX, 160 + this->padY, 100, 50, "Capture");
    this->captureBtn->color(fl_rgb_color(13, 125, 181));
    this->captureBtn->labelcolor(fl_rgb_color(255, 255, 255));
    this->captureBtn->callback(buttonCallback, this);

    this->end();
    this->Fl_Window::show();
}

int NoiseCancellerGUI::run() {
    return Fl::run();
}

void NoiseCancellerGUI::onBtnClick() {
    if (!isCapturing) {
        this->paRouter.selectInputDevice(this->inputMicSelect->value());
        this->paRouter.selectOutputDevice(this->outputMicSelect->value());
        this->paRouter.startCapture();

        Fl::add_timeout(0.1, timeoutCallback, this);
    }
    else {
        this->paRouter.stopCapture();
        Fl::remove_timeout(timeoutCallback, this);
    }

    // flip bit and update button text
    isCapturing = !isCapturing;

    if (isCapturing) {
        this->captureBtn->label("Stop");
        this->captureBtn->color(fl_rgb_color(181, 125, 13));
    }

    else {
        this->captureBtn->label("Capture");
        this->captureBtn->color(fl_rgb_color(13, 125, 181));
    }
}

void NoiseCancellerGUI::updateVolumeBar() const {
    auto [in, out] = paRouter.getSignalVolumes();
    // get relative values of volume bar as ints
    auto inputVolume = in * MAX_VOLUME;
    auto outputVolume = out * MAX_VOLUME;

    this->inputVolumeBar->value(inputVolume);
    this->outputVolumeBar->value(outputVolume);
}



NoiseCancellerGUI::~NoiseCancellerGUI() {
    // Fl_Window will destroy all child widgets
}

