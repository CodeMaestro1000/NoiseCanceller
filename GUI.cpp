//
// Created by Akoji Timothy on 03/03/2025.
//
#include <iostream>
#include <string>
#include <vector>

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>


#include "GUI.h"

#define WINDOW_W 480
#define WINDOW_H 320

NoiseCancellerGUI::NoiseCancellerGUI(): Fl_Window(WINDOW_W, WINDOW_H, "Noise Canceller")
{
    this->padX = 10;
    this->padY = 40;


    this->inputMicSelect = new MicrophoneSelect(0 + this->padX, 0 + this->padY, "Input", this->inputMics);
    this->outputMicSelect = new MicrophoneSelect(
        WINDOW_W - padX - 200, 0 + this->padY, "Output", this->outputMics
        );

    this->captureBtn = new Fl_Button(180 + this->padX, 85 + this->padY, 100, 50, "Capture");
    this->captureBtn->callback(buttonCallback, this);


    this->end();
    this->Fl_Window::show();
}

int NoiseCancellerGUI::run() {
    return Fl::run();
}

void NoiseCancellerGUI::onBtnClick() {
    std::cout<<"Button clicked!"<< std::endl;
    std::cout<<"Input: "<<this->inputMicSelect->value()<<std::endl;
    std::cout<<"Output: "<<this->outputMicSelect->value()<<std::endl;

    if (!isCapturing) {
        this->paRouter.selectInputDevice(this->inputMicSelect->value());
        this->paRouter.selectOutputDevice(this->outputMicSelect->value());
        this->paRouter.startCapture();
    }
    else
        this->paRouter.stopCapture();

    // flip bit and update button text
    isCapturing = !isCapturing;

    if (isCapturing)
        this->captureBtn->label("Stop");
    else
        this->captureBtn->label("Capture");
}


NoiseCancellerGUI::~NoiseCancellerGUI() {
    std::cout<<"Destroying all" << std::endl;
    // Fl_Window will destroy all child widgets
}

