#include "mog/core/Density.h"
#include "mog/core/Device.h"
#include <math.h>

using namespace std;
using namespace mog;

const Density Density::x1_0 = Density(Type::x1_0);
const Density Density::x1_5 = Density(Type::x1_5);
const Density Density::x2_0 = Density(Type::x2_0);
const Density Density::x3_0 = Density(Type::x3_0);
const Density Density::x4_0 = Density(Type::x4_0);

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

Density::Density(Type type) {
    switch (type) {
        case Type::x1_0:
            this->value = 1.0f;
            this->directory = "@1x";
            break;
        case Type::x1_5:
            this->value = 1.5f;
            this->directory = "@1_5x";
            break;
        case Type::x2_0:
            this->value = 2.0f;
            this->directory = "@2x";
            break;
        case Type::x3_0:
            this->value = 3.0f;
            this->directory = "@3x";
            break;
        case Type::x4_0:
            this->value = 4.0f;
            this->directory = "@4x";
            break;
    }
    this->type = type;
}
