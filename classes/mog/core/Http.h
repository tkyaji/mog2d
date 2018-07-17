#ifndef Http_h
#define Http_h

#include <string>
#include <functional>
#include <unordered_map>
#include "mog/core/PubSub.h"

namespace mog {
    class Http {
    public:
        enum class Method {
            Get,
            Post,
        };
        
        class Response {
        public:
            int statusCode = 0;
            std::string errorDescription;
            std::shared_ptr<mog::ByteArray> data;
        };

        class Request {
        public:
            std::string url;
            Method method = Method::Get;
            int timeout = 0;
            std::unordered_map<std::string, std::string> params;

            Request(std::string url, Method method = Method::Get, int timeout = 0);
            Request(std::string url, std::unordered_map<std::string, std::string> params, Method method = Method::Get, int timeout = 0);
        };

        static void request(const Http::Request &req, std::function<void(const Http::Response &res)> callback = nullptr);
    };
}

#endif /* Http_h */
