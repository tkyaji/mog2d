#ifndef MacHelper_h
#define MacHelper_h

#import "mog/core/Engine.h"
#import <Cocoa/Cocoa.h>

#define MOG_VIEW_CONTROLLER "MogViewController"
#define MOG_VIEW "MogView"
#define MOG_ENGINE_CONTROLLER "MogEngineController"

namespace mog {
    class MacHelper {
    public:
        static NSViewController *getNSViewController() {
            return (__bridge UIViewController *)Engine::getInstance()->getNativeObject(MOG_VIEW_CONTROLLER)->getObject();
        }

        static shared_ptr<NativeObject> getNSViewControllerNO() {
            return Engine::getInstance()->getNativeObject(MOG_VIEW_CONTROLLER);
        }

        static NSView *getNSView() {
            return (__bridge NSView *)Engine::getInstance()->getNativeObject(MOG_VIEW)->getObject();
        }
        
        static shared_ptr<NativeObject> getNSViewNO() {
            return Engine::getInstance()->getNativeObject(MOG_VIEW);
        }
    };
}

#endif /* MacHelper_h */
