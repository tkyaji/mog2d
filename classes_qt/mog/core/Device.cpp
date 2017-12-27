#include "Device.h"
#include <QApplication>
#include <QScreen>

using namespace mog;

float Device::getDeviceDensity() {
    return QApplication::primaryScreen()->devicePixelRatio();
}
