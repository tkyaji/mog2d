#include <algorithm>
#include <assert.h>
#include "mog/core/NativeClass.h"
#include "mog/core/Engine.h"
#include "mog/os/AndroidHelper.h"

using namespace mog;

void __toNativeDictionary(const Dictionary &dictionary, jobject mapObj);
void __toNativeArray(const Array &array, jobject listObj);

#pragma - JavaMethodInterface

class JavaMethodInterface {
public:
    enum class MethodType {
        Int,
        Long,
        Short,
        Byte,
        Char,
        Float,
        Double,
        Boolean,
        Object,
        Void,
    };

    ~JavaMethodInterface() {
        if (this->values) {
            delete[] this->values;
        }
    }

protected:
    bool isStatic = false;
    JNIEnv *env = nullptr;
    jobject jobj = nullptr;
    string methodName = "";
    jvalue *values = nullptr;
    string signature = "";
    bool valid = false;
    NRet::Type retType;
    MethodType methodType;

    bool buildParams(jobject jmethod, const NArg *args, int len) {
        jclass jmethodCls = this->env->GetObjectClass(jmethod);
        jmethodID getNameId = this->env->GetMethodID(jmethodCls, "getName", "()Ljava/lang/String;");

        jmethodID getParameterTypesId = this->env->GetMethodID(jmethodCls, "getParameterTypes", "()[Ljava/lang/Class;");
        jobjectArray jclasses = (jobjectArray )this->env->CallObjectMethod(jmethod, getParameterTypesId);
        int classesLength = this->env->GetArrayLength(jclasses);
        if (classesLength != len) return false;

        jclass clsCls = this->env->FindClass("java/lang/Class");
        jmethodID clsGetNameId = this->env->GetMethodID(clsCls, "getName", "()Ljava/lang/String;");

        this->signature = "(";
        for (int j = 0; j < classesLength; j++) {
            auto arg = args[j];
            jclass jparamCls = (jclass)this->env->GetObjectArrayElement(jclasses, j);
            jstring jparamClsName = (jstring)this->env->CallObjectMethod(jparamCls, clsGetNameId);
            const char *jparamClsNameCStr = this->env->GetStringUTFChars(jparamClsName, NULL);
            string jParamClsNameStr = string(jparamClsNameCStr);
            this->env->ReleaseStringUTFChars(jparamClsName, jparamClsNameCStr);

            if (!this->buildArg(arg, jparamCls, jParamClsNameStr, j)) {
                return false;
            }
        }
        this->signature += ")";

        return true;
    }


    bool buildArg(const NArg &arg, jclass jcls, string jClsName, int idx) {
        if (arg.type == NArg::Type::Int && jClsName == "int") {
            this->values[idx].i = (jint)arg.v.i;
            this->signature += "I";
        } else if (arg.type == NArg::Type::Long && jClsName == "long") {
            this->values[idx].j = (jlong)arg.v.l;
            this->signature += "J";
        } else if (arg.type == NArg::Type::Float && jClsName == "float") {
            this->values[idx].f = (jfloat)arg.v.f;
            this->signature += "F";
        } else if (arg.type == NArg::Type::Double && jClsName == "double") {
            this->values[idx].d = (jdouble)arg.v.d;
            this->signature += "D";
        } else if (arg.type == NArg::Type::Bool && jClsName == "boolean") {
            this->values[idx].z = (jboolean)arg.v.b;
            this->signature += "Z";
        } else if (jClsName == "byte") {
            this->signature += "B";
            // TODO
            return false;
        } else if (jClsName == "short") {
            this->signature += "S";
            // TODO
            return false;
        } else if (jClsName == "char") {
            this->signature += "C";
            // TODO
            return false;
        } else if (arg.type == NArg::Type::Object && this->env->IsInstanceOf((jobject)arg.o->getObject(), jcls)) {
            this->values[idx].l = (jobject)arg.o->getObject();
            replace(jClsName.begin(), jClsName.end(), '.', '/');
            this->signature += "L" + jClsName + ";";
        } else {
            return false;
        }
        return true;
    }

