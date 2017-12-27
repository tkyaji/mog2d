#import "Device.h"
#import <Cocoa/Cocoa.h>

using namespace mog;

float Device::getDeviceDensity() {
    return [NSScreen mainScreen].backingScaleFactor;
}
