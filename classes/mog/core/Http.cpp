#include "mog/Constants.h"
#include "mog/core/Http.h"
#include "mog/core/HttpNative.h"
#include "mog/core/Engine.h"

using namespace mog;


Http::Request::Request(std::string url, Method method, int timeout) {
    this->url = url;
    this->method = method;
    this->timeout = timeout;
}

Http::Request::Request(std::string url, std::unordered_map<std::string, std::string> params, Method method, int timeout) {
    this->url = url;
    this->params = params;
    this->method = method;
    this->timeout = timeout;
};

void Http::request(const Http::Request &req, std::function<void(const Http::Response &res)> callback) {
    HttpNative::request(req, callback);
}