    bool buildReturnType(jobject jmethod, jclass jmethodCls) {
        jclass clsCls = this->env->FindClass("java/lang/Class");
        jmethodID clsGetNameId = this->env->GetMethodID(clsCls, "getName", "()Ljava/lang/String;");

        jmethodID getReturnTypeId = this->env->GetMethodID(jmethodCls, "getReturnType", "()Ljava/lang/Class;");
        jclass retCls = (jclass)this->env->CallObjectMethod(jmethod, getReturnTypeId);
        jstring retJcName = (jstring)this->env->CallObjectMethod(retCls, clsGetNameId);
        const char *retJcNameCStr = this->env->GetStringUTFChars(retJcName, NULL);
        string retJcNameStr = string(retJcNameCStr);
        this->env->ReleaseStringUTFChars(retJcName, retJcNameCStr);

        if (retJcNameStr == "int") {
            this->signature += "I";
            this->methodType = MethodType::Int;
        } else if (retJcNameStr == "long") {
            this->signature += "J";
            this->methodType = MethodType::Long;
        } else if (retJcNameStr == "float") {
            this->signature += "F";
            this->methodType = MethodType::Float;
        } else if (retJcNameStr == "double") {
            this->signature += "D";
            this->methodType = MethodType::Double;
        } else if (retJcNameStr == "boolean") {
            this->signature += "Z";
            this->methodType = MethodType::Boolean;
        } else if (retJcNameStr == "byte") {
            this->signature += "B";
            this->methodType = MethodType::Byte;
        } else if (retJcNameStr == "short") {
            this->signature += "S";
            this->methodType = MethodType::Short;
        } else if (retJcNameStr == "char") {
            this->signature += "C";
            this->methodType = MethodType::Char;
        } else if (retJcNameStr == "void") {
            this->signature += "V";
            this->methodType = MethodType::Void;
        } else {
            replace(retJcNameStr.begin(), retJcNameStr.end(), '.', '/');
            string typeStr = "L" + retJcNameStr + ";";
            this->signature += typeStr.c_str();
            this->methodType = MethodType::Object;
        }
        return true;
    }
};


#pragma - JavaMethod

class JavaMethod : public JavaMethodInterface {
public:
    JavaMethod(jobject jobj, string methodName, NRet::Type retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jobj;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, nullptr, 0, retType);
    }

    JavaMethod(jobject jobj, string methodName, const NArg *args, int len, NRet::Type retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jobj;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, args, len, retType);
    }

    JavaMethod(jclass jcls, string methodName, NRet::Type retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, nullptr, 0, retType);
    }

    JavaMethod(jclass jcls, string methodName, const NArg *args, int len, NRet::Type retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, args, len, retType);
    }

    NRet invoke() {
        assert(this->valid);

        if (this->isStatic) {
            return this->invokeStaticMethod();
        } else {
            return this->invokeMethod();
        }
    }

protected:
    void build(string methodName, const NArg *args, int len, NRet::Type retType) {
        this->env->PushLocalFrame(32);

        jclass jcls;
        if (this->isStatic) {
            jcls = (jclass)this->jobj;
        } else {
            jcls = this->env->GetObjectClass(this->jobj);
        }
        jclass jclsCls = this->env->GetObjectClass(jcls);

        jmethodID getMethodsId = this->env->GetMethodID(jclsCls, "getMethods", "()[Ljava/lang/reflect/Method;");
        jobjectArray methods = (jobjectArray)this->env->CallObjectMethod(jcls, getMethodsId);

        jclass jmethodCls = this->env->FindClass("java/lang/reflect/Method");
        jmethodID getNameId = this->env->GetMethodID(jmethodCls, "getName", "()Ljava/lang/String;");

        this->values = new jvalue[len];
        int methodsLength = this->env->GetArrayLength(methods);
        for (int i = 0; i < methodsLength; i++) {
            this->env->PushLocalFrame(16);

            jobject jmethod = this->env->GetObjectArrayElement(methods, i);

            jstring jmethodName = (jstring)this->env->CallObjectMethod(jmethod, getNameId);
            const char *methodNameCStr = this->env->GetStringUTFChars(jmethodName, NULL);
            string methodNameStr = string(methodNameCStr);
            this->env->ReleaseStringUTFChars(jmethodName, methodNameCStr);
            if (methodName != methodNameStr) {
                this->env->PopLocalFrame(NULL);
                continue;
            }

            if (!this->buildParams(jmethod, args, len)) {
                this->env->PopLocalFrame(NULL);
                continue;
            }
            if (!this->buildReturnType(jmethod, jmethodCls)) {
                this->env->PopLocalFrame(NULL);
                continue;
            }

            this->valid = true;
            this->env->PopLocalFrame(NULL);
            break;
        }

        this->env->PopLocalFrame(NULL);
    }

    NRet invokeMethod() {
        NRet ret;

        jclass jcls = this->env->GetObjectClass(this->jobj);
        jmethodID methodId = this->env->GetMethodID(jcls, this->methodName.c_str(), this->signature.c_str());
        assert(methodId);

        switch (this->methodType) {
            case MethodType::Int:
                ret.v.i = this->env->CallIntMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Long:
                ret.v.l = this->env->CallLongMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Short:
                ret.v.i = this->env->CallShortMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Byte:
                ret.v.i = this->env->CallByteMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Char:
                ret.v.i = this->env->CallCharMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Float:
                ret.v.f = this->env->CallFloatMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Double:
                ret.v.d = this->env->CallDoubleMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Boolean:
                ret.v.b = this->env->CallBooleanMethodA(this->jobj, methodId, this->values);
                break;
            case MethodType::Object: {
                jobject o = this->env->CallObjectMethodA(this->jobj, methodId, this->values);
                ret.o = NativeObject::create(o);
                break;
            }
            case MethodType::Void:
                this->env->CallVoidMethodA(this->jobj, methodId, this->values);
                break;
        }

        return ret;
    }

    NRet invokeStaticMethod() {
        NRet ret;

        jclass jcls = (jclass)this->jobj;
        jmethodID methodId = this->env->GetStaticMethodID(jcls, this->methodName.c_str(), this->signature.c_str());
        assert(methodId);

        switch (this->methodType) {
            case MethodType::Int:
                ret.v.i = this->env->CallStaticIntMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Long:
                ret.v.l = this->env->CallStaticLongMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Short:
                ret.v.i = this->env->CallStaticShortMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Byte:
                ret.v.i = this->env->CallStaticByteMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Char:
                ret.v.i = this->env->CallStaticCharMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Float:
                ret.v.f = this->env->CallStaticFloatMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Double:
                ret.v.d = this->env->CallStaticDoubleMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Boolean:
                ret.v.b = this->env->CallStaticBooleanMethodA((jclass)this->jobj, methodId, this->values);
                break;
            case MethodType::Object: {
                jobject o = this->env->CallStaticObjectMethodA((jclass)this->jobj, methodId, this->values);
                ret.o = NativeObject::create(o);
                break;
            }
            case MethodType::Void:
                this->env->CallStaticVoidMethodA((jclass)this->jobj, methodId, this->values);
                break;
        }

        return ret;
    }
};


