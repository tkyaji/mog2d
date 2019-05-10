#include "mog/core/HttpNative.h"
#include "mog/core/NativeClass.h"
#include "mog/core/Engine.h"
#include "mog/os/AndroidHelper.h"

using namespace mog;

void HttpNative::request(const Http::Request &req, std::function<void(const Http::Response &res)> callback) {
    auto engine = Engine::getInstance();
    auto paramDict = Dictionary::create();
    for (auto &pair : req.params) {
        paramDict->put(pair.first, String::create(pair.second));
    }
    std::function<void(const std::shared_ptr<mog::List> &args)> func = [callback](const std::shared_ptr<mog::List> &args) {
        auto response = Http::Response();
        response.statusCode = args->at<Int>(0)->getValue();
        response.errorDescription = args->at<String>(1)->getValue();
        response.data = args->at<ByteArray>(2);
        callback(response);
    };
    NativeClass::create("org.mog2d.Http")->execute("request",
                                                   AndroidHelper::mogActivity,
                                                   String::create(req.url),
                                                   Int::create((int)req.method),
                                                   Int::create(req.timeout),
                                                   paramDict,
                                                   NativeObject::create(func));
}
