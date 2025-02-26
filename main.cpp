#include <csignal>
#include <iostream>
#include "AudioRouter.h"

volatile sig_atomic_t stop; // don't optimize!

void interruptHandler(int signum) {
    stop = 1;
}

int main() {
    signal(SIGINT, interruptHandler); // Capture CTRL + C to clean up before exiting.
    signal(SIGTERM, interruptHandler); // Capture button event in CLION
    PortAudioRouter paRouter;

    paRouter.selectInputDevice(1); // macbook default
    paRouter.selectOutputDevice(0); // blackhole

    paRouter.startCapture();
    while (!stop){}
    paRouter.stopCapture();

    return 0;
}
