#include <FL/Fl.H>
#include "GUI.h"

int main() {
    Fl::scheme("gtk+");
    NoiseCancellerGUI gui = NoiseCancellerGUI();
    return Fl::run();
}