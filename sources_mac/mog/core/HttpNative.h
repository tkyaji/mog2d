#ifndef HttpNative_h
#define HttpNative_h

#include <string>
#include <functional>
#include <unordered_map>
#include "mog/core/Http.h"
#include "mog/core/PubSub.h"

namespace mog {
    class HttpNative {
        friend class Http;
    public:
        static void request(const Http::Request &req, std::function<void(const Http::Response &res)> callback);
    };
}

#endif /* HttpNative_h */
