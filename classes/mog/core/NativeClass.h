#ifndef NativeClass_h
#define NativeClass_h

#include <string>
#include <memory>
#include "mog/core/Data.h"

using namespace std;

namespace mog {

    class NativeObject;
    class NativeClass;
    class NArg;

#pragma - NFunction

    class NFunction {
    public:
        NFunction(function<void(NArg *args, int len)> func);
        void invoke(NArg *args, int len);

    private:
        function<void(NArg *args, int len)> func;
    };


#pragma - NArg

    class NArg {
    public:
        friend class NativeClass;
        friend class NativeObject;
        friend class NRet;

        enum class Type {
            Int,
            Long,
            Float,
            Double,
            Bool,
            Object,
        };

        union Val {
            int i;
            long l;
            float f;
            double d;
            bool b;
        };

        Type type;
        Val v;
        shared_ptr<NativeObject> o;

        NArg();
        NArg(int v);
        NArg(long v);
        NArg(float v);
        NArg(double v);
        NArg(bool v);
        NArg(const char *v);
        NArg(string v);
        NArg(const Dictionary &v);
        NArg(const Array &v);
        NArg(const shared_ptr<NativeObject> &v);
        NArg(void *v);
        NArg(function<void(NArg *args, int len)> func);
        ~NArg();

//    private:

//        class Impl;
//        shared_ptr<Impl> pImpl;
    };


#pragma - NRet

    class NRet {
    public:
        enum class Type {
            Void,
            Int,
            Long,
            Float,
            Double,
            Bool,
            Object,
        };

        union Val {
            int i;
            long l;
            float f;
            double d;
            bool b;
        };

        Type type;
        Val v;
        shared_ptr<NativeObject> o;

        NRet();
        ~NRet();

//        NativeObject getNativeObject();

//        NRet();
//        ~NRet();

        /*
    private:
        class Impl;
        shared_ptr<Impl> pImpl;
         */
    };


#pragma - Native


#pragma - NativeClass

    class NativeClass {
    public:
        friend class NativeObject;

        static shared_ptr<NativeClass> create(string className);

//        NativeClass(string className);
        ~NativeClass();

        shared_ptr<NativeObject> newInstance();
        shared_ptr<NativeObject> newInstance(const NArg *args, int len);
        shared_ptr<NativeObject> newInstance(const NArg &arg1);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                 const NArg &arg6);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                 const NArg &arg6, const NArg &arg7);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                 const NArg &arg6, const NArg &arg7, const NArg &arg8);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                 const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9);
        shared_ptr<NativeObject> newInstance(const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4, const NArg &arg5,
                                 const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9, const NArg &arg10);

        NRet execute(string methodName, NArg *args, int len, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                     NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                     const NArg &arg10, NRet::Type retType = NRet::Type::Void);

    private:
        NativeClass() {}
        void *cls;
    };




#pragma - NativeObject

    class NativeObject {
    public:
        /*
        NativeObject();
        NativeObject(void *obj, bool autoRelease = true);
        NativeObject(string str);
        NativeObject(const Dictionary &dict);
        NativeObject(const Array &arr);
        NativeObject(const Int &i);
        NativeObject(const Long &l);
        NativeObject(const Float &f);
        NativeObject(const Double &d);
        NativeObject(const Bool &b);
        NativeObject(const String &s);
        NativeObject(const Bytes &b);
        ~NativeObject();
        NativeObject(const NativeObject &copy);
        NativeObject(const NArg &arg);
        NativeObject(const NRet &ret);
        NativeObject &operator=(const NativeObject &copy);
         */

        ~NativeObject();

        static shared_ptr<NativeObject> create(void *obj);
        static shared_ptr<NativeObject> create(string str);
        static shared_ptr<NativeObject> create(const Dictionary &dict);
        static shared_ptr<NativeObject> create(const Array &arr);
        static shared_ptr<NativeObject> create(const Int &i);
        static shared_ptr<NativeObject> create(const Long &l);
        static shared_ptr<NativeObject> create(const Float &f);
        static shared_ptr<NativeObject> create(const Double &d);
        static shared_ptr<NativeObject> create(const Bool &b);
        static shared_ptr<NativeObject> create(const String &s);
        static shared_ptr<NativeObject> create(const Bytes &b);


        NRet execute(string methodName, NArg *args, int len, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                     NRet::Type retType = NRet::Type::Void);
        NRet execute(string methodName, const NArg &arg1, const NArg &arg2, const NArg &arg3, const NArg &arg4,
                     const NArg &arg5, const NArg &arg6, const NArg &arg7, const NArg &arg8, const NArg &arg9,
                     const NArg &arg10, NRet::Type retType = NRet::Type::Void);
        void *getObject();

//        static NativeObject Null;

    private:
        void *obj = nullptr;

        NativeObject() {}
    };


#pragma NUtils
    class NUtils {
    public:
        static Dictionary toDictionary(void *o);
        static Dictionary toDictionary(const NArg &arg);
        static Dictionary toDictionary(const NRet &ret);

        static Array toArray(void *o);
        static Array toArray(const NArg &arg);
        static Array toArray(const NRet &ret);

        static string toString(void *o);
        static string toString(const NArg &arg);
        static string toString(const NRet &ret);
    };

}

#endif /* NativeClass_h */
