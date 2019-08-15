#include "mog/Constants.h"
#include "mog/core/Http.h"
#include "mog/core/HttpNative.h"
#include "mog/core/Engine.h"

using namespace mog;


Http::Request::Request(std::string url, Method method) {
    this->url = url;
    this->method = method;
}

void Http::Request::setHeaders(const std::unordered_map<std::string, std::string> &headers) {
    this->headers = headers;
}

void Http::Request::addHeader(std::string name, std::string value) {
    this->headers[name] = value;
}

void Http::Request::setTimeout(int timeout) {
    this->timeout = timeout;
}

void Http::Request::setBody(std::string body) {
    auto byteArr = String::create(body)->toByteArray();
    this->setBody(byteArr);
}

void Http::Request::setBody(const std::shared_ptr<ByteArray> &body) {
    this->body = body;
}


void Http::request(const Http::Request &req, std::function<void(const Http::Response &res)> callback) {
    HttpNative::request(req, callback);
}
