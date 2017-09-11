//
//  EngineNative.h
//  Emo-x
//
//  Created by tkyaji on 2017/02/08.
//  Copyright © 2017年 tkyaji. All rights reserved.
//

#ifndef EngineNative_h
#define EngineNative_h

#include "mog/core/Engine.h"

namespace mog {
    class EngineNative {
    public:
//        static void setNativeData(Engine *engine, const shared_ptr<NativeData> &nativeData);
        static void initStatsView(Engine *engine);
        static void setStats(double delta, int drawCall, int instants, Engine *engine);
        static void setStatsViewEnable(Engine *engine, bool enable);
        static void setStatsViewAlignment(Engine *engine, Alignment alignment);
    };
}

#endif /* EngineNative_h */
