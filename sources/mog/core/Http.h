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
            Request(std::string url, Method method = Method::Get);
            
            void setHeaders(const std::unordered_map<std::string, std::string> &headers);
            void addHeader(std::string name, std::string value);
            void setTimeout(int timeout);
            void setBody(std::string body);
            void setBody(const std::shared_ptr<ByteArray> &body);

            std::string url;
            Method method = Method::Get;
            int timeout = 0;
            std::shared_ptr<ByteArray> body = nullptr;
            std::unordered_map<std::string, std::string> headers;
        };

        static void request(const Http::Request &req, std::function<void(const Http::Response &res)> callback = nullptr);
    };
}

#endif /* Http_h */
