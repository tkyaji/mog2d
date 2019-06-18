#import "mog/core/NativeClass.h"
#import <Foundation/Foundation.h>
#import <unordered_map>
#import "mog/Constants.h"

using namespace mog;

@interface NativeFunction : NSObject
- (instancetype)init __attribute__((unavailable("init is not available")));
- (instancetype)initWithFunction:(std::function<void(const std::shared_ptr<mog::List> &args)>)func;
- (void)invoke:(std::shared_ptr<List>)args;
@end

@implementation NativeFunction {
    std::function<void(const std::shared_ptr<mog::List> &args)> _func;
}
- (instancetype)initWithFunction:(std::function<void(const std::shared_ptr<mog::List> &args)>)func {
    self = [super init];
    _func = func;
    return self;
}
- (void)invoke:(std::shared_ptr<List>)args {
    return _func(args);
}
@end

static void toNativeDictionary(const std::shared_ptr<Dictionary> &dictionary, NSMutableDictionary *dict);
static void toNativeList(const std::shared_ptr<List> &list, NSMutableArray *nList);

static void toMogDictionary(NSDictionary *nDict, std::shared_ptr<Dictionary> &dict);
static void toMogList(NSArray *nList, std::shared_ptr<List> &list);


#pragma - NativeInvoker

class NativeInvoker {
public:
    NativeInvoker(NSProxy<NSObject> *target) {
        this->target = target;
    }
    
    std::shared_ptr<Data> execute(std::string methodName, const std::shared_ptr<List> &args, DataType retType) {
        SEL sel = NSSelectorFromString([NSString stringWithUTF8String:methodName.c_str()]);
        NSMethodSignature *sig = [this->target methodSignatureForSelector:sel];
        assert(sig);
        NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
        [inv setSelector:sel];
        for (int i = 0; i < args->size(); i++) {
            auto arg = args->at<Data>(i);
            this->setArgument(inv, arg, 2 + i);
        }
        [inv invokeWithTarget:this->target];
        return this->getReturnValue(inv, retType);
    }
    
    std::shared_ptr<mog::Data> getProperty(std::string propertyName, DataType retType) {
        id nsId = (id)this->target;
        id nsValue = [nsId valueForKey:[NSString stringWithUTF8String:propertyName.c_str()]];
        switch (retType) {
            case DataType::Int: {
                int i = [(NSNumber *)nsValue intValue];
                return Int::create(i);
            }
            case DataType::Long: {
                long l = [(NSNumber *)nsValue longValue];
                return Long::create(l);
            }
            case DataType::Float: {
                float f = [(NSNumber *)nsValue floatValue];
                return Float::create(f);
            }
            case DataType::Double: {
                double d = [(NSNumber *)nsValue doubleValue];
                return Double::create(d);
            }
            case DataType::Bool: {
                bool b = [(NSNumber *)nsValue boolValue];
                return Bool::create(b);
            }
            case DataType::String: {
                const char *s = ((NSString *)nsValue).UTF8String;
                return String::create(s);
            }
            case DataType::ByteArray: {
                NSData *data = (NSData *)nsValue;
                return ByteArray::create((unsigned char *)data.bytes, (unsigned int)data.length, true);
            }
            case DataType::Dictionary: {
                NSDictionary *nDict = (NSDictionary *)nsValue;
                auto dict = Dictionary::create();
                toMogDictionary(nDict, dict);
                return dict;
            }
            case DataType::List: {
                NSArray *nList = (NSArray *)nsValue;
                auto list = List::create();
                toMogList(nList, list);
                return list;
            }
            case DataType::NativeObject: {
                return NativeObject::create((void *)nsValue);
            }
            default:
                return nullptr;
        }
    }
    
