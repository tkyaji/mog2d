#import "MogEngineController.h"
#import "mog/core/Engine.h"
#import "app/App.h"
#import "mog/Constants.h"
#import "mog/core/mogmalloc.h"
#import "mog/os/IOSHelper.h"
#ifdef SCRIPT_BINDNG_ENGINE_HEADER
#import SCRIPT_BINDNG_ENGINE_HEADER
#endif

@implementation MogEngineController {
    MogViewController *_mogViewController;
    MogView *_mogView;
    UIView *_launchScreenView;
    CADisplayLink *_displayLink;
    std::shared_ptr<mog::Engine> _engine;
    float _fps;
    std::map<unsigned int, mog::TouchInput> _touches;
}

- (instancetype)initWithMog:(MogViewController *)viewController view:(MogView *)view {
    self = [super init];
    if (!self) return self;
    
    mogmalloc_initialize();
#ifdef ENABLE_SCRIPT_BINDNG
    _engine = SCRIPT_BINDNG_ENGINE_CLASS::create();
#else
    _engine = mog::Engine::create(std::make_shared<mog::App>());
#endif
    _engine->setDisplaySize(mog::Size(view.glWidth, view.glHeight), mog::Size(view.frame.size.width, view.frame.size.height));
    _engine->resetScreenSize();
    _mogViewController = viewController;
    _mogView = view;
    
    mog::IOSHelper::mogViewController = (__bridge void *)viewController;
    mog::IOSHelper::mogView = (__bridge void *)view;
    mog::IOSHelper::mogEngineController = (__bridge void *)self;
    
    _fps = DEFAULT_FPS;
    
    UIStoryboard *storyboard = [UIStoryboard storyboardWithName:@"LaunchScreen" bundle:nil];
    _launchScreenView = storyboard.instantiateInitialViewController.view;
    [view addSubview:_launchScreenView];
    
    return self;
}

- (void)startDraw {
    if (_displayLink) return;
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
    if ([CADisplayLink instancesRespondToSelector:@selector(setPreferredFramesPerSecond:)]) {
        [_displayLink setPreferredFramesPerSecond:(_fps)];
    } else {
        _displayLink.frameInterval = 60 / _fps;
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
    _engine->startEngine();
}

- (void)stopEngine {
    _engine->stopEngine();
    [self stopDraw];
}

- (void)terminateEngine {
    [self stopEngine];
    _engine = nullptr;
    mogmalloc_finalize();
}

- (void)didReceiveMemoryWarning {
    _engine->onLowMemory();
}

- (void)setFps:(float)fps {
    _fps = fps;
    [self stopDraw];
    [self startDraw];
}

- (void)drawFrame {
    @synchronized (self) {
        if (!_displayLink) return;
        
        [EAGLContext setCurrentContext:_mogView.glContext];
        
        _engine->onDrawFrame(_touches);
        [self clearTouchEvent];
        
        [_mogView.glContext presentRenderbuffer:GL_RENDERBUFFER];
        
        if (_launchScreenView) {
            [_launchScreenView removeFromSuperview];
            _launchScreenView = nil;
        }
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

- (void)clearTouchEvent {
    std::vector<unsigned int> touchIdToRemove;
    for (auto &pair : _touches) {
        if (pair.second.action == mog::TouchAction::TouchUp || pair.second.action == mog::TouchAction::TouchDownUp) {
            touchIdToRemove.emplace_back(pair.first);
        } else {
            pair.second.action = mog::TouchAction::TouchMove;
        }
    }
    for (unsigned int touchId : touchIdToRemove) {
        _touches.erase(touchId);
    }
}

@end
