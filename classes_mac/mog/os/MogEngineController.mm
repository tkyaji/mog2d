#import "MogEngineController.h"
#import "mog/core/Engine.h"
#import "app/App.h"
#import "mog/Constants.h"
#import "mog/core/NativeClass.h"
//#import "mog/os/IOSHelper.h"

@implementation MogEngineController {
    MogViewController *_mogViewController;
    MogView *_mogView;
    CVDisplayLinkRef _displayLink;
    mog::Engine *_engine;
    float _fps;
    map<unsigned int, mog::TouchInput> _touches;
}

- (instancetype)initWithMog:(MogViewController *)viewController view:(MogView *)view {
    self = [super init];
    if (!self) return self;
    
    _engine = mog::Engine::initInstance();
    _engine->setDisplaySize(mog::Size(view.glWidth, view.glHeight), view.scaleFactor);
    _engine->setScreenSizeBasedOnHeight(view.frame.size.height);
    _mogViewController = viewController;
    _mogView = view;
    
    // TODO
    /*
    _engine->setNativeObject(MOG_VIEW_CONTROLLER, mog::NativeObject::create((__bridge void *)viewController));
    _engine->setNativeObject(MOG_VIEW, mog::NativeObject::create((__bridge void *)view));
    _engine->setNativeObject(MOG_ENGINE_CONTROLLER, mog::NativeObject::create((__bridge void *)self));
     */
    
    _fps = DEFAULT_FPS;
    
    return self;
}

- (void)initEngine {
    _engine->initEngine(make_shared<mog::App>());
}

static CVReturn renderCallback(CVDisplayLinkRef displayLink,
                               const CVTimeStamp *inNow,
                               const CVTimeStamp *inOutputTime,
                               CVOptionFlags flagsIn,
                               CVOptionFlags *flagsOut,
                               void *displayLinkContext) {
    
    MogEngineController *engineController = (__bridge MogEngineController *)displayLinkContext;
    [engineController performSelectorOnMainThread:@selector(drawFrame) withObject:nil waitUntilDone:NO];
    
    return kCVReturnSuccess;
}

- (void)startDraw {
    if (_displayLink) return;
    
    CGLContextObj cglContext = _mogView.glContext.CGLContextObj;
    CGLPixelFormatObj cglPixelFormat = _mogView.glContext.pixelFormat.CGLPixelFormatObj;
    CVDisplayLinkCreateWithActiveCGDisplays(&_displayLink);
    CVDisplayLinkSetOutputCallback(_displayLink, &renderCallback, (__bridge void*)(self));
    CVDisplayLinkSetCurrentCGDisplayFromOpenGLContext(_displayLink, cglContext, cglPixelFormat);
    CVDisplayLinkStart(_displayLink);
}

- (void)stopDraw {
    CVDisplayLinkStop(_displayLink);
    _displayLink = nil;
}

- (void)startEngine {
    [self startDraw];
    _engine->startEngine();
}

- (void)stopEngine {
    _engine->stopEngine();
    [self stopDraw];
}

- (void)didReceiveMemoryWarning {
    _engine->onLowMemory();
}

- (void)terminateEngine {
    _engine->terminateEngine();
}

- (void)setFps:(float)fps {
    _fps = fps;
    [self stopDraw];
    [self startDraw];
}

- (void)drawFrame {
    @synchronized (self) {
        [_mogView.glContext lock];
        
        mog::Engine::getInstance()->onDrawFrame(_touches);
        _touches.clear();
        
        [_mogView.glContext flushBuffer];
        [_mogView.glContext unlock];
    }
}

- (void)fireTouchEvent:(unsigned int)touchId location:(CGPoint)location action:(TouchAction)action {
    @synchronized (self) {
        switch (action) {
            case TouchActionDown:
                _touches[touchId] = mog::TouchInput(mog::TouchAction::TouchDown, touchId, location.x, location.y);
                break;
                
            case TouchActionMove:
                if (_touches.count(touchId) == 0 || _touches[touchId].action == mog::TouchAction::TouchMove) {
                    _touches[touchId] = mog::TouchInput(mog::TouchAction::TouchMove, touchId, location.x, location.y);
                }
                break;
                
            case TouchActionUp:
                if (_touches.count(touchId) > 0 &&
                    (_touches[touchId].action == mog::TouchAction::TouchDown || _touches[touchId].action == mog::TouchAction::TouchDownUp)) {
                    _touches[touchId] = mog::TouchInput(mog::TouchAction::TouchDownUp, touchId, location.x, location.y);
                } else {
                    _touches[touchId] = mog::TouchInput(mog::TouchAction::TouchUp, touchId, location.x, location.y);
                }
                break;
        }
    }
    
}

@end