    void setProperty(std::string propertyName, const std::shared_ptr<Data> &value) {
        id nsId = (id)this->target;
        NSString *key = [NSString stringWithUTF8String:propertyName.c_str()];
        id nsValue = nil;
        switch (value->type) {
            case DataType::Void: {
                break;
            }
            case DataType::Int: {
                int i = std::static_pointer_cast<Int>(value)->getValue();
                nsValue = [NSNumber numberWithInt:i];
                break;
            }
            case DataType::Long: {
                long long l = std::static_pointer_cast<Long>(value)->getValue();
                nsValue = [NSNumber numberWithLongLong:l];
                break;
            }
            case DataType::Float: {
                float f = std::static_pointer_cast<Float>(value)->getValue();
                nsValue = [NSNumber numberWithFloat:f];
                break;
            }
            case DataType::Double: {
                double d = std::static_pointer_cast<Double>(value)->getValue();
                nsValue = [NSNumber numberWithDouble:d];
                break;
            }
            case DataType::Bool: {
                bool b = std::static_pointer_cast<Bool>(value)->getValue();
                nsValue = [NSNumber numberWithBool:b];
                break;
            }
            case DataType::String: {
                std::string s = std::static_pointer_cast<String>(value)->getValue();
                nsValue = [NSString stringWithUTF8String:s.c_str()];
                break;
            }
            case DataType::List: {
                auto list = std::static_pointer_cast<List>(value);
                NSMutableArray *nList = [NSMutableArray new];
                toNativeList(list, nList);
                nsValue = nList;
                break;
            }
            case DataType::Dictionary: {
                auto dict = std::static_pointer_cast<Dictionary>(value);
                NSMutableDictionary *nDict = [NSMutableDictionary new];
                toNativeDictionary(dict, nDict);
                nsValue = nDict;
                break;
            }
            case DataType::ByteArray: {
                auto bytes = std::static_pointer_cast<ByteArray>(value);
                unsigned char *value = nullptr;
                unsigned int length = 0;
                bytes->getValue(&value, &length);
                NSData *data = [NSData dataWithBytes:value length:length];
                nsValue = data;
                break;
            }
            case DataType::NativeObject: {
                auto o = std::static_pointer_cast<NativeObject>(value);
                nsValue = (NSObject *)o->getValue();
            }
        }
        [nsId setValue:nsValue forKey:key];
    }

    
private:
    NSProxy<NSObject> *target;
    
    void setArgument(NSInvocation* inv, const std::shared_ptr<Data> &arg, int idx) {
        switch (arg->type) {
            case DataType::Void: {
                void *o = nil;
                [inv setArgument:o atIndex:idx];
                break;
            }
            case DataType::Int: {
                int i = std::static_pointer_cast<Int>(arg)->getValue();
                [inv setArgument:&i atIndex:idx];
                break;
            }
            case DataType::Long: {
                long long l = std::static_pointer_cast<Long>(arg)->getValue();
                [inv setArgument:&l atIndex:idx];
                break;
            }
            case DataType::Float: {
                float f = std::static_pointer_cast<Float>(arg)->getValue();
                [inv setArgument:&f atIndex:idx];
                break;
            }
            case DataType::Double: {
                double d = std::static_pointer_cast<Double>(arg)->getValue();
                [inv setArgument:&d atIndex:idx];
                break;
            }
            case DataType::Bool: {
                bool b = std::static_pointer_cast<Bool>(arg)->getValue();
                [inv setArgument:&b atIndex:idx];
                break;
            }
            case DataType::String: {
                std::string s = std::static_pointer_cast<String>(arg)->getValue();
                NSString *str = [NSString stringWithUTF8String:s.c_str()];
                [inv setArgument:&str atIndex:idx];
                break;
            }
            case DataType::List: {
                auto list = std::static_pointer_cast<List>(arg);
                NSMutableArray *nList = [NSMutableArray new];
                toNativeList(list, nList);
                [inv setArgument:&nList atIndex:idx];
                break;
            }
            case DataType::Dictionary: {
                auto dict = std::static_pointer_cast<Dictionary>(arg);
                NSMutableDictionary *nDict = [NSMutableDictionary new];
                toNativeDictionary(dict, nDict);
                [inv setArgument:&nDict atIndex:idx];
                break;
            }
            case DataType::ByteArray: {
                auto bytes = std::static_pointer_cast<ByteArray>(arg);
                unsigned char *value = nullptr;
                unsigned int length = 0;
                bytes->getValue(&value, &length);
                NSData *data = [NSData dataWithBytes:value length:length];
                [inv setArgument:&data atIndex:idx];
                break;
            }
            case DataType::NativeObject: {
                auto o = std::static_pointer_cast<NativeObject>(arg);
                NSObject *no = (NSObject *)o->getValue();
                [inv setArgument:no atIndex:idx];
            }
        }
    }
    
