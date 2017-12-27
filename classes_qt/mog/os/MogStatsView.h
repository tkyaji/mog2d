#ifndef MogStatsView_h
#define MogStatsView_h

#include "mog/core/Engine.h"

namespace mog {
    class MogStatsView {
    public:
        static bool isEnabled();
        static void setStats(float delta, int drawCall, int instants);
        static void setEnable(bool enable);
        static void setAlignment(Alignment alignment);

    private:
        static bool enabled;
    };
}

#endif /* MogStatsView_h */
