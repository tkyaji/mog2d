#ifndef IOSHelper_h
#define IOSHelper_h

#import "mog/core/Engine.h"
#import <UIKit/UIKit.h>

#define MOG_VIEW_CONTROLLER "MogViewController"
#define MOG_VIEW "MogView"
#define MOG_ENGINE_CONTROLLER "MogEngineController"

namespace mog {
    class IOSHelper {
    public:
        static UIViewController *getUIViewController() {
            return (__bridge UIViewController *)Engine::getInstance()->getNativeObject(MOG_VIEW_CONTROLLER)->getObject();
        }

        static shared_ptr<NativeObject> getUIViewControllerNO() {
            return Engine::getInstance()->getNativeObject(MOG_VIEW_CONTROLLER);
        }

        static UIView *getUIView() {
            return (__bridge UIView *)Engine::getInstance()->getNativeObject(MOG_VIEW)->getObject();
        }
        
        static shared_ptr<NativeObject> getUIViewNO() {
            return Engine::getInstance()->getNativeObject(MOG_VIEW);
        }
    };
}

#endif /* IOSHelper_h */