    std::shared_ptr<Data> getReturnValue(NSInvocation* inv, DataType retType) {
        switch (retType) {
            case DataType::Void: {
                return nullptr;
            }
            case DataType::Int: {
                int i = 0;
                [inv getReturnValue:&i];
                return Int::create(i);
            }
            case DataType::Long: {
                long long l = 0;
                [inv getReturnValue:&l];
                return Long::create(l);
            }
            case DataType::Float: {
                float f = 0.0f;
                [inv getReturnValue:&f];
                return Float::create(f);
            }
            case DataType::Double: {
                double d = 0.0;
                [inv getReturnValue:&d];
                return Double::create(d);
            }
            case DataType::Bool: {
                bool b = false;
                [inv getReturnValue:&b];
                return Bool::create(b);
            }
            case DataType::String: {
                NSString *s = nil;
                [inv getReturnValue:&s];
                if (s == nil) s = @"";
                return String::create(s.UTF8String);
            }
            case DataType::List: {
                NSArray *nList = nil;
                [inv getReturnValue:&nList];
                auto list = List::create();
                if (nList) {
                    toMogList(nList, list);
                }
                return list;
            }
            case DataType::Dictionary: {
                NSDictionary *nDict = nil;
                [inv getReturnValue:&nDict];
                auto dict = Dictionary::create();
                if (nDict) {
                    toMogDictionary(nDict, dict);
                }
                return dict;
            }
            case DataType::ByteArray: {
                NSData *data = nil;
                [inv getReturnValue:&data];
                if (data) {
                    return ByteArray::create((unsigned char *)data.bytes, (unsigned int)data.length, true);
                } else {
                    return ByteArray::create(nullptr, 0);
                }
            }
            case DataType::NativeObject: {
                NSObject *o = nil;
                [inv getReturnValue:&o];
                return NativeObject::create(o);
            }
        }
    }
};


#pragma - NativeClass

std::shared_ptr<NativeClass> NativeClass::create(std::string className) {
    return std::shared_ptr<NativeClass>(new NativeClass(className));
}

NativeClass::NativeClass(std::string className) {
    Class cls = NSClassFromString([NSString stringWithUTF8String:className.c_str()]);
    this->cls = (void *)cls;
}

NativeClass::~NativeClass() {
}

std::shared_ptr<NativeObject> NativeClass::newInstance() {
    Class cls = (Class)this->cls;
    NSObject *obj = [cls alloc];
    return NativeObject::create((void *)obj);
}

void NativeClass::execute(std::string methodName) {
    auto args = List::create();
    this->executeMain(methodName, args, DataType::Void);
}

void NativeClass::executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
    this->executeMain(methodName, args, DataType::Void);
}

std::shared_ptr<mog::Data> NativeClass::executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType) {
    Class cls = (Class)this->cls;
    return NativeInvoker(cls).execute(methodName, args, retType);
}

std::shared_ptr<mog::Data> NativeClass::getPropertyMain(std::string propertyName, DataType retType) {
    id nsObj = (id)this->cls;
    return NativeInvoker(nsObj).getProperty(propertyName, retType);
}

void NativeClass::setProperty(std::string propertyName, const std::shared_ptr<Data> &value) {
    id nsObj = (id)this->cls;
    NativeInvoker(nsObj).setProperty(propertyName, value);
}


