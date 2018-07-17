#import <Foundation/Foundation.h>

typedef NS_ENUM (NSUInteger, TouchAction) {
    TouchActionDown,
    TouchActionUp,
    TouchActionMove,
};

@protocol MogTouchEventDelegate <NSObject>

@required

- (void)fireTouchEvent:(unsigned int)touchId location:(CGPoint)location action:(TouchAction)action;

@end
