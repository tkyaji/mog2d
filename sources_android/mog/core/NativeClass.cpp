#include <algorithm>
#include <assert.h>
#include <string.h>
#include "mog/core/NativeClass.h"
#include "mog/core/Engine.h"
#include "mog/os/JNILocalFrame.h"
#include "mog/os/AndroidHelper.h"

using namespace mog;

static void toNativeDictionary(const std::shared_ptr<Dictionary> &dictionary, jobject jMap);
static void toNativeList(const std::shared_ptr<List> &list, jobject jList);

static void toMogDictionary(jobject jMap, std::shared_ptr<Dictionary> &dict);
static void toMogList(jobject jList, std::shared_ptr<List> &list);

static void convertToNArg(JNIEnv* env, jobjectArray params, std::shared_ptr<List> &list);

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
    std::string methodName = "";
    jvalue *values = nullptr;
    std::string signature = "";
    bool valid = false;
    DataType retType;
    MethodType methodType;

    bool buildParams(jobject jmethod, const std::shared_ptr<List> &args) {
        jclass jmethodCls = this->env->GetObjectClass(jmethod);
        jmethodID getNameId = this->env->GetMethodID(jmethodCls, "getName", "()Ljava/lang/String;");

        jmethodID getParameterTypesId = this->env->GetMethodID(jmethodCls, "getParameterTypes", "()[Ljava/lang/Class;");
        jobjectArray jclasses = (jobjectArray )this->env->CallObjectMethod(jmethod, getParameterTypesId);
        int classesLength = this->env->GetArrayLength(jclasses);
        if (classesLength != args->size()) return false;

        jclass clsCls = this->env->FindClass("java/lang/Class");
        jmethodID clsGetNameId = this->env->GetMethodID(clsCls, "getName", "()Ljava/lang/String;");

        this->signature = "(";
        for (int j = 0; j < classesLength; j++) {
            auto arg = args->at<Data>(j);
            jclass jparamCls = (jclass)this->env->GetObjectArrayElement(jclasses, j);
            jstring jparamClsName = (jstring)this->env->CallObjectMethod(jparamCls, clsGetNameId);
            const char *jparamClsNameCStr = this->env->GetStringUTFChars(jparamClsName, NULL);
            std::string jParamClsNameStr = std::string(jparamClsNameCStr);
            this->env->ReleaseStringUTFChars(jparamClsName, jparamClsNameCStr);

            if (!this->buildArg(arg, jparamCls, jParamClsNameStr, j)) {
                return false;
            }
        }
        this->signature += ")";

        return true;
    }


    bool buildArg(const std::shared_ptr<Data> &arg, jclass jcls, std::string jClsName, int idx) {
        if (arg->type == DataType::Int && jClsName == "int") {
            this->values[idx].i = (jint)std::static_pointer_cast<mog::Int>(arg)->getValue();
            this->signature += "I";
        } else if (arg->type == DataType::Long && jClsName == "long") {
            this->values[idx].j = (jlong)std::static_pointer_cast<mog::Long>(arg)->getValue();
            this->signature += "J";
        } else if (arg->type == DataType::Float && jClsName == "float") {
            this->values[idx].f = (jfloat)std::static_pointer_cast<mog::Float>(arg)->getValue();
            this->signature += "F";
        } else if (arg->type == DataType ::Double && jClsName == "double") {
            this->values[idx].d = (jdouble)std::static_pointer_cast<mog::Double>(arg)->getValue();
            this->signature += "D";
        } else if (arg->type == DataType::Bool && jClsName == "boolean") {
            this->values[idx].z = (jboolean)std::static_pointer_cast<mog::Bool>(arg)->getValue();
            this->signature += "Z";
        } else if (jClsName == "byte") {
            this->values[idx].b = (jbyte)std::static_pointer_cast<mog::Int>(arg)->getValue();
            this->signature += "B";
        } else if (jClsName == "short") {
            this->values[idx].s = (jshort)std::static_pointer_cast<mog::Int>(arg)->getValue();
            this->signature += "S";
        } else if (jClsName == "char") {
            this->values[idx].c = (jchar)std::static_pointer_cast<mog::Int>(arg)->getValue();
            this->signature += "C";
        } else if (arg->type == DataType::NativeObject && this->env->IsInstanceOf((jobject)std::static_pointer_cast<NativeObject>(arg)->getValue(), jcls)) {
            this->values[idx].l = (jobject)std::static_pointer_cast<NativeObject>(arg)->getValue();
            std::replace(jClsName.begin(), jClsName.end(), '.', '/');
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
        std::string retJcNameStr = std::string(retJcNameCStr);
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
            std::string typeStr = "L" + retJcNameStr + ";";
            this->signature += typeStr.c_str();
            this->methodType = MethodType::Object;
        }
        return true;
    }
};


#pragma - JavaMethod

class JavaMethod : public JavaMethodInterface {
public:
    JavaMethod(jobject jobj, std::string methodName, DataType retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jobj;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, List::create(), retType);
    }

    JavaMethod(jobject jobj, std::string methodName, const std::shared_ptr<List> &args, DataType retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jobj;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, args, retType);
    }

    JavaMethod(jclass jcls, std::string methodName, DataType retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, List::create(), retType);
    }

    JavaMethod(jclass jcls, std::string methodName, const std::shared_ptr<List> &args, DataType retType) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = methodName;
        this->retType = retType;
        this->build(methodName, args, retType);
    }

    std::shared_ptr<Data> invoke() {
        assert(this->valid);

        if (this->isStatic) {
            return this->invokeStaticMethod();
        } else {
            return this->invokeMethod();
        }
    }