#pragma - NativeObject

std::shared_ptr<NativeObject> NativeObject::create(void *value) {
    return std::shared_ptr<NativeObject>(new NativeObject(value));
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<Dictionary> &dict) {
    NSMutableDictionary *nsDict = [NSMutableDictionary new];
    toNativeDictionary(dict, nsDict);
    return create(nsDict);
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<List> &list) {
    NSMutableArray *nsArr = [NSMutableArray new];
    toNativeList(list, nsArr);
    return create(nsArr);
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<ByteArray> &bytes) {
    unsigned char *value = nullptr;
    unsigned int length = 0;
    bytes->getValue(&value, &length);
    NSData *data = [NSData dataWithBytes:value length:length];
    return NativeObject::create((void *)data);
}

std::shared_ptr<NativeObject> NativeObject::create(std::function<void(const std::shared_ptr<mog::List> &args)> func) {
    NativeFunction *nativeFunc = [[NativeFunction alloc] initWithFunction:func];
    return create(nativeFunc);
}

NativeObject::NativeObject(void *value) {
    [(NSObject *)value retain];
    this->value = value;
}

NativeObject::~NativeObject() {
    if (this->value) {
        [(NSObject *)this->value release];
    }
}

void *NativeObject::getValue() {
    return this->value;
}

void NativeObject::execute(std::string methodName) {
    auto args = List::create();
    this->executeMain(methodName, args, DataType::Void);
}

void NativeObject::executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
    this->executeMain(methodName, args, DataType::Void);
}

std::shared_ptr<mog::Data> NativeObject::executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType) {
    id nsObj = (id)this->value;
    return NativeInvoker(nsObj).execute(methodName, args, retType);
}

std::shared_ptr<mog::Data> NativeObject::getPropertyMain(std::string propertyName, DataType retType) {
    id nsObj = (id)this->value;
    return NativeInvoker(nsObj).getProperty(propertyName, retType);
}

void NativeObject::setProperty(std::string propertyName, const std::shared_ptr<Data> &value) {
    id nsObj = (id)this->value;
    NativeInvoker(nsObj).setProperty(propertyName, value);
}

static void toNativeDictionary(const std::shared_ptr<Dictionary> &dictionary, NSMutableDictionary *dict) {
    for (std::string key : dictionary->getKeys()) {
        auto type = dictionary->getType(key);
        NSString *keyStr = [NSString stringWithUTF8String:key.c_str()];
        NSObject *obj = nil;
        switch (type) {
            case DataType::Int:
                obj = [NSNumber numberWithInt:dictionary->get<Int>(key)->getValue()];
                break;
            case DataType::Long:
                obj = [NSNumber numberWithLong:dictionary->get<Long>(key)->getValue()];
                break;
            case DataType::Float:
                obj = [NSNumber numberWithFloat:dictionary->get<Float>(key)->getValue()];
                break;
            case DataType::Double:
                obj = [NSNumber numberWithDouble:dictionary->get<Double>(key)->getValue()];
                break;
            case DataType::String:
                obj = [NSString stringWithUTF8String:dictionary->get<String>(key)->getValue().c_str()];
                break;
            case DataType::Bool:
                obj = [NSNumber numberWithBool:(BOOL)dictionary->get<Bool>(key)->getValue()];
                break;
            case DataType::ByteArray: {
                auto bytes = dictionary->get<ByteArray>(key);
                unsigned char *value = nullptr;
                unsigned int length = 0;
                bytes->getValue(&value, &length);
                obj = [NSData dataWithBytes:value length:length];
                break;
            }
            case DataType::Dictionary: {
                NSMutableDictionary *d = [NSMutableDictionary new];
                toNativeDictionary(dictionary->get<Dictionary>(key), d);
                obj = d;
                break;
            }
            case DataType::List: {
                NSMutableArray *a = [NSMutableArray new];
                toNativeList(dictionary->get<List>(key), a);
                obj = a;
                break;
            }
            case DataType::NativeObject: {
                auto o = dictionary->get<NativeObject>(key);
                obj = (NSObject *)o->getValue();
                break;
            }
            default:
                break;
        }
        [dict setObject:obj forKey:keyStr];
    }
}

