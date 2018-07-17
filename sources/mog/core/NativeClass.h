#ifndef NativeClass_h
#define NativeClass_h

#include <string>
#include <memory>
#include <functional>
#include "mog/core/Data.h"

extern void *enabler;

namespace mog {
    class NativeObject;
    
    template <class T>
    static DataType __getDataType() {
        return DataType::Void;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Int>() {
        return DataType::Int;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Long>() {
        return DataType::Long;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Float>() {
        return DataType::Float;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Double>() {
        return DataType::Double;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Bool>() {
        return DataType::Bool;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::String>() {
        return DataType::String;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::ByteArray>() {
        return DataType::ByteArray;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::List>() {
        return DataType::List;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::Dictionary>() {
        return DataType::Dictionary;
    }
    template <>
    __attribute__((unused)) DataType __getDataType<mog::NativeObject>() {
        return DataType::NativeObject;
    }
    
    class NativeObject : public Data {
    public:
        static std::shared_ptr<NativeObject> create(void *value);
        static std::shared_ptr<NativeObject> create(const std::shared_ptr<Dictionary> &dict);
        static std::shared_ptr<NativeObject> create(const std::shared_ptr<List> &list);
        static std::shared_ptr<NativeObject> create(const std::shared_ptr<ByteArray> &bytes);
        static std::shared_ptr<NativeObject> create(std::function<void(const std::shared_ptr<mog::List> &args)> func);
        
        ~NativeObject();

        void *getValue();
        virtual void write(std::ostream &out) override { }
        virtual void read(std::istream &in) override {}

        void execute(std::string methodName);
        
        template<class... Args>
        void execute(std::string methodName, const Args&... arg) {
            auto args = mog::List::create();
            this->appendArg(args, arg...);
            this->executeWithList(methodName, args);
        }
        void executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> execute(std::string methodName) {
            auto args = mog::List::create();
            return this->executeWithList<T>(methodName, args);
        }

        template <class T, class... Args, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> execute(std::string methodName, const Args&... arg) {
            auto args = mog::List::create();
            this->appendArg(args, arg...);
            auto ret = this->executeWithList<T>(methodName, args);
            return std::static_pointer_cast<T>(ret);
        }

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
            DataType retType = __getDataType<T>();
            auto ret = this->executeMain(methodName, args, retType);
            return std::static_pointer_cast<T>(ret);
        }

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> getProperty(std::string propertyName) {
            DataType retType = __getDataType<T>();
            auto ret = this->getPropertyMain(propertyName, retType);
            return std::static_pointer_cast<T>(ret);
        }

        void setProperty(std::string propertyName, const std::shared_ptr<Data> &value);

    private:
        NativeObject(void *value);
        std::shared_ptr<mog::Data> executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType);
        std::shared_ptr<mog::Data> getPropertyMain(std::string propertyName, DataType retType);

        template<class First, class... Rest>
        void appendArg(std::shared_ptr<mog::List> &args, const First& first, const Rest&... rest) {
            args->append(first);
            this->appendArg(args, rest...);
        }
        template<class First>
        void appendArg(std::shared_ptr<mog::List> &args, const First& first) {
            args->append(first);
        }

        void *value = nullptr;
    };

    
    class NativeClass {
    public:
        static std::shared_ptr<NativeClass> create(std::string className);
        
        ~NativeClass();
        
        std::shared_ptr<NativeObject> newInstance();
#ifndef MOG_IOS
        template <class T, class... Args, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<NativeObject> newInstance(const Args&... arg) {
            auto args = mog::List::create();
            this->appendArg(args, arg...);
            this->newInstanceWithList(args);
        }

        std::shared_ptr<NativeObject> newInstanceWithList(const std::shared_ptr<mog::List> &args);
#endif
        void execute(std::string methodName);

        template<class... Args>
        void execute(std::string methodName, const Args&... arg) {
            auto args = mog::List::create();
            this->appendArg(args, arg...);
            this->executeWithList(methodName, args);
        }
        void executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> execute(std::string methodName) {
            auto args = mog::List::create();
            return this->executeWithList<T>(methodName, args);
        }

        template <class T, class... Args, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> execute(std::string methodName, const Args&... arg) {
            auto args = mog::List::create();
            this->appendArg(args, arg...);
            auto ret = this->executeWithList<T>(methodName, args);
            return std::static_pointer_cast<T>(ret);
        }

        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> executeWithList(std::string methodName, const std::shared_ptr<mog::List> &args) {
            DataType retType = __getDataType<T>();
            auto ret = this->executeMain(methodName, args, retType);
            return std::static_pointer_cast<T>(ret);
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> getProperty(std::string propertyName) {
            DataType retType = __getDataType<T>();
            auto ret = this->getPropertyMain(propertyName, retType);
            return std::static_pointer_cast<T>(ret);
        }
        
        void setProperty(std::string propertyName, const std::shared_ptr<Data> &value);
        
    private:
        NativeClass(std::string className);
        std::shared_ptr<mog::Data> executeMain(std::string methodName, const std::shared_ptr<mog::List> &args, DataType retType);
        std::shared_ptr<mog::Data> getPropertyMain(std::string propertyName, DataType retType);

        template<class First, class... Rest>
        void appendArg(std::shared_ptr<mog::List> &args, const First& first, const Rest&... rest) {
            args->append(first);
            this->appendArg(args, rest...);
        }
        template<class First>
        void appendArg(std::shared_ptr<mog::List> &args, const First& arg) {
            args->append(arg);
        }

        void *cls;
    };


#ifndef MOG_IOS
    class NativeFunction {
    public:
        NativeFunction(std::function<void(const std::shared_ptr<mog::List> &args)> func);
        void invoke(const std::shared_ptr<mog::List> &args);

    private:
        std::function<void(const std::shared_ptr<mog::List> &args)> func;
    };
#endif
}


#endif /* NativeClass_h */
