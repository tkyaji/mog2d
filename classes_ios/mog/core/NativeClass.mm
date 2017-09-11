#import "mog/core/NativeClass.h"
#import <Foundation/Foundation.h>
#import <unordered_map>
#import "mog/Constants.h"
#import "MogFunction.h"

using namespace mog;

void __toNativeDictionary(const Dictionary &dictionary, NSMutableDictionary *dict);
void __toNativeArray(const Array &array, NSMutableArray *arr);

#pragma - NativeInvoker

class NativeInvoker {
public:
    NativeInvoker(NSProxy<NSObject> *target) {
        this->target = target;
    }
    
    NRet execute(string methodName, const NArg *args, int len, NRet::Type retType) {
        SEL sel = NSSelectorFromString([NSString stringWithUTF8String:methodName.c_str()]);
        NSMethodSignature *sig = [this->target methodSignatureForSelector:sel];
        assert(sig);
        NSInvocation* inv = [NSInvocation invocationWithMethodSignature:sig];
        [inv setSelector:sel];
        for (int i = 0; i < len; i++) {
            this->setArgument(inv, args[i], 2 + i);
        }
        [inv invokeWithTarget:this->target];
        NRet ret;
        ret.type = retType;
        this->getReturnValue(inv, &ret);
        return ret;
    }
    
private:
    NSProxy<NSObject> *target;
    
    void setArgument(NSInvocation* inv, const NArg &arg, int idx) {
        switch (arg.type) {
            case NArg::Type::Int: {
                int i = arg.v.i;
                [inv setArgument:&i atIndex:idx];
                break;
            }
            case NArg::Type::Long: {
                long l = arg.v.l;
                [inv setArgument:&l atIndex:idx];
                break;
            }
            case NArg::Type::Float: {
                float f = arg.v.f;
                [inv setArgument:&f atIndex:idx];
                break;
            }
            case NArg::Type::Double: {
                double d = arg.v.d;
                [inv setArgument:&d atIndex:idx];
                break;
            }
            case NArg::Type::Bool: {
                bool b = arg.v.b;
                [inv setArgument:&b atIndex:idx];
                break;
            }
            case NArg::Type::Object: {
                void *o = arg.o->getObject();
                [inv setArgument:&o atIndex:idx];
                break;
            }
        }
    }
    
    void getReturnValue(NSInvocation* inv, NRet *ret) {
        switch (ret->type) {
            case NRet::Type::Int: {
                [inv getReturnValue:&ret->v.i];
                break;
            }
            case NRet::Type::Long: {
                [inv getReturnValue:&ret->v.l];
                break;
            }
            case NRet::Type::Float: {
                [inv getReturnValue:&ret->v.f];
                break;
            }
            case NRet::Type::Double: {
                [inv getReturnValue:&ret->v.d];
                break;
            }
            case NRet::Type::Bool: {
                [inv getReturnValue:&ret->v.b];
                break;
            }
            case NRet::Type::Object: {
                NSObject *o;
                [inv getReturnValue:&o];
                if (o) {
                    ret->o = NativeObject::create((void *)o);
                }
                break;
            }
            case NRet::Type::Void:
                break;
        }
    }
};


#pragma - NFunction

NFunction::NFunction(function<void(NArg *args, int len)> func) {
    this->func = func;
}

void NFunction::invoke(NArg *args, int len) {
    this->func(args, len);
}


#pragma - NArg

NArg::NArg(int v) {
    this->type = Type::Int;
    this->v.i = v;
}

NArg::NArg(long v) {
    this->type = Type::Long;
    this->v.l = v;
}

NArg::NArg(float v) {
    this->type = Type::Float;
    this->v.f = v;
}

NArg::NArg(double v) {
    this->type = Type::Double;
    this->v.d = v;
}

NArg::NArg(bool v) {
    this->type = Type::Bool;
    this->v.b = v;
}

NArg::NArg(const char *v) {
    this->type = Type::Object;
    this->o = NativeObject::create(string(v));
}

NArg::NArg(string v) {
    this->type = Type::Object;
    this->o = NativeObject::create(v);
}

/*
NArg::NArg(void *v, Type type) {
    if (type == Type::Object) {
        this->type = Type::Object;
        this->v.o = v;
        NativeObjectPool::retain(this->v.o);
        
    } else if (type == Type::Function) {
        this->type = Type::Function;
        this->v.o = v;
    }
}
 */
/*
NArg::NArg(void *v) {
    this->type = Type::Function;
    this->v.o = v;
}
*/

NArg::NArg(function<void(NArg *args, int len)> func) {
    this->type = Type::Object;
    MogFunction *mogFunc = [[MogFunction alloc] initWithCallback:^(void *args, int len) {
        func((NArg *)args, len);
    }];
    this->o = NativeObject::create((void *)mogFunc);
}

NArg::NArg(const Dictionary &v) {
    this->type = Type::Object;
    this->o = NativeObject::create(v);
}