#pragma - JavaConstructor

class JavaConstructor : public JavaMethodInterface {
public:
    JavaConstructor(jclass jcls) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = "<init>";
        this->retType = NRet::Type::Void;
        this->build(nullptr, 0, this->retType);
    }

    JavaConstructor(jclass jcls, const NArg *args, int len) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = "<init>";
        this->retType = NRet::Type::Void;
        this->build(args, len, this->retType);
    }

    shared_ptr<NativeObject> newInstance() {
        assert(this->valid);

        jmethodID init = this->env->GetMethodID((jclass)this->jobj, "<init>", this->signature.c_str());
        jobject jobj = this->env->NewObjectA((jclass)this->jobj, init, this->values);

        auto no = NativeObject::create(jobj);
        env->DeleteLocalRef(jobj);
        return no;
    }

protected:
    void build(const NArg *args, int len, NRet::Type retType) {
        this->env->PushLocalFrame(32);

        jclass jcls = (jclass)this->jobj;
        jclass jclsCls = this->env->GetObjectClass(jcls);

        jmethodID getConstructorsId = this->env->GetMethodID(jclsCls, "getConstructors", "()[Ljava/lang/reflect/Constructor;");
        jobjectArray constructors = (jobjectArray)this->env->CallObjectMethod(jcls, getConstructorsId);

        jclass jconstructorCls = this->env->FindClass("java/lang/reflect/Constructor");
        jmethodID getNameId = this->env->GetMethodID(jconstructorCls, "getName", "()Ljava/lang/String;");

        this->values = new jvalue[len];
        int methodsLength = this->env->GetArrayLength(constructors);
        for (int i = 0; i < methodsLength; i++) {
            this->env->PushLocalFrame(16);
            jobject jmethod = this->env->GetObjectArrayElement(constructors, i);
            if (!this->buildParams(jmethod, args, len)) {
                this->env->PopLocalFrame(NULL);
                continue;
            }
            this->signature += "V";

            this->valid = true;
            this->env->PopLocalFrame(NULL);
            break;
        }

        this->env->PopLocalFrame(NULL);
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

NArg::NArg() {
    this->type = Type::Object;
    this->o = nullptr;
}

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

NArg::NArg(function<void(NArg *args, int len)> func) {
    this->type = Type::Object;
    auto nf = new NFunction(func);

    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("org/mog2d/MogFunction");
    jmethodID mid = env->GetMethodID(jcls, "<init>", "(J)V");
    jobject jobj = env->NewObject(jcls, mid, (jlong)nf);
    this->o = NativeObject::create(jobj);
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
        case Type::Object:
            return NativeObject(this->v.o);
        default:
            return NativeObject();
    }
}
*/

#pragma - NativeClass

shared_ptr<NativeClass> NativeClass::create(string className) {
    auto c = shared_ptr<NativeClass>(new NativeClass());
    replace(className.begin(), className.end(), '.', '/');
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass(className.c_str());
    assert(jcls);
    jobject go = env->NewGlobalRef(jcls);
    env->DeleteLocalRef(jcls);
    c->cls = (void *)go;
    return c;
}

