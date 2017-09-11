#ifndef NativePlugin_h
#define NativePlugin_h

#include <string>
#include <functional>
#include <mutex>
#include "mog/core/Data.h"

using namespace std;

namespace mog {
    class NativePlugin {
    public:
        NativePlugin(string pluginClassName);

        virtual void execute(string action) = 0;
        virtual void execute(string action, const Dictionary &params) = 0;

        virtual Dictionary executeWithReturn(string action) = 0;
        virtual Dictionary executeWithReturn(string action, const Dictionary &params) = 0;

        virtual void executeWithCallback(string action, function<void(const Dictionary &params)> callback) = 0;
        virtual void executeWithCallback(string action, const Dictionary &params, function<void(const Dictionary &params)> callback) = 0;

    protected:
        string pluginClassName;
    };
    
    
    class NativeCallbackManager {
    public:
        static NativeCallbackManager *initialize();
        static NativeCallbackManager *getInstance();
        
        void invokeCallback();
        void addNativeCallback(function<void(const Dictionary &params)> callback, const Dictionary &params);

    private:
        static NativeCallbackManager *instance;
        volatile bool hasCallback = false;
        mutex mtx;
        vector<pair<function<void(const Dictionary &params)>, Dictionary>> callbackCaches;
    };
}

#endif /* NativePlugin_h */