NArg::NArg(const Array &v) {
    this->type = Type::Object;
    this->o = NativeObject::create(v);
}

NArg::NArg(const shared_ptr<NativeObject> &v) {
    this->type = Type::Object;
    this->o = v;
}

NArg::NArg(void *v) {
    this->type = Type::Object;
    this->o = NativeObject::create(v);
}

NArg::~NArg() {
}


#pragma - NRet

NRet::NRet() {
    this->type = Type::Void;
}

NRet::~NRet() {
}

/*
NativeObject NRet::getNativeObject() {
    switch (this->type) {
        case Type::Int:
            return NativeObject(Int(this->v.i));
        case Type::Long:
            return NativeObject(Long(this->v.l));
        case Type::Float:
            return NativeObject(Float(this->v.f));
        case Type::Double:
            return NativeObject(Double(this->v.d));
        case Type::Bool:
            return NativeObject(Bool(this->v.b));
        case Type::Object:n
            return NativeObject(this->v.o);
        default:
            return NativeObject();
    }
}
*/

#pragma - NativeClass

shared_ptr<NativeClass> NativeClass::create(string className) {
    auto c = shared_ptr<NativeClass>(new NativeClass());
    Class cls = NSClassFromString([NSString stringWithUTF8String:className.c_str()]);
    c->cls = (void *)cls;
    return c;
}

NativeClass::~NativeClass() {
}

shared_ptr<NativeObject> NativeClass::newInstance() {
    Class cls = (Class)this->cls;
    NSObject *obj = [cls alloc];
    return NativeObject::create((void *)obj);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg *args, int len) {
    Class cls = (Class)this->cls;
    NSObject *obj = [cls alloc];
    return NativeObject::create((void *)obj);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1) {
    NArg args[] = {arg1};
    return this->newInstance(args, 1);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2) {
    NArg args[] = {arg1, arg2};
    return this->newInstance(args, 2);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3) {
    NArg args[] = {arg1, arg2, arg3};
    return this->newInstance(args, 3);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4) {
    NArg args[] = {arg1, arg2, arg3, arg4};
    return this->newInstance(args, 4);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5};
    return this->newInstance(args, 5);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                      const NArg &arg6) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    return this->newInstance(args, 6);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                      const NArg &arg6, const NArg &arg7) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7};
    return this->newInstance(args, 7);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                      const NArg &arg6, const NArg &arg7, const NArg &arg8) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8};
    return this->newInstance(args, 8);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                      const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9};
    return this->newInstance(args, 9);
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                      const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9, const NArg &arg10) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10};
    return this->newInstance(args, 10);
}


NRet NativeClass::execute(string methodName, NArg *args, int len, NRet::Type retType) {
    Class cls = (Class)this->cls;
    return NativeInvoker(cls).execute(methodName, args, len, retType);
}

NRet NativeClass::execute(string methodName, NRet::Type retType) {
    return this->execute(methodName, nullptr, 0, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, NRet::Type retType) {
    NArg args[] = {arg1};
    return this->execute(methodName, args, 1, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, NRet::Type retType) {
    NArg args[] = {arg1, arg2};
    return this->execute(methodName, args, 2, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3};
    return this->execute(methodName, args, 3, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4};
    return this->execute(methodName, args, 4, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5};
    return this->execute(methodName, args, 5, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    return this->execute(methodName, args, 6, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7};
    return this->execute(methodName, args, 7, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8};
    return this->execute(methodName, args, 8, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                           NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9};
    return this->execute(methodName, args, 9, retType);
}

NRet NativeClass::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                           const NArg &arg10, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10};
    return this->execute(methodName, args, 10, retType);
}



#pragma - NativeObject

NativeObject::~NativeObject() {
    if (this->obj) {
        [(NSObject *)obj release];
    }
}


shared_ptr<NativeObject> NativeObject::create(void *obj) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    o->obj = obj;
    [(NSObject *)obj retain];
    return o;
}

shared_ptr<NativeObject> NativeObject::create(string str) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSString *nsStr = [[NSString alloc] initWithUTF8String:str.c_str()];
    [nsStr retain];
    o->obj = (void *)nsStr;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Dictionary &dict) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSMutableDictionary *nsDict = [NSMutableDictionary new];
    __toNativeDictionary(dict, nsDict);
    [nsDict retain];
    o->obj = (void *)nsDict;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Array &arr) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSMutableArray *nsArr = [NSMutableArray new];
    __toNativeArray(arr, nsArr);
    [nsArr retain];
    o->obj = (void *)nsArr;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Int &i) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSNumber *n = [NSNumber numberWithInt:i.value];
    [n retain];
    o->obj = (void *)n;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Long &l) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSNumber *n = [NSNumber numberWithLong:l.value];
    [n retain];
    o->obj = (void *)n;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Float &f) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSNumber *n = [NSNumber numberWithFloat:f.value];
    [n retain];
    o->obj = (void *)n;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Double &d) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSNumber *n = [NSNumber numberWithDouble:d.value];
    [n retain];
    o->obj = (void *)n;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Bool &b) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSNumber *n = [NSNumber numberWithBool:b.value];
    [n retain];
    o->obj = (void *)n;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const String &s) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSString *nsStr = [[NSString alloc] initWithUTF8String:s.value.c_str()];
    [nsStr retain];
    o->obj = (void *)nsStr;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Bytes &b) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    NSData *data = [NSData dataWithBytes:b.value length:b.length];
    [data retain];
    o->obj = (void *)data;
    return o;
}

