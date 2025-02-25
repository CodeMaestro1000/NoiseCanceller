#include "AudioRouter.h"

int main() {

    PortAudioRouter paRouter;

    paRouter.startCapture();
    Pa_Sleep(60 * 1000);
    paRouter.stopCapture();

    return 0;
}
