#ifndef app_App_h
#define app_App_h

#include "mog/mog.h"

namespace mog {
    class App : public AppBase {
    public:
        void onLoad() override;
    };
}

#endif /* app_App_h */