NRet NativeObject::execute(string methodName, NArg *args, int len, NRet::Type retType) {
    id nsObj = (id)this->obj;
    return NativeInvoker(nsObj).execute(methodName, args, len, retType);
}

NRet NativeObject::execute(string methodName, NRet::Type retType) {
    return this->execute(methodName, nullptr, 0, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, NRet::Type retType) {
    NArg args[] = {arg1};
    return this->execute(methodName, args, 1, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, NRet::Type retType) {
    NArg args[] = {arg1, arg2};
    return this->execute(methodName, args, 2, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3};
    return this->execute(methodName, args, 3, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4};
    return this->execute(methodName, args, 4, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5};
    return this->execute(methodName, args, 5, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6};
    return this->execute(methodName, args, 6, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7};
    return this->execute(methodName, args, 7, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8};
    return this->execute(methodName, args, 8, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                           NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9};
    return this->execute(methodName, args, 9, retType);
}

NRet NativeObject::execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                           const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                           const NArg &arg10, NRet::Type retType) {
    NArg args[] = {arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10};
    return this->execute(methodName, args, 10, retType);
}

void *NativeObject::getObject() {
    return this->obj;
}

void __toNativeDictionary(const Dictionary &dictionary, NSMutableDictionary *dict) {
    for (string key : dictionary.getKeys()) {
        auto type = dictionary.getType(key);
        NSString *keyStr = [NSString stringWithUTF8String:key.c_str()];
        NSObject *obj = nil;
        switch (type) {
            case DataType::Int:
                obj = [NSNumber numberWithInt:dictionary.get<Int>(key).value];
                break;
            case DataType::Long:
                obj = [NSNumber numberWithLong:dictionary.get<Long>(key).value];
                break;
            case DataType::Float:
                obj = [NSNumber numberWithFloat:dictionary.get<Float>(key).value];
                break;
            case DataType::Double:
                obj = [NSNumber numberWithDouble:dictionary.get<Double>(key).value];
                break;
            case DataType::String:
                obj = [NSString stringWithUTF8String:dictionary.get<String>(key).value.c_str()];
                break;
            case DataType::Bool:
                obj = [NSNumber numberWithBool:(BOOL)dictionary.get<Bool>(key).value];
                break;
            case DataType::Bytes: {
                auto bytes = dictionary.get<Bytes>(key);
                obj = [NSData dataWithBytes:bytes.value length:bytes.length];
                break;
            }
            case DataType::Dictionary: {
                NSMutableDictionary *d = [NSMutableDictionary new];
                __toNativeDictionary(dictionary.get<Dictionary>(key), d);
                obj = d;
                break;
            }
            case DataType::Array: {
                NSMutableArray *a = [NSMutableArray new];
                __toNativeArray(dictionary.get<Array>(key), a);
                obj = a;
                break;
            }
            default:
                break;
        }
        [dict setObject:obj forKey:keyStr];
    }
}

void __toNativeArray(const Array &array, NSMutableArray *arr) {
    for (int i = 0; i < array.size(); i++) {
        auto type = array.atType(i);
        NSObject *obj = nil;
        switch (type) {
            case DataType::Int:
                obj = [NSNumber numberWithInt:array.at<Int>(i).value];
                break;
            case DataType::Long:
                obj = [NSNumber numberWithLong:array.at<Long>(i).value];
                break;
            case DataType::Float:
                obj = [NSNumber numberWithFloat:array.at<Float>(i).value];
                break;
            case DataType::Double:
                obj = [NSNumber numberWithDouble:array.at<Double>(i).value];
                break;
            case DataType::String:
                obj = [NSString stringWithUTF8String:array.at<String>(i).value.c_str()];
                break;
            case DataType::Bool:
                obj = [NSNumber numberWithBool:(BOOL)array.at<Bool>(i).value];
                break;
            case DataType::Bytes: {
                auto bytes = array.at<Bytes>(i);
                obj = [NSData dataWithBytes:bytes.value length:bytes.length];
                break;
            }
            case DataType::Dictionary: {
                NSMutableDictionary *d = [NSMutableDictionary new];
                __toNativeDictionary(array.at<Dictionary>(i), d);
                obj = d;
                break;
            }
            case DataType::Array: {
                NSMutableArray *a = [NSMutableArray new];
                __toNativeArray(array.at<Array>(i), a);
                obj = a;
                break;
            }
            default:
                break;
        }
        [arr addObject:obj];
    }
}
