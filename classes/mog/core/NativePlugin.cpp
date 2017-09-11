#include "mog/core/NativePlugin.h"
#include "mog/core/Data.h"

using namespace mog;

# pragma - NativePlugin

NativePlugin::NativePlugin(string pluginClassName) {
    this->pluginClassName = pluginClassName;
}


# pragma - NativeCallbackManager

NativeCallbackManager *NativeCallbackManager::instance = nullptr;

NativeCallbackManager *NativeCallbackManager::initialize() {
    NativeCallbackManager::instance = new NativeCallbackManager();
    return NativeCallbackManager::instance;
}

NativeCallbackManager *NativeCallbackManager::getInstance() {
    return NativeCallbackManager::instance;
}

void NativeCallbackManager::addNativeCallback(function<void(const Dictionary &params)> callback, const Dictionary &params) {
    lock_guard<mutex> lock(this->mtx);
    pair<function<void(const Dictionary &params)>, Dictionary> pair(callback, params);
    this->callbackCaches.emplace_back(pair);
    this->hasCallback = true;
}

void NativeCallbackManager::invokeCallback() {
    if (!this->hasCallback) return;
    
    lock_guard<mutex> lock(this->mtx);
    for (auto &pair : this->callbackCaches) {
        pair.first(pair.second);
    }
    this->callbackCaches.clear();
    this->hasCallback = false;
}
