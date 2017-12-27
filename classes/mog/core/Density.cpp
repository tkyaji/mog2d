#include "mog/core/Density.h"
#include "mog/core/Device.h"
#include <math.h>

using namespace std;
using namespace mog;

#ifdef MOG_IOS
vector<Density> Density::allDensities = {Density::x1_0, Density::x2_0, Density::x3_0};
#else
vector<Density> Density::allDensities = {Density::x1_0, Density::x1_5, Density::x2_0, Density::x3_0, Density::x4_0};
#endif

Density Density::getCurrent() {
    float deviceDensity = Device::getDeviceDensity();
    int scale = (int)ceil(deviceDensity * 10.0f);
    auto density = Density::x1_0;
    if (scale > 30) {
        density = Density::x4_0;
    } else if (scale > 20) {
        density = Density::x3_0;
    } else if (scale > 15) {
        density = Density::x2_0;
    } else if (scale > 10) {
        density = Density::x1_5;
    }
    return density;
}

Density::Density(int idx) {
    switch (idx) {
        case 0:
            this->value = 1.0f;
            this->directory = "@1x";
            break;
        case 1:
            this->value = 1.5f;
            this->directory = "@1_5x";
            break;
        case 2:
            this->value = 2.0f;
            this->directory = "@2x";
            break;
        case 3:
            this->value = 3.0f;
            this->directory = "@3x";
            break;
        case 4:
            this->value = 4.0f;
            this->directory = "@4x";
            break;
    }
    this->idx = idx;
}
