#import "Device.h"
#import <UIKit/UIKit.h>

using namespace mog;

float Device::getDeviceDensity() {
    return [UIScreen mainScreen].scale;
}