static void toNativeList(const std::shared_ptr<List> &list, NSMutableArray *nList) {
    for (int i = 0; i < list->size(); i++) {
        auto type = list->atType(i);
        NSObject *obj = nil;
        switch (type) {
            case DataType::Int:
                obj = [NSNumber numberWithInt:list->at<Int>(i)->getValue()];
                break;
            case DataType::Long:
                obj = [NSNumber numberWithLong:list->at<Long>(i)->getValue()];
                break;
            case DataType::Float:
                obj = [NSNumber numberWithFloat:list->at<Float>(i)->getValue()];
                break;
            case DataType::Double:
                obj = [NSNumber numberWithDouble:list->at<Double>(i)->getValue()];
                break;
            case DataType::String:
                obj = [NSString stringWithUTF8String:list->at<String>(i)->getValue().c_str()];
                break;
            case DataType::Bool:
                obj = [NSNumber numberWithBool:(BOOL)list->at<Bool>(i)->getValue()];
                break;
            case DataType::ByteArray: {
                auto bytes = list->at<ByteArray>(i);
                unsigned char *value = nullptr;
                unsigned int length = 0;
                bytes->getValue(&value, &length);
                obj = [NSData dataWithBytes:value length:length];
                break;
            }
            case DataType::Dictionary: {
                NSMutableDictionary *d = [NSMutableDictionary new];
                toNativeDictionary(list->at<Dictionary>(i), d);
                obj = d;
                break;
            }
            case DataType::List: {
                NSMutableArray *a = [NSMutableArray new];
                toNativeList(list->at<List>(i), a);
                obj = a;
                break;
            }
            case DataType::NativeObject: {
                obj = (NSObject *)list->at<NativeObject>(i)->getValue();
                break;
            }
            default:
                break;
        }
        [nList addObject:obj];
    }
}

static void toMogDictionary(NSDictionary *nDict, std::shared_ptr<Dictionary> &dict) {
    for (NSString *key in [nDict keyEnumerator]) {
        std::string keyStr = key.UTF8String;
        NSObject *val = nDict[key];
        
        if ([val isKindOfClass:[NSNumber class]]) {
            NSNumber *num = (NSNumber *)val;
            long long l = [num longLongValue];
            double d = [num doubleValue];
            if (l == (long long)d) {
                dict->put(keyStr, Long::create(l));
            } else {
                dict->put(keyStr, Double::create(d));
            }
            
        } else if ([val isKindOfClass:[NSString class]]) {
            NSString *str = (NSString *)val;
            dict->put(keyStr, String::create(str.UTF8String));
            
        } else if ([val isKindOfClass:[NSData class]]) {
            NSData *data = (NSData *)val;
            dict->put(keyStr, ByteArray::create((unsigned char *)data.bytes, (unsigned int)data.length, true));
            
        } else {
            dict->put(keyStr, NativeObject::create((void *)val));
        }
    }
}

static void toMogList(NSArray *nList, std::shared_ptr<List> &list) {
    for (NSObject *val in nList) {
        if ([val isKindOfClass:[NSNumber class]]) {
            NSNumber *num = (NSNumber *)val;
            long long l = [num longLongValue];
            double d = [num doubleValue];
            if (l == (long long)d) {
                list->append(Long::create(l));
            } else {
                list->append(Double::create(d));
            }
            
        } else if ([val isKindOfClass:[NSString class]]) {
            NSString *str = (NSString *)val;
            list->append(String::create(str.UTF8String));
            
        } else if ([val isKindOfClass:[NSData class]]) {
            NSData *data = (NSData *)val;
            list->append(ByteArray::create((unsigned char *)data.bytes, (unsigned int)data.length, true));
            
        } else {
            list->append(NativeObject::create((void *)val));
        }
    }
}

