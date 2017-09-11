#ifndef App_h
#define App_h

#include "mog/mog.h"

namespace mog {
    class App : public AppBase {
    public:
        void onLoad() override;
    };
}

#endif /* App_h */
