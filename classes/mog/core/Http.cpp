#include <stdlib.h>
#include <time.h>
#include <thread>
#include "mog/Constants.h"
#include "mog/core/Http.h"
#include "mog/core/Engine.h"
#define HTTP_IMPLEMENTATION
#include "mog/libs/http.h"

using namespace mog;

#pragma - Http::Request

Http::Request::Request(string url, function<void(const Bytes &bytes, Status status)> callback) {
    this->url = url;
    this->callback = callback;
}

Http::Request::~Request() {
    this->cancel();
}

void Http::Request::sendGet() {
    this->startTime = getTimestamp();
    /*
    this->onUpdateFuncId = Engine::getInstance()->registerOnUpdateFunc([this](unsigned int funcId) {
        if (!this->request) {
            this->request = http_get(this->url.c_str(), NULL);
            if (!this->request) return;
        }
        
        if (this->timeout > 0) {
            if (getTimestamp() - this->startTime > this->timeout * 1000000L) {
                this->invokeCallback(Http::Status::Timeout);
                this->cancel();
                return;
            }
        }
        
        http_status_t status = http_process(request);
        if (status == HTTP_STATUS_PENDING) {
            return;
        }
        
        if (status == HTTP_STATUS_FAILED) {
            LOGW("http-get error : %s", this->request->reason_phrase);
            this->invokeCallback(Http::Status::Error);
        } else {
            this->invokeCallback(Http::Status::Complete);
        }
        this->cancel();
    });
     */
}

void Http::Request::invokeCallback(Http::Status status) {
    if (!this->callback) return;
    auto bytes = Bytes((unsigned char *)this->request->response_data, (unsigned int)this->request->response_size);
    this->callback(bytes, status);
}

void Http::Request::cancel() {
    if (this->onUpdateFuncId > 0) {
//        Engine::getInstance()->removeOnUpdateFunc(this->onUpdateFuncId);
        this->onUpdateFuncId = 0;
    }
    if (this->request) {
        http_release(this->request);
    }
    this->request = nullptr;
    this->callback = nullptr;
    
    if (Http::requestPool.count((intptr_t)this) > 0) {
        Http::requestPool.erase((intptr_t)this);
    }
}

void Http::Request::setTimeout(float timeout) {
    this->timeout = timeout;
}


#pragma - Http

unordered_map<intptr_t, shared_ptr<Http::Request>> Http::requestPool;

shared_ptr<Http::Request> Http::get(string url, function<void(const Bytes &bytes, Status status)> callback, bool requestContinuation) {
    auto request = shared_ptr<Http::Request>(new Http::Request(url, callback));
    if (requestContinuation) {
        Http::requestPool[(intptr_t)request.get()] = request;
    }
    request->sendGet();
    return request;
}