NativeClass::~NativeClass() {
    if (this->cls) {
        JNIEnv *env = AndroidHelper::getEnv();
        env->DeleteGlobalRef((jclass)this->cls);
    }
}

shared_ptr<NativeObject> NativeClass::newInstance() {
    return JavaConstructor((jclass)this->cls).newInstance();
}

shared_ptr<NativeObject> NativeClass::newInstance(const NArg *args, int len) {
    return JavaConstructor((jclass)this->cls, args, len).newInstance();
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
    return JavaMethod((jclass)this->cls, methodName, args, len, retType).invoke();
}

NRet NativeClass::execute(string methodName, NRet::Type retType) {
    return JavaMethod((jclass)this->cls, methodName, retType).invoke();
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
        JNIEnv *env = AndroidHelper::getEnv();
        env->DeleteGlobalRef((jobject)this->obj);
    }
}

shared_ptr<NativeObject> NativeObject::create(void *obj) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    if (obj) {
        JNIEnv *env = AndroidHelper::getEnv();
        jobject go = env->NewGlobalRef((jobject)obj);
        o->obj = go;
    }
    return o;
}

shared_ptr<NativeObject> NativeObject::create(string str) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    const char *cstr = str.c_str();
    jstring s = env->NewStringUTF(cstr);
    jobject go = env->NewGlobalRef(s);
    env->DeleteLocalRef(s);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Dictionary &dict) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(16);
    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
    jobject mapObj = env->NewObject(mapClass, mapInit);
    __toNativeDictionary(dict, mapObj);
    jobject go = env->NewGlobalRef((jobject)mapObj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Array &arr) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(16);
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");
    jmethodID addMethod = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");
    jobject listObj = env->NewObject(listClass, listInit);
    __toNativeArray(arr, listObj);
    jobject go = env->NewGlobalRef((jobject)listObj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Int &i) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jclass jcls = env->FindClass("java/lang/Integer");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(I)V");
    jobject jobj= env->NewObject(jcls, integerInit, i.value);
    jobject go = env->NewGlobalRef(jobj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Long &l) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jclass jcls = env->FindClass("java/lang/Long");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(J)V");
    jobject jobj= env->NewObject(jcls, integerInit, l.value);
    jobject go = env->NewGlobalRef(jobj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Float &f) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jclass jcls = env->FindClass("java/lang/Float");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(F)V");
    jobject jobj= env->NewObject(jcls, integerInit, f.value);
    jobject go = env->NewGlobalRef(jobj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Double &d) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jclass jcls = env->FindClass("java/lang/Double");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(D)V");
    jobject jobj= env->NewObject(jcls, integerInit, d.value);
    jobject go = env->NewGlobalRef(jobj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Bool &b) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jclass jcls = env->FindClass("java/lang/Boolean");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(Z)V");
    jobject jobj= env->NewObject(jcls, integerInit, b.value);
    jobject go = env->NewGlobalRef(jobj);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const String &s) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    const char *cstr = s.value.c_str();
    jstring jstr = env->NewStringUTF(cstr);
    jobject go = env->NewGlobalRef(jstr);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}

shared_ptr<NativeObject> NativeObject::create(const Bytes &b) {
    auto o = shared_ptr<NativeObject>(new NativeObject());
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(8);
    jbyteArray jbyteArr = env->NewByteArray(b.length);
    jbyte *bytes = env->GetByteArrayElements(jbyteArr, NULL);
    for (int i = 0; i < b.length; i++) {
        bytes[i] = b.value[i];
    }
    jobject go = env->NewGlobalRef(jbyteArr);
    env->ReleaseByteArrayElements(jbyteArr, bytes, 0);
    env->PopLocalFrame(NULL);
    o->obj = go;
    return o;
}



//NativeObject NativeObject::Null = NativeObject();

