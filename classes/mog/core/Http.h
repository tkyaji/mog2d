#ifndef Http_h
#define Http_h

#include <string>
#include <functional>
#include <unordered_map>
#include "mog/core/PubSub.h"

using namespace std;

struct http_t;

namespace mog {
    class Http {
    public:
        class Response;
        
        enum class Status {
            Complete,
            Error,
            Timeout,
        };
        
        
        class Request {
        public:
            friend class Http;
            string url;
            ~Request();
            void cancel();
            void setTimeout(float timeout);
            
        private:
            http_t *request = nullptr;
            unsigned int onUpdateFuncId = 0;
            function<void(const Bytes &bytes, Status status)> callback;
            float timeout = 0;
            long long startTime = 0;
            
            Request(string url, function<void(const Bytes &bytes, Status status)> callback);
            
            void sendGet();
            void invokeCallback(Status status);
        };
        
        
        static shared_ptr<Request> get(string url, function<void(const Bytes &bytes, Status status)> callback, bool requestContinuation = true);
        // TODO
        //static shared_ptr<Request> post(string url, function<void(const Bytes &bytes, Status status)> callback);
        
    private:
        static unordered_map<intptr_t, shared_ptr<Request>> requestPool;
    };
}

#endif /* Http_h */