protected:
    void build(std::string methodName, const std::shared_ptr<List> &args, DataType retType) {
        auto lf0 = JNILocalFrame::create(this->env, 32);

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

        this->values = new jvalue[args->size()];
        int methodsLength = this->env->GetArrayLength(methods);
        for (int i = 0; i < methodsLength; i++) {
            auto lf1 = JNILocalFrame::create(this->env, 16);

            jobject jmethod = this->env->GetObjectArrayElement(methods, i);

            jstring jmethodName = (jstring)this->env->CallObjectMethod(jmethod, getNameId);
            const char *methodNameCStr = this->env->GetStringUTFChars(jmethodName, NULL);
            std::string methodNameStr = std::string(methodNameCStr);
            this->env->ReleaseStringUTFChars(jmethodName, methodNameCStr);
            if (methodName != methodNameStr) {
                continue;
            }

            if (!this->buildParams(jmethod, args)) {
                continue;
            }
            if (!this->buildReturnType(jmethod, jmethodCls)) {
                continue;
            }

            this->valid = true;
            break;
        }
    }

    std::shared_ptr<Data> invokeMethod() {
        jclass jcls = this->env->GetObjectClass(this->jobj);
        jmethodID methodId = this->env->GetMethodID(jcls, this->methodName.c_str(), this->signature.c_str());
        assert(methodId);

        switch (this->methodType) {
            case MethodType::Int: {
                int i = this->env->CallIntMethodA(this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Long: {
                long long l = this->env->CallLongMethodA(this->jobj, methodId, this->values);
                return Long::create(l);
            }
            case MethodType::Short: {
                int i = this->env->CallShortMethodA(this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Byte: {
                int i = this->env->CallByteMethodA(this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Char: {
                int i = this->env->CallCharMethodA(this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Float: {
                float f = this->env->CallFloatMethodA(this->jobj, methodId, this->values);
                return Float::create(f);
            }
            case MethodType::Double: {
                double d = this->env->CallDoubleMethodA(this->jobj, methodId, this->values);
                return Double::create(d);
            }
            case MethodType::Boolean: {
                bool b = this->env->CallBooleanMethodA(this->jobj, methodId, this->values);
                return Bool::create(b);
            }
            case MethodType::Object: {
                jobject o = this->env->CallObjectMethodA(this->jobj, methodId, this->values);
                return NativeObject::create(o);
            }
            case MethodType::Void:
                this->env->CallVoidMethodA(this->jobj, methodId, this->values);
                return nullptr;
        }
    }

    std::shared_ptr<Data> invokeStaticMethod() {
        std::shared_ptr<Data> ret = nullptr;

        jclass jcls = (jclass)this->jobj;
        jmethodID methodId = this->env->GetStaticMethodID(jcls, this->methodName.c_str(), this->signature.c_str());
        assert(methodId);

        switch (this->methodType) {
            case MethodType::Int: {
                int i = this->env->CallStaticIntMethodA((jclass) this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Long: {
                long l = this->env->CallStaticLongMethodA((jclass)this->jobj, methodId, this->values);
                return Long::create(l);
            }
            case MethodType::Short: {
                int i = this->env->CallStaticShortMethodA((jclass)this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Byte: {
                int i = this->env->CallStaticByteMethodA((jclass)this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Char: {
                int i = this->env->CallStaticCharMethodA((jclass)this->jobj, methodId, this->values);
                return Int::create(i);
            }
            case MethodType::Float: {
                float f = this->env->CallStaticFloatMethodA((jclass)this->jobj, methodId, this->values);
                return Float::create(f);
            }
            case MethodType::Double: {
                double d = this->env->CallStaticDoubleMethodA((jclass)this->jobj, methodId, this->values);
                return Double::create(d);
            }
            case MethodType::Boolean: {
                bool b = this->env->CallStaticBooleanMethodA((jclass)this->jobj, methodId, this->values);
                return Bool::create(b);
            }
            case MethodType::Object: {
                jobject o = this->env->CallStaticObjectMethodA((jclass)this->jobj, methodId, this->values);
                return NativeObject::create(o);
            }
            case MethodType::Void: {
                return nullptr;
            }
        }
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
        this->retType = DataType::Void;
        this->build(List::create(), this->retType);
    }

    JavaConstructor(jclass jcls, const std::shared_ptr<List> &args) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jcls;
        this->isStatic = true;
        this->methodName = "<init>";
        this->retType = DataType::Void;
        this->build(args, this->retType);
    }

    std::shared_ptr<NativeObject> newInstance() {
        assert(this->valid);

        jmethodID init = this->env->GetMethodID((jclass)this->jobj, "<init>", this->signature.c_str());
        jobject jobj = this->env->NewObjectA((jclass)this->jobj, init, this->values);

        auto no = NativeObject::create(jobj);
        env->DeleteLocalRef(jobj);
        return no;
    }

protected:
    void build(const std::shared_ptr<List> &args, DataType retType) {
        auto lf0 = JNILocalFrame::create(this->env, 32);

        jclass jcls = (jclass)this->jobj;
        jclass jclsCls = this->env->GetObjectClass(jcls);

        jmethodID getConstructorsId = this->env->GetMethodID(jclsCls, "getConstructors", "()[Ljava/lang/reflect/Constructor;");
        jobjectArray constructors = (jobjectArray)this->env->CallObjectMethod(jcls, getConstructorsId);

        jclass jconstructorCls = this->env->FindClass("java/lang/reflect/Constructor");
        jmethodID getNameId = this->env->GetMethodID(jconstructorCls, "getName", "()Ljava/lang/String;");

        this->values = new jvalue[args->size()];
        int methodsLength = this->env->GetArrayLength(constructors);
        for (int i = 0; i < methodsLength; i++) {
            auto lf0 = JNILocalFrame::create(this->env, 16);

            jobject jmethod = this->env->GetObjectArrayElement(constructors, i);
            if (!this->buildParams(jmethod, args)) {
                continue;
            }
            this->signature += "V";

            this->valid = true;
            break;
        }
    }
};


#pragma - JavaField

class JavaField {
public:
    JNIEnv *env = nullptr;
    jobject jobj = nullptr;
    jclass jcls = nullptr;
    std::string fieldName;
    DataType type;

    JavaField(jobject jobj, std::string fieldName, DataType type) {
        this->env = AndroidHelper::getEnv();
        this->jobj = jobj;
        this->jcls = this->env->GetObjectClass(jobj);
        this->fieldName = fieldName;
        this->type = type;
    }

    JavaField(jclass jcls, std::string fieldName, DataType type) {
        this->env = AndroidHelper::getEnv();
        this->jcls = jcls;
        this->fieldName = fieldName;
        this->type = type;
    }

    std::shared_ptr<Data> getValue() {
        auto lf0 = JNILocalFrame::create(this->env, 16);

        jstring jFieldName = this->env->NewStringUTF(this->fieldName.c_str());
        jclass jclscls = this->env->GetObjectClass(this->jcls);
        jmethodID getFieldId = this->env->GetMethodID(jclscls, "getField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
        jobject jField = this->env->CallObjectMethod(this->jcls, getFieldId, jFieldName);
        jclass jFieldCls = this->env->GetObjectClass(jField);

        std::shared_ptr<Data> data = nullptr;

        switch (this->type) {
            case DataType::Void:
                break;
            case DataType::Int: {
                jmethodID getIntId = this->env->GetMethodID(jFieldCls, "getInt", "(Ljava/lang/Object;)I");
                jint i = env->CallIntMethod(jField, getIntId, this->jobj);
                data = Int::create((int)i);
                break;
            }
            case DataType::Long: {
                jmethodID getLongId = this->env->GetMethodID(jFieldCls, "getLong", "(Ljava/lang/Object;)J");
                jlong j = env->CallLongMethod(jField, getLongId, this->jobj);
                data = Long::create((long long)j);
                break;
            }
            case DataType::Float: {
                jmethodID getFloatId = this->env->GetMethodID(jFieldCls, "getFloat", "(Ljava/lang/Object;)F");
                jfloat f = env->CallFloatMethod(jField, getFloatId, this->jobj);
                data = Float::create((float)f);
                break;
            }
            case DataType::Double: {
                jmethodID getDoubleId = this->env->GetMethodID(jFieldCls, "getDouble", "(Ljava/lang/Object;)D");
                jdouble d = env->CallDoubleMethod(jField, getDoubleId, this->jobj);
                data = Double::create((double)d);
                break;
            }
            case DataType::Bool: {
                jmethodID getBoolId = this->env->GetMethodID(jFieldCls, "getBoolean", "(Ljava/lang/Object;)Z");
                jboolean b = env->CallBooleanMethod(jField, getBoolId, this->jobj);
                data = Bool::create((bool)b);
                break;
            }
            case DataType::String: {
                jmethodID getStringId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject s = env->CallObjectMethod(jField, getStringId, this->jobj);
                const char *str = env->GetStringUTFChars((jstring)s, NULL);
                data = String::create(str);
                this->env->ReleaseStringUTFChars((jstring)s, str);
                break;
            }
            case DataType::List: {
                jmethodID getListId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject l = env->CallObjectMethod(jField, getListId, this->jobj);
                auto list = List::create();
                toMogList(l, list);
                data = list;
                break;
            }
            case DataType::Dictionary: {
                jmethodID getMapId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject m = env->CallObjectMethod(jField, getMapId, this->jobj);
                auto dict = Dictionary::create();
                toMogDictionary(m, dict);
                data = dict;
                break;
            }
            case DataType::ByteArray: {
                jmethodID getBytesId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject ba = env->CallObjectMethod(jField, getBytesId, this->jobj);
                jbyteArray jByteArr = (jbyteArray)ba;
                jbyte *bytes = env->GetByteArrayElements(jByteArr, NULL);
                int length = (int)env->GetArrayLength(jByteArr);
                auto byteArr = ByteArray::create((unsigned char *)bytes, (unsigned int)length, true);
                env->ReleaseByteArrayElements(jByteArr, bytes, JNI_ABORT);
                data = byteArr;
                break;
            }
            case DataType::NativeObject: {
                jmethodID getObjId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject o = env->CallObjectMethod(jField, getObjId, this->jobj);
                data = NativeObject::create((void *)o);
                break;
            }
        }

        return data;
    }

    void setValue(std::shared_ptr<Data> value) {
        auto lf0 = JNILocalFrame::create(this->env, 16);

        jstring jFieldName = this->env->NewStringUTF(this->fieldName.c_str());
        jclass jclscls = this->env->GetObjectClass(this->jcls);
        jmethodID getFieldId = this->env->GetMethodID(jclscls, "getField", "(Ljava/lang/String;)Ljava/lang/reflect/Field;");
        jobject jField = this->env->CallObjectMethod(this->jcls, getFieldId, jFieldName);
        jclass jFieldCls = this->env->GetObjectClass(jField);

        std::shared_ptr<Data> data = nullptr;

        switch (this->type) {
            case DataType::Void:
                break;
            case DataType::Int: {
                jmethodID setIntId = this->env->GetMethodID(jFieldCls, "setInt", "(Ljava/lang/Object;I)V");
                int i = std::static_pointer_cast<Int>(value)->getValue();
                env->CallVoidMethod(jField, setIntId, this->jobj, (jint)i);
                break;
            }
            case DataType::Long: {
                jmethodID setLongId = this->env->GetMethodID(jFieldCls, "setLong", "(Ljava/lang/Object;J)V");
                long long j = std::static_pointer_cast<Long>(value)->getValue();
                env->CallVoidMethod(jField, setLongId, this->jobj, (jlong)j);
                break;
            }
            case DataType::Float: {
                jmethodID setFloatId = this->env->GetMethodID(jFieldCls, "setFloat", "(Ljava/lang/Object;F)V");
                float f = std::static_pointer_cast<Float>(value)->getValue();
                env->CallVoidMethod(jField, setFloatId, this->jobj, (jfloat)f);
                break;
            }
            case DataType::Double: {
                jmethodID setDoubleId = this->env->GetMethodID(jFieldCls, "setDouble", "(Ljava/lang/Object;D)V");
                double d = std::static_pointer_cast<Double>(value)->getValue();
                env->CallVoidMethod(jField, setDoubleId, this->jobj, (jdouble)d);
                break;
            }
            case DataType::Bool: {
                jmethodID setBoolId = this->env->GetMethodID(jFieldCls, "setBoolean", "(Ljava/lang/Object;Z)V");
                bool z = std::static_pointer_cast<Bool>(value)->getValue();
                env->CallVoidMethod(jField, setBoolId, this->jobj, (jboolean)z);
                break;
            }
            case DataType::String: {
                jmethodID setId = this->env->GetMethodID(jFieldCls, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
                std::string s = std::static_pointer_cast<String>(value)->getValue();
                jstring str = this->env->NewStringUTF(s.c_str());
                env->CallVoidMethod(jField, setId, this->jobj, (jobject)str);
                break;
            }
            case DataType::List: {
                jmethodID setId = this->env->GetMethodID(jFieldCls, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
                auto l = std::static_pointer_cast<List>(value);
                jclass listClass = env->FindClass("java/util/ArrayList");
                jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");
                jobject jList = env->NewObject(listClass, listInit);
                toNativeList(l, jList);
                env->CallVoidMethod(jField, setId, this->jobj, jList);
                break;
            }
            case DataType::Dictionary: {
                jmethodID setId = this->env->GetMethodID(jFieldCls, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
                auto d = std::static_pointer_cast<Dictionary>(value);
                jclass mapClass = env->FindClass("java/util/HashMap");
                jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
                jobject jMap = env->NewObject(mapClass, mapInit);
                toNativeDictionary(d, jMap);
                env->CallVoidMethod(jField, setId, this->jobj, jMap);
                break;
            }
            case DataType::ByteArray: {
                jmethodID setId = this->env->GetMethodID(jFieldCls, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
                auto ba = std::static_pointer_cast<ByteArray>(value);
                unsigned char *data = nullptr;
                unsigned int len = 0;
                ba->getValue(&data, &len);
                jbyteArray jbyteArr = env->NewByteArray((jsize)len);
                jbyte *jBytes = env->GetByteArrayElements(jbyteArr, NULL);
                for (int i = 0; i < len; i++) {
                    jBytes[i] = (jbyte)data[i];
                }
                env->CallVoidMethod(jField, setId, this->jobj, (jobject)jbyteArr);
                env->ReleaseByteArrayElements(jbyteArr, jBytes, 0);
                break;
            }
            case DataType::NativeObject: {
                jmethodID setId = this->env->GetMethodID(jFieldCls, "set", "(Ljava/lang/Object;Ljava/lang/Object;)V");
                jmethodID getObjId = this->env->GetMethodID(jFieldCls, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
                jobject o = (jobject)std::static_pointer_cast<NativeObject>(value)->getValue();
                env->CallVoidMethod(jField, setId, this->jobj, o);
                break;
            }
        }
    }
};


#pragma - NativeClass

std::shared_ptr<NativeClass> NativeClass::create(std::string className) {
    return std::shared_ptr<NativeClass>(new NativeClass(className));
}

NativeClass::NativeClass(std::string className) {
    replace(className.begin(), className.end(), '.', '/');
    JNIEnv *env = AndroidHelper::getEnv();
    jclass jcls = env->FindClass(className.c_str());
    assert(jcls);
    jobject jgo = env->NewGlobalRef(jcls);
    env->DeleteLocalRef(jcls);
    this->cls = (void *)jgo;
}

NativeClass::~NativeClass() {
    if (this->cls) {
        JNIEnv *env = AndroidHelper::getEnv();
        env->DeleteGlobalRef((jclass)this->cls);
    }
}

std::shared_ptr<NativeObject> NativeClass::newInstance() {
    return JavaConstructor((jclass)this->cls).newInstance();
}

std::shared_ptr<NativeObject> NativeClass::newInstanceWithList(const std::shared_ptr<mog::List> &args) {
    return JavaConstructor((jclass)this->cls, args).newInstance();
}

void NativeClass::execute(std::string methodName) {
    JavaMethod((jclass)this->cls, methodName, List::create(), DataType::Void).invoke();
}

void NativeClass::executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
    JavaMethod((jclass)this->cls, methodName, args, DataType::Void).invoke();
}

void NativeClass::setProperty(std::string propertyName, const std::shared_ptr<Data> &value) {
    JavaField((jclass)this->cls, propertyName, value->type).setValue(value);
}

std::shared_ptr<mog::Data> NativeClass::executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType) {
    return JavaMethod((jclass)this->cls, methodName, args, retType).invoke();
}

std::shared_ptr<mog::Data> NativeClass::getPropertyMain(std::string propertyName, DataType retType) {
    return JavaField((jclass)this->cls, propertyName, retType).getValue();
}


#pragma - NativeObject

NativeObject::NativeObject(void *value) {
    JNIEnv *env = AndroidHelper::getEnv();
    jobject go = env->NewGlobalRef((jobject)value);
    this->value = go;
}

NativeObject::~NativeObject() {
    if (this->value) {
        JNIEnv *env = AndroidHelper::getEnv();
        env->DeleteGlobalRef((jobject)this->value);
    }
}

std::shared_ptr<NativeObject> NativeObject::create(void *value) {
    return std::shared_ptr<NativeObject>(new NativeObject(value));
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<Dictionary> &dict) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 16);
    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    jobject jMap = env->NewObject(mapClass, mapInit);
    toNativeDictionary(dict, jMap);
    auto no = create((void *)jMap);
    return no;
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<List> &list) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 16);
    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");
    jobject jList = env->NewObject(listClass, listInit);
    toNativeList(list, jList);
    auto no = create((void *)jList);
    return no;
}

std::shared_ptr<NativeObject> NativeObject::create(const std::shared_ptr<ByteArray> &bytes) {
    JNIEnv *env = AndroidHelper::getEnv();
    unsigned char *data = nullptr;
    unsigned int length = 0;
    bytes->getValue(&data, &length);
    jbyteArray jbyteArr = env->NewByteArray((jsize)length);
    jbyte *jBytes = env->GetByteArrayElements(jbyteArr, NULL);
    for (int i = 0; i < length; i++) {
        jBytes[i] = (jbyte)data[i];
    }
    auto no = create((void *)jbyteArr);
    env->ReleaseByteArrayElements(jbyteArr, jBytes, 0);
    env->DeleteLocalRef(jbyteArr);
    return no;
}

std::shared_ptr<NativeObject> NativeObject::create(std::function<void(const std::shared_ptr<mog::List> &args)> func) {
    auto nativeFunc = new NativeFunction(func);
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 16);
    jclass mogFuncClass = env->FindClass("org/mog/MogFunction");
    jmethodID mogFuncInit = env->GetMethodID(mogFuncClass, "<init>", "(J)V");
    jobject mogFuncObj = env->NewObject(mogFuncClass, mogFuncInit, (long long)nativeFunc);
    auto mogFunc = create((void *)mogFuncObj);
    return mogFunc;
}

void *NativeObject::getValue() {
    return this->value;
}

void NativeObject::execute(std::string methodName) {
    JavaMethod((jobject)this->value, methodName, List::create(), DataType::Void).invoke();
}

void NativeObject::executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
    JavaMethod((jobject)this->value, methodName, args, DataType::Void).invoke();
}

void NativeObject::setProperty(std::string propertyName, const std::shared_ptr<Data> &value) {
    JavaField((jobject)this->value, propertyName, value->type).setValue(value);
}

std::shared_ptr<mog::Data> NativeObject::executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType) {
    return JavaMethod((jobject)this->value, methodName, args, retType).invoke();
}

std::shared_ptr<mog::Data> NativeObject::getPropertyMain(std::string propertyName, DataType retType) {
    return JavaField((jobject)this->value, propertyName, retType).getValue();
}


#pragma - NativeFunction

NativeFunction::NativeFunction(std::function<void(const std::shared_ptr<mog::List> &args)> func) {
    this->func = func;
}

void NativeFunction::invoke(const std::shared_ptr<mog::List> &args) {
    this->func(args);
}

static void toNativeDictionary(const std::shared_ptr<Dictionary> &dictionary, jobject jMap) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 16);

    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    jmethodID putMethod = env->GetMethodID(mapClass, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    jclass listClass = env->FindClass("java/util/ArrayList");
    jmethodID listInit = env->GetMethodID(listClass, "<init>", "()V");

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

    auto keys = dictionary->getKeys();
    for (auto &key : keys) {
        auto lf1 = JNILocalFrame::create(env, 16);

        jvalue v;
        jstring keyStr = env->NewStringUTF(key.c_str());
        auto type = dictionary->getType(key);

        switch (type) {
            case DataType::Void:
                break;
            case DataType::Int: {
                jobject integerObj = env->NewObject(integerClass, integerInit, dictionary->get<mog::Int>(key)->getValue());
                env->CallObjectMethod(jMap, putMethod, keyStr, integerObj);
                break;
            }
            case DataType::Long: {
                jobject longObj = env->NewObject(longClass, longInit, dictionary->get<mog::Long>(key)->getValue());
                env->CallObjectMethod(jMap, putMethod, keyStr, longObj);
                break;
            }
            case DataType::Float: {
                jobject floatObj = env->NewObject(floatClass, floatInit, dictionary->get<mog::Float>(key)->getValue());
                env->CallObjectMethod(jMap, putMethod, keyStr, floatObj);
                break;
            }
            case DataType::Double: {
                jobject doubleObj = env->NewObject(doubleClass, doubleInit, dictionary->get<mog::Double>(key)->getValue());
                env->CallObjectMethod(jMap, putMethod, keyStr, doubleObj);
                break;
            }
            case DataType::Bool: {
                jobject booleanObj = env->NewObject(booleanClass, booleanInit, dictionary->get<mog::Bool>(key)->getValue());
                env->CallObjectMethod(jMap, putMethod, keyStr, booleanObj);
                break;
            }
            case DataType::String: {
                const char *s = dictionary->get<mog::String>(key)->getValue().c_str();
                jstring str = env->NewStringUTF(s);
                env->CallObjectMethod(jMap, putMethod, keyStr, str);
                break;
            }
            case DataType::List: {
                jobject jList = env->NewObject(listClass, listInit);
                toNativeList(dictionary->get<mog::List>(key), jList);
                env->CallObjectMethod(jMap, putMethod, keyStr, jList);
                break;
            }
            case DataType::Dictionary: {
                jobject jMap2 = env->NewObject(mapClass, mapInit);
                toNativeDictionary(dictionary->get<mog::Dictionary>(key), jMap2);
                env->CallObjectMethod(jMap, putMethod, keyStr, jMap2);
                break;
            }
            case DataType::ByteArray: {
                unsigned char *data = nullptr;
                unsigned int len = 0;
                dictionary->get<mog::ByteArray>(key)->getValue(&data, &len);
                jbyteArray jbyteArr = env->NewByteArray((jsize)len);
                jbyte *jBytes = env->GetByteArrayElements(jbyteArr, NULL);
                for (int i = 0; i < len; i++) {
                    jBytes[i] = (jbyte)data[i];
                }
                env->CallObjectMethod(jMap, putMethod, keyStr, jbyteArr);
                break;
            }
            case DataType::NativeObject: {
                auto no = dictionary->get<mog::NativeObject>(key);
                jobject jno = (jobject)no->getValue();
                env->CallObjectMethod(jMap, putMethod, keyStr, jno);
                break;
            }
        }
    }
}

static void toNativeList(const std::shared_ptr<List> &list, jobject jList) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 16);

    jclass mapClass = env->FindClass("java/util/HashMap");
    jmethodID mapInit = env->GetMethodID(mapClass, "<init>", "()V");

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

    for (int i = 0; i < list->size(); i++) {
        auto lf1 = JNILocalFrame::create(env, 16);

        jvalue v;
        auto type = list->atType(i);

        switch (type) {
            case DataType::Void:
                break;
            case DataType::Int: {
                jobject integerObj = env->NewObject(integerClass, integerInit, list->at<mog::Int>(i)->getValue());
                env->CallBooleanMethod(jList, addMethod, integerObj);
                break;
            }
            case DataType::Long: {
                jobject longObj = env->NewObject(longClass, longInit, list->at<mog::Long>(i)->getValue());
                env->CallBooleanMethod(jList, addMethod, longObj);
                break;
            }
            case DataType::Float: {
                jobject floatObj = env->NewObject(floatClass, floatInit, list->at<mog::Float>(i)->getValue());
                env->CallBooleanMethod(jList, addMethod, floatObj);
                break;
            }
            case DataType::Double: {
                jobject doubleObj = env->NewObject(doubleClass, doubleInit, list->at<mog::Double>(i)->getValue());
                env->CallBooleanMethod(jList, addMethod, doubleObj);
                break;
            }
            case DataType::Bool: {
                jobject booleanObj = env->NewObject(booleanClass, booleanInit, list->at<mog::Bool>(i)->getValue());
                env->CallBooleanMethod(jList, addMethod, booleanObj);
                break;
            }
            case DataType::String: {
                const char *s = list->at<String>(i)->getValue().c_str();
                jstring str = env->NewStringUTF(s);
                env->CallBooleanMethod(jList, addMethod, str);
                break;
            }
            case DataType::List: {
                jobject jList2 = env->NewObject(listClass, listInit);
                toNativeList(list->at<List>(i), jList2);
                env->CallBooleanMethod(jList, addMethod, jList2);
                break;
            }
            case DataType::Dictionary: {
                jobject map = env->NewObject(mapClass, mapInit);
                toNativeDictionary(list->at<Dictionary>(i), map);
                env->CallBooleanMethod(jList, addMethod, map);
                break;
            }
            case DataType::ByteArray: {
                unsigned char *data = nullptr;
                unsigned int len = 0;
                list->at<mog::ByteArray>(i)->getValue(&data, &len);
                jbyteArray jbyteArr = env->NewByteArray((jsize)len);
                jbyte *jBytes = env->GetByteArrayElements(jbyteArr, NULL);
                for (int i = 0; i < len; i++) {
                    jBytes[i] = (jbyte)data[i];
                }
                env->CallBooleanMethod(jList, addMethod, jbyteArr);
            }
            case DataType::NativeObject: {
                auto no = list->at<mog::NativeObject>(i);
                jobject jno = (jobject)no->getValue();
                env->CallBooleanMethod(jList, addMethod, jno);
            }
        }
    }
}

static void toMogDictionary(jobject jMap, std::shared_ptr<Dictionary> &dict) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 128);

    jclass jcls = env->GetObjectClass(jMap);
    jclass mapCls = env->FindClass("java/util/Map");
    if (env->IsInstanceOf(jMap, mapCls)) {
        jmethodID keySetId = env->GetMethodID(jcls, "keySet", "()Ljava/util/Set;");
        jobject jKeySet = env->CallObjectMethod(jMap, keySetId);
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
        jclass byteArrClass = env->FindClass("[B");

        int len = env->GetArrayLength(keyArr);
        for (int i = 0; i < len; i++) {
            jstring jKey = (jstring)env->GetObjectArrayElement(keyArr, i);
            const char *keyCStr = env->GetStringUTFChars(jKey, NULL);
            auto key = std::string(keyCStr);
            jobject jElement = env->CallObjectMethod(jMap, getId, jKey);
            env->ReleaseStringUTFChars(jKey, keyCStr);

            if (!jElement) continue;

            jclass jElmCls = env->GetObjectClass(jElement);

            if (env->IsInstanceOf(jElement, integerClass)) {
                jint i = env->CallIntMethod(jElement, intValueId);
                dict->put(key, mog::Int::create((int)i));
            } else if (env->IsInstanceOf(jElement, longClass)) {
                jlong l = env->CallLongMethod(jElement, longValueId);
                dict->put(key, mog::Long::create((long long)l));
            } else if (env->IsInstanceOf(jElement, charClass)) {
                jchar c = env->CallCharMethod(jElement, charValueId);
                dict->put(key, mog::Int::create((int)c));
            } else if (env->IsInstanceOf(jElement, byteClass)) {
                jbyte b = env->CallByteMethod(jElement, byteValueId);
                dict->put(key, mog::Int::create((int)b));
            } else if (env->IsInstanceOf(jElement, floatClass)) {
                jfloat f = env->CallFloatMethod(jElement, floatValueId);
                dict->put(key, mog::Float::create((float)f));
            } else if (env->IsInstanceOf(jElement, doubleClass)) {
                jdouble d = env->CallDoubleMethod(jElement, doubleValueId);
                dict->put(key, mog::Double::create((double)d));
            } else if (env->IsInstanceOf(jElement, booleanClass)) {
                jboolean b = env->CallBooleanMethod(jElement, booleanValueId);
                dict->put(key, mog::Bool::create((bool)b));
            } else if (env->IsInstanceOf(jElement, stringClass)) {
                const char *cstr = env->GetStringUTFChars((jstring)jElement, NULL);
                dict->put(key, mog::String::create(cstr));
                env->ReleaseStringUTFChars((jstring)jElement, cstr);
            } else if (env->IsInstanceOf(jElement, mapClass)) {
                auto dict2 = Dictionary::create();
                toMogDictionary(jElement, dict2);
                dict->put(key, dict2);
            } else if (env->IsInstanceOf(jElement, listClass)) {
                auto list = List::create();
                toMogList(jElement, list);
                dict->put(key, list);
            } else if (env->IsInstanceOf(jElement, byteArrClass)) {
                jbyteArray jByteArr = (jbyteArray)jElement;
                jbyte *bytes = env->GetByteArrayElements(jByteArr, NULL);
                int length = (int)env->GetArrayLength(jByteArr);
                auto byteArr = ByteArray::create((unsigned char *)bytes, (unsigned int)length, true);
                env->ReleaseByteArrayElements(jByteArr, bytes, JNI_ABORT);
                dict->put(key, byteArr);
            }
        }
    }
}

static void toMogList(jobject jList, std::shared_ptr<List> &list) {
    JNIEnv *env = AndroidHelper::getEnv();
    auto lf0 = JNILocalFrame::create(env, 128);

    jclass jcls = env->GetObjectClass(jList);
    jclass listCls = env->FindClass("java/util/List");
    if (env->IsInstanceOf(jList, listCls)) {
        jmethodID sizeId = env->GetMethodID(jcls, "size", "()I");
        int size = env->CallIntMethod(jList, sizeId);
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
        jclass byteArrClass = env->FindClass("[B");

        for (int i = 0; i < size; i++) {
            jobject jElement = env->CallObjectMethod(jList, getId, (jint)i);
            jclass jElmCls = env->GetObjectClass(jElement);

            if (env->IsInstanceOf(jElement, integerClass)) {
                jint i = env->CallIntMethod(jElement, intValueId);
                list->append(Int::create((int)i));
            } else if (env->IsInstanceOf(jElement, longClass)) {
                jlong l = env->CallLongMethod(jElement, longValueId);
                list->append(Long::create((long long)l));
            } else if (env->IsInstanceOf(jElement, charClass)) {
                jchar c = env->CallCharMethod(jElement, charValueId);
                list->append(Int::create((int)c));
            } else if (env->IsInstanceOf(jElement, byteClass)) {
                jbyte b = env->CallByteMethod(jElement, byteValueId);
                list->append(Int::create((int)b));
            } else if (env->IsInstanceOf(jElement, floatClass)) {
                jfloat f = env->CallFloatMethod(jElement, floatValueId);
                list->append(Float::create((float)f));
            } else if (env->IsInstanceOf(jElement, doubleClass)) {
                jdouble d = env->CallDoubleMethod(jElement, doubleValueId);
                list->append(Double::create((double)d));
            } else if (env->IsInstanceOf(jElement, booleanClass)) {
                jboolean b = env->CallBooleanMethod(jElement, booleanValueId);
                list->append(Bool::create((bool)b));
            } else if (env->IsInstanceOf(jElement, stringClass)) {
                const char *cstr = env->GetStringUTFChars((jstring)jElement, NULL);
                list->append(String::create(cstr));
                env->ReleaseStringUTFChars((jstring)jElement, cstr);
            } else if (env->IsInstanceOf(jElement, mapClass)) {
                auto dict = Dictionary::create();
                toMogDictionary(jElement, dict);
                list->append(dict);
            } else if (env->IsInstanceOf(jElement, listClass)) {
                auto list2 = List::create();
                toMogList(jElement, list2);
                list->append(list2);
            } else if (env->IsInstanceOf(jElement, byteArrClass)) {
                jbyteArray jByteArr = (jbyteArray)jElement;
                jbyte *bytes = env->GetByteArrayElements(jByteArr, NULL);
                int length = (int)env->GetArrayLength(jByteArr);
                auto byteArr = ByteArray::create((unsigned char *)bytes, (unsigned int)length, true);
                env->ReleaseByteArrayElements(jByteArr, bytes, JNI_ABORT);
                list->append(byteArr);
            }
        }
    }
}


static void convertArgs(JNIEnv* env, jobjectArray params, std::shared_ptr<List> &list) {
    auto lf0 = JNILocalFrame::create(env, 128);

    int len = env->GetArrayLength(params);

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
    jclass byteArrClass = env->FindClass("[B");

    for (int i = 0; i < len; i++) {
        jobject jobj = env->GetObjectArrayElement(params, i);
        if (jobj == NULL) {
            list->append(nullptr);
        } else if (env->IsInstanceOf(jobj, integerClass)) {
            jint v = env->CallIntMethod(jobj, intValueId);
            list->append(Int::create((int)v));
        } else if (env->IsInstanceOf(jobj, longClass)) {
            jlong v = env->CallLongMethod(jobj, longValueId);
            list->append(Long::create((long long)v));
        } else if (env->IsInstanceOf(jobj, charClass)) {
            jchar v = env->CallCharMethod(jobj, charValueId);
            list->append(Int::create((int)v));
        } else if (env->IsInstanceOf(jobj, byteClass)) {
            jbyte v = env->CallByteMethod(jobj, byteValueId);
            list->append(Int::create((int)v));
        } else if (env->IsInstanceOf(jobj, floatClass)) {
            jfloat v = env->CallFloatMethod(jobj, floatValueId);
            list->append(Float::create((float)v));
        } else if (env->IsInstanceOf(jobj, doubleClass)) {
            jdouble v = env->CallDoubleMethod(jobj, doubleValueId);
            list->append(Double::create((double)v));
        } else if (env->IsInstanceOf(jobj, booleanClass)) {
            jboolean v = env->CallBooleanMethod(jobj, booleanValueId);
            list->append(Bool::create((bool)v));
        } else if (env->IsInstanceOf(jobj, byteArrClass)) {
            jbyteArray jByteArr = (jbyteArray)jobj;
            jbyte *bytes = env->GetByteArrayElements(jByteArr, NULL);
            int length = (int)env->GetArrayLength(jByteArr);
            auto byteArr = ByteArray::create((unsigned char *)bytes, (unsigned int)length, true);
            env->ReleaseByteArrayElements(jByteArr, bytes, JNI_ABORT);
            list->append(byteArr);
        } else {
            jobject go = env->NewGlobalRef(jobj);
            list->append(NativeObject::create((void *)go));
        }
    }
}


extern "C" {
JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_runCallback(JNIEnv* env, jobject obj, jlong functionPtr, jobjectArray params) {
    NativeFunction *nf = (NativeFunction *)functionPtr;
    auto args = List::create();
    convertArgs(env, params, args);
    nf->invoke(args);
}

JNIEXPORT void JNICALL Java_org_mog2d_MogJniBridge_releaseNativeFunction(JNIEnv* env, jobject obj, jlong functionPtr) {
    delete (NativeFunction *)functionPtr;
}
}