/*
NativeObject::NativeObject() {
}

NativeObject::NativeObject(void *obj, bool autoRelease) {
    this->obj = obj;
    if (autoRelease) {
        NativeObjectPool::retain((jobject)obj);
    }
}

NativeObject::NativeObject(string str) {
    JNIEnv *env = AndroidHelper::getEnv();
    const char *cstr = str.c_str();
    jstring s = env->NewStringUTF(cstr);
    jobject go = env->NewGlobalRef(s);
    env->DeleteLocalRef(s);
    this->obj = go;
}

NativeObject::NativeObject(const Dictionary &dict) {
    void *nativeDict = toNativeDictionary(dict).getObject();
    NativeObjectPool::retain((jobject)nativeDict);
    this->obj = nativeDict;
}

NativeObject::NativeObject(const Array &arr) {
    void *nativeArr = toNativeArray(arr).getObject();
    NativeObjectPool::retain((jobject)nativeArr);
    this->obj = nativeArr;
}

NativeObject::NativeObject(const Int &i) {
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("java/lang/Integer");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(I)V");
    jobject jobj= env->NewObject(jcls, integerInit, i.value);
    this->obj = env->NewGlobalRef(jobj);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(jcls);
}

NativeObject::NativeObject(const Long &l) {
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("java/lang/Long");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(J)V");
    jobject jobj= env->NewObject(jcls, integerInit, l.value);
    this->obj = env->NewGlobalRef(jobj);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(jcls);
}

NativeObject::NativeObject(const Float &f) {
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("java/lang/Float");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(F)V");
    jobject jobj= env->NewObject(jcls, integerInit, f.value);
    this->obj = env->NewGlobalRef(jobj);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(jcls);
}

NativeObject::NativeObject(const Double &d) {
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("java/lang/Double");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(D)V");
    jobject jobj= env->NewObject(jcls, integerInit, d.value);
    this->obj = env->NewGlobalRef(jobj);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(jcls);
}

NativeObject::NativeObject(const Bool &b) {
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass("java/lang/Boolean");
    jmethodID integerInit = env->GetMethodID(jcls, "<init>", "(Z)V");
    jobject jobj= env->NewObject(jcls, integerInit, b.value);
    this->obj = env->NewGlobalRef(jobj);
    env->DeleteLocalRef(jobj);
    env->DeleteLocalRef(jcls);
}

NativeObject::NativeObject(const String &s) {
    JNIEnv *env = AndroidHelper::getEnv();
    const char *cstr = s.value.c_str();
    jstring jstr = env->NewStringUTF(cstr);
    this->obj = env->NewGlobalRef(jstr);
    env->DeleteLocalRef(jstr);
}

NativeObject::NativeObject(const Bytes &b) {
    JNIEnv *env = AndroidHelper::getEnv();
    jbyteArray jbyteArr = env->NewByteArray(b.length);
    jbyte *bytes = env->GetByteArrayElements(jbyteArr, NULL);
    for (int i = 0; i < b.length; i++) {
        bytes[i] = b.value[i];
    }
    this->obj = env->NewGlobalRef(jbyteArr);
    env->ReleaseByteArrayElements(jbyteArr, bytes, 0);
    env->DeleteLocalRef(jbyteArr);
}

NativeObject::~NativeObject() {
    if (this->obj) {
        NativeObjectPool::release((jobject)this->obj);
    }
}

NativeObject::NativeObject(const NativeObject &copy) {
    if (copy.obj) {
        this->obj = copy.obj;
        NativeObjectPool::retain((jobject)this->obj);
    }
}

NativeObject::NativeObject(const NArg &arg) {
    this->obj = arg.v.o;
    NativeObjectPool::retain((jobject)this->obj);
}

NativeObject::NativeObject(const NRet &ret) {
    this->obj = ret.v.o;
    NativeObjectPool::retain((jobject)this->obj);
}

NativeObject &NativeObject::operator=(const NativeObject &copy) {
    if (this != &copy) {
        if (copy.obj) {
            this->obj = copy.obj;
            NativeObjectPool::retain((jobject)this->obj);
        }
    }
    return *this;
}
*/


NRet NativeObject::execute(string methodName, NArg *args, int len, NRet::Type retType) {
    jobject jobj = (jobject)this->obj;
//    return NativeInvoker(jobj).execute(methodName, args, len, retType);
    return JavaMethod(jobj, methodName, args, len, retType).invoke();
}

