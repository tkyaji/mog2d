#import <Foundation/Foundation.h>
#import "mog/core/HttpNative.h"

using namespace mog;

@interface HttpRequest : NSObject<NSURLConnectionDelegate> {
}

+ (void)request:(Http::Request)req callback:(std::function<void(const Http::Response &res)>)callback;

+ (NSString *)buildParamsString:(std::unordered_map<std::string, std::string>)params;

@end

@implementation HttpRequest

+ (void)request:(Http::Request)req callback:(std::function<void(const Http::Response &res)>)callback {
    
    NSString *methodStr = @"GET";
    if (req.method == Http::Method::Post) {
        methodStr = @"POST";
    }
    
    NSMutableURLRequest *urlReq = [NSMutableURLRequest new];
    [urlReq setHTTPMethod:methodStr];
    [urlReq setURL:[NSURL URLWithString:[NSString stringWithUTF8String:req.url.c_str()]]];
    [urlReq setCachePolicy:NSURLRequestReloadIgnoringLocalCacheData];
    [urlReq setTimeoutInterval:req.timeout];
    [urlReq setHTTPShouldHandleCookies:NO];
    
    for (auto &pair : req.headers) {
        NSString *name = [NSString stringWithUTF8String:pair.first.c_str()];
        NSString *value = [NSString stringWithUTF8String:pair.second.c_str()];
        [urlReq addValue:value forHTTPHeaderField:name];
    }
    
    if (req.body) {
        NSData *data = [NSData dataWithBytes:req.body->getBytes() length:(NSUInteger)req.body->getLength()];
        [urlReq setHTTPBody:data];
    }
    
    NSURLSessionConfiguration *configuration = [NSURLSessionConfiguration defaultSessionConfiguration];
    NSURLSession *session = [NSURLSession sessionWithConfiguration:configuration];
    NSURLSessionDataTask *task = [session dataTaskWithRequest:urlReq completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
        dispatch_async(dispatch_get_main_queue(), ^{
            Http::Response httpRes;
            httpRes.statusCode = (int)[(NSHTTPURLResponse *)response statusCode];
            if (error) httpRes.errorDescription = error.description.UTF8String;
            if (data) httpRes.data = ByteArray::create((unsigned char *)data.bytes, (unsigned int )data.length, true);
            if (callback) callback(httpRes);
        });
    }];
    [task resume];
}

+ (NSString *)buildParamsString:(std::unordered_map<std::string, std::string>)params {
    NSMutableString *paramsStr = [NSMutableString new];
    for (auto &pair : params) {
        if (paramsStr.length > 0) {
            [paramsStr appendString:@"&"];
        }
        [paramsStr appendString:[NSString stringWithFormat:@"%s=%s", pair.first.c_str(), pair.second.c_str()]];
    }
    return paramsStr;
}

@end


void HttpNative::request(const Http::Request &req, std::function<void(const Http::Response &res)> callback) {
    [HttpRequest request:req callback:callback];
}
