#import "MogEngineController.h"
#import "mog/core/Engine.h"
#import "app/App.h"
#import "mog/Constants.h"
#import "mog/core/NativeClass.h"
#import "mog/os/IOSHelper.h"

@implementation MogEngineController {
    MogViewController *_mogViewController;
    MogView *_mogView;
    CADisplayLink *_displayLink;
    mog::Engine *_engine;
    float _fps;
    BOOL _engineInitialized;
    map<unsigned int, mog::TouchInput> _touches;
}

- (instancetype)initWithMog:(MogViewController *)viewController view:(MogView *)view {
    self = [super init];
    if (!self) return self;
    
    _engine = mog::Engine::initInstance();
    _engine->setDisplaySize(mog::Size(view.glWidth, view.glHeight), view.contentScaleFactor);
    _engine->setScreenSizeBasedOnHeight(BASE_SCREEN_HEIGHT);
    _mogViewController = viewController;
    _mogView = view;
    
    _engine->setNativeObject(MOG_VIEW_CONTROLLER, mog::NativeObject::create((__bridge void *)viewController));
    _engine->setNativeObject(MOG_VIEW, mog::NativeObject::create((__bridge void *)view));
    _engine->setNativeObject(MOG_ENGINE_CONTROLLER, mog::NativeObject::create((__bridge void *)self));
    
    auto app = shared_ptr<mog::App>(new mog::App());
    _engine->setApp(app);
    
    _fps = DEFAULT_FPS;
    
    return self;
}

- (void)startDraw {
    if (_displayLink) return;
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
    if ([CADisplayLink instancesRespondToSelector:@selector(setPreferredFramesPerSecond:)]) {
        [_displayLink setPreferredFramesPerSecond:(_fps)];
    } else {
#if __IPHONE_OS_VERSION_MAX_ALLOWED >= __IPHONE_10_0
        _displayLink.preferredFramesPerSecond = _fps;
#else
        _displayLink.frameInterval = 60 / _fps;
#endif
    }
    [_displayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
}

- (void)stopDraw {
    if (!_displayLink) return;
    [_displayLink invalidate];
    _displayLink = nil;
}

- (void)startEngine {
    [self startDraw];
    if (!_engineInitialized) {
        _engine->initEngine();
        _engineInitialized = YES;
    }
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
        [EAGLContext setCurrentContext:_mogView.glContext];
        
        mog::Engine::getInstance()->onDrawFrame(_touches);
        _touches.clear();
        
        glBindRenderbufferOES(GL_RENDERBUFFER_OES, _mogView.colorBuffer);
        [_mogView.glContext presentRenderbuffer:GL_RENDERBUFFER_OES];
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