NRet NativeObject::execute(string methodName, NRet::Type retType) {
    jobject jobj = (jobject)this->obj;
//    return NativeInvoker(jobj).execute(methodName, retType);
    return JavaMethod(jobj, methodName, retType).invoke();
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

void __toNativeDictionary(const Dictionary &dictionary, jobject mapObj) {
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(16);

    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");
    jmethodID addMethod = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");

    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID integerInit = env->GetMethodID(integerClass, "<init>", "(I)V");
    jclass longClass = env->FindClass("java/lang/Long");
    jmethodID longInit = env->GetMethodID(longClass, "<init>", "(J)V");
    jclass floatClass = env->FindClass("java/lang/Float");
    jmethodID floatInit = env->GetMethodID(floatClass, "<init>", "(F)V");
    jclass doubleClass = env->FindClass("java/lang/Double");
    jmethodID doubleInit = env->GetMethodID(doubleClass, "<init>", "(D)V");
    jclass booleanClass = env->FindClass("java/lang/Boolean");
    jmethodID booleanInit = env->GetMethodID(booleanClass, "<init>", "(Z)V");

    auto keys = dictionary.getKeys();
    for (auto &key : keys) {
        env->PushLocalFrame(16);

        jvalue v;
        const char *keyCStr = key.c_str();
        jstring keyStr = env->NewStringUTF(key.c_str());
        auto type = dictionary.getType(key);

        switch (type) {
            case DataType::Int: {
                jobject integerObj = env->NewObject(integerClass, integerInit, dictionary.get<Int>(key).value);
                env->CallObjectMethod(mapObj, putMethod, keyStr, integerObj);
                break;
            }
            case DataType::Long: {
                jobject longObj = env->NewObject(longClass, longInit, dictionary.get<Long>(key).value);
                env->CallObjectMethod(mapObj, putMethod, keyStr, longObj);
                break;
            }
            case DataType::Float: {
                jobject floatObj = env->NewObject(floatClass, floatInit, dictionary.get<Float>(key).value);
                env->CallObjectMethod(mapObj, putMethod, keyStr, floatObj);
                break;
            }
            case DataType::Double: {
                jobject doubleObj = env->NewObject(doubleClass, doubleInit, dictionary.get<Double>(key).value);
                env->CallObjectMethod(mapObj, putMethod, keyStr, doubleObj);
                break;
            }
            case DataType::Bool: {
                jobject booleanObj = env->NewObject(booleanClass, booleanInit, dictionary.get<Bool>(key).value);
                env->CallObjectMethod(mapObj, putMethod, keyStr, booleanObj);
                break;
            }
            case DataType::String: {
                const char *s = dictionary.get<String>(key).value.c_str();
                jstring str = env->NewStringUTF(s);
                env->CallObjectMethod(mapObj, putMethod, keyStr, str);
                break;
            }
            case DataType::Array: {
                jobject list = env->NewObject(listClass, listInit);
                __toNativeArray(dictionary.get<Array>(key), list);
                env->CallObjectMethod(mapObj, putMethod, keyStr, list);
                break;
            }
            case DataType::Dictionary: {
                jobject map = env->NewObject(mapClass, mapInit);
                __toNativeDictionary(dictionary.get<Dictionary>(key), map);
                env->CallObjectMethod(mapObj, putMethod, keyStr, map);
                break;
            }
            default:
                break;
        }
        env->PopLocalFrame(NULL);
    }
    env->PopLocalFrame(NULL);
}

void __toNativeArray(const Array &array, jobject listObj) {
    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(16);

    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");
    jmethodID addMethod = env->GetMethodID(listClass, "add", "(Ljava/lang/Object;)Z");

    jclass integerClass = env->FindClass("java/lang/Integer");
    jmethodID integerInit = env->GetMethodID(integerClass, "<init>", "(I)V");
    jclass longClass = env->FindClass("java/lang/Long");
    jmethodID longInit = env->GetMethodID(longClass, "<init>", "(J)V");
    jclass floatClass = env->FindClass("java/lang/Float");
    jmethodID floatInit = env->GetMethodID(floatClass, "<init>", "(F)V");
    jclass doubleClass = env->FindClass("java/lang/Double");
    jmethodID doubleInit = env->GetMethodID(doubleClass, "<init>", "(D)V");
    jclass booleanClass = env->FindClass("java/lang/Boolean");
    jmethodID booleanInit = env->GetMethodID(booleanClass, "<init>", "(Z)V");

    for (int i = 0; i < array.size(); i++) {
        env->PushLocalFrame(16);

        jvalue v;
        auto type = array.atType(i);

        switch (type) {
            case DataType::Int: {
                jobject integerObj = env->NewObject(integerClass, integerInit, array.at<Int>(i).value);
                env->CallBooleanMethod(listObj, addMethod, integerObj);
                break;
            }
            case DataType::Long: {
                jobject longObj = env->NewObject(longClass, longInit, array.at<Long>(i).value);
                env->CallBooleanMethod(listObj, addMethod, longObj);
                break;
            }
            case DataType::Float: {
                jobject floatObj = env->NewObject(floatClass, floatInit, array.at<Float>(i).value);
                env->CallBooleanMethod(listObj, addMethod, floatObj);
                break;
            }
            case DataType::Double: {
                jobject doubleObj = env->NewObject(doubleClass, doubleInit, array.at<Double>(i).value);
                env->CallBooleanMethod(listObj, addMethod, doubleObj);
                break;
            }
            case DataType::Bool: {
                jobject booleanObj = env->NewObject(booleanClass, booleanInit, array.at<Bool>(i).value);
                env->CallBooleanMethod(listObj, addMethod, booleanObj);
                break;
            }
            case DataType::String: {
                const char *s = array.at<String>(i).value.c_str();
                jstring str = env->NewStringUTF(s);
                env->CallBooleanMethod(listObj, addMethod, str);
                break;
            }
            case DataType::Array: {
                jobject list = env->NewObject(listClass, listInit);
                __toNativeArray(array.at<Array>(i), list);
                env->CallBooleanMethod(listObj, addMethod, list);
                break;
            }
            case DataType::Dictionary: {
                jobject map = env->NewObject(mapClass, mapInit);
                __toNativeDictionary(array.at<Dictionary>(i), map);
                env->CallBooleanMethod(listObj, addMethod, map);
                break;
            }
            default:
                break;
        }
        env->PopLocalFrame(NULL);
    }
    env->PopLocalFrame(NULL);
}


Dictionary NUtils::toDictionary(void *o) {
    Dictionary dict;

    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(128);

    jobject jobj = (jobject)o;
    jclass jcls = env->GetObjectClass(jobj);
    jclass mapCls = env->FindClass("java/util/Map");
    if (env->IsInstanceOf(jobj, mapCls)) {
        jmethodID keySetId = env->GetMethodID(jcls, "keySet", "()Ljava/util/Set;");
        jobject jKeySet = env->CallObjectMethod(jobj, keySetId);
        jclass jKeySetCls = env->GetObjectClass(jKeySet);
        jmethodID toArrayId = env->GetMethodID(jKeySetCls, "toArray", "()[Ljava/lang/Object;");
        jobjectArray keyArr = (jobjectArray)env->CallObjectMethod(jKeySet, toArrayId);
        jmethodID getId = env->GetMethodID(jcls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");

        jclass integerClass = env->FindClass("java/lang/Integer");
        jmethodID intValueId = env->GetMethodID(integerClass, "intValue", "()I");
        jclass longClass = env->FindClass("java/lang/Long");
        jmethodID longValueId = env->GetMethodID(longClass, "longValue", "()J");
        jclass charClass = env->FindClass("java/lang/Character");
        jmethodID charValueId = env->GetMethodID(charClass, "charValue", "()C");
        jclass byteClass = env->FindClass("java/lang/Byte");
        jmethodID byteValueId = env->GetMethodID(byteClass, "byteValue", "()B");
        jclass floatClass = env->FindClass("java/lang/Float");
        jmethodID floatValueId = env->GetMethodID(floatClass, "floatValue", "()F");
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleValueId = env->GetMethodID(doubleClass, "doubleValue", "()D");
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        jmethodID booleanValueId = env->GetMethodID(booleanClass, "booleanValue", "()Z");
        jclass stringClass = env->FindClass("java/lang/String");
        jclass listClass = env->FindClass("java/util/List");
        jclass mapClass = env->FindClass("java/util/Map");

        int len = env->GetArrayLength(keyArr);
        for (int i = 0; i < len; i++) {
            jstring jKey = (jstring)env->GetObjectArrayElement(keyArr, i);
            const char *keyCStr = env->GetStringUTFChars(jKey, NULL);
            auto key = string(keyCStr);
            jobject jElement = env->CallObjectMethod(jobj, getId, jKey);
            if (!jElement) continue;

            jclass jElmCls = env->GetObjectClass(jElement);

            if (env->IsInstanceOf(jElement, integerClass)) {
                jint i = env->CallIntMethod(jElement, intValueId);
                dict.put(key, Int((int)i));
            } else if (env->IsInstanceOf(jElement, longClass)) {
                jlong l = env->CallLongMethod(jElement, longValueId);
                dict.put(key, Long((long)l));
            } else if (env->IsInstanceOf(jElement, charClass)) {
                jchar c = env->CallCharMethod(jElement, charValueId);
                dict.put(key, Int((int)c));
            } else if (env->IsInstanceOf(jElement, byteClass)) {
                jbyte b = env->CallByteMethod(jElement, byteValueId);
                dict.put(key, Int((int)b));
            } else if (env->IsInstanceOf(jElement, floatClass)) {
                jfloat f = env->CallFloatMethod(jElement, floatValueId);
                dict.put(key, Float((float)f));
            } else if (env->IsInstanceOf(jElement, doubleClass)) {
                jdouble d = env->CallDoubleMethod(jElement, doubleValueId);
                dict.put(key, Double((double)d));
            } else if (env->IsInstanceOf(jElement, booleanClass)) {
                jboolean b = env->CallBooleanMethod(jElement, booleanValueId);
                dict.put(key, Bool((bool)b));
            } else if (env->IsInstanceOf(jElement, stringClass)) {
                const char *cstr = env->GetStringUTFChars((jstring)jElement, NULL);
                dict.put(key, String(cstr));
                env->ReleaseStringUTFChars((jstring)jElement, cstr);
            } else if (env->IsInstanceOf(jElement, mapClass)) {
                Dictionary d = NUtils::toDictionary(jElement);
                dict.put(key, d);
            } else if (env->IsInstanceOf(jElement, listClass)) {
                Array a = NUtils::toArray(jElement);
                dict.put(key, a);
            }
        }
    }

    env->PopLocalFrame(NULL);

    return dict;
}

Dictionary NUtils::toDictionary(const NArg &arg) {
    return NUtils::toDictionary(arg.o->getObject());
}

Dictionary NUtils::toDictionary(const NRet &ret) {
    return NUtils::toDictionary(ret.o->getObject());
}


Array NUtils::toArray(void *o) {
    Array arr;

    JNIEnv *env = AndroidHelper::getEnv();
    env->PushLocalFrame(128);

    jobject jobj = (jobject)o;
    jclass jcls = env->GetObjectClass(jobj);
    jclass listCls = env->FindClass("java/util/List");
    if (env->IsInstanceOf(jobj, listCls)) {
        jmethodID sizeId = env->GetMethodID(jcls, "size", "()I");
        int size = env->CallIntMethod(jobj, sizeId);
        jmethodID getId = env->GetMethodID(jcls, "get", "(I)Ljava/lang/Object;");

        jclass integerClass = env->FindClass("java/lang/Integer");
        jmethodID intValueId = env->GetMethodID(integerClass, "intValue", "()I");
        jclass longClass = env->FindClass("java/lang/Long");
        jmethodID longValueId = env->GetMethodID(longClass, "longValue", "()J");
        jclass charClass = env->FindClass("java/lang/Character");
        jmethodID charValueId = env->GetMethodID(charClass, "charValue", "()C");
        jclass byteClass = env->FindClass("java/lang/Byte");
        jmethodID byteValueId = env->GetMethodID(byteClass, "byteValue", "()B");
        jclass floatClass = env->FindClass("java/lang/Float");
        jmethodID floatValueId = env->GetMethodID(floatClass, "floatValue", "()F");
        jclass doubleClass = env->FindClass("java/lang/Double");
        jmethodID doubleValueId = env->GetMethodID(doubleClass, "doubleValue", "()D");
        jclass booleanClass = env->FindClass("java/lang/Boolean");
        jmethodID booleanValueId = env->GetMethodID(booleanClass, "booleanValue", "()Z");
        jclass stringClass = env->FindClass("java/lang/String");
        jclass listClass = env->FindClass("java/util/List");
        jclass mapClass = env->FindClass("java/util/Map");

        for (int i = 0; i < size; i++) {
            jobject jElement = env->CallObjectMethod(jobj, getId, (jint)i);
            jclass jElmCls = env->GetObjectClass(jElement);

            if (env->IsInstanceOf(jElement, integerClass)) {
                jint i = env->CallIntMethod(jElement, intValueId);
                arr.append(Int((int)i));
            } else if (env->IsInstanceOf(jElement, longClass)) {
                jlong l = env->CallLongMethod(jElement, longValueId);
                arr.append(Long((long)l));
            } else if (env->IsInstanceOf(jElement, charClass)) {
                jchar c = env->CallCharMethod(jElement, charValueId);
                arr.append(Int((int)c));
            } else if (env->IsInstanceOf(jElement, byteClass)) {
                jbyte b = env->CallByteMethod(jElement, byteValueId);
                arr.append(Int((int)b));
            } else if (env->IsInstanceOf(jElement, floatClass)) {
                jfloat f = env->CallFloatMethod(jElement, floatValueId);
                arr.append(Float((float)f));
            } else if (env->IsInstanceOf(jElement, doubleClass)) {
                jdouble d = env->CallDoubleMethod(jElement, doubleValueId);
                arr.append(Double((double)d));
            } else if (env->IsInstanceOf(jElement, booleanClass)) {
                jboolean b = env->CallBooleanMethod(jElement, booleanValueId);
                arr.append(Bool((bool)b));
            } else if (env->IsInstanceOf(jElement, stringClass)) {
                const char *cstr = env->GetStringUTFChars((jstring)jElement, NULL);
                arr.append(String(cstr));
                env->ReleaseStringUTFChars((jstring)jElement, cstr);
            } else if (env->IsInstanceOf(jElement, mapClass)) {
                Dictionary d = NUtils::toDictionary(jElement);
                arr.append(d);
            } else if (env->IsInstanceOf(jElement, listClass)) {
                Array a = NUtils::toArray(jElement);
                arr.append(a);
            }
        }
    }

    env->PopLocalFrame(NULL);

    return arr;
}

Array NUtils::toArray(const NArg &arg) {
    return NUtils::toArray(arg.o->getObject());
}

Array NUtils::toArray(const NRet &ret) {
    return NUtils::toArray(ret.o->getObject());
}


string NUtils::toString(void *o) {
    JNIEnv *env = AndroidHelper::getEnv();
    jstring jobj = (jstring)o;
    const char *cstr = env->GetStringUTFChars(jobj, NULL);
    string str = string(cstr);
    env->ReleaseStringUTFChars(jobj, cstr);
    return str;
}

string NUtils::toString(const NArg &arg) {
    return NUtils::toString(arg.o->getObject());
}

string NUtils::toString(const NRet &ret) {
    return NUtils::toString(ret.o->getObject());
}
