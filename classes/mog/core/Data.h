#ifndef Data_h
#define Data_h

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

extern void *enabler;

namespace mog {
    enum class DataType : char {
        Null,
        Int,
        Long,
        Float,
        Double,
        Bool,
        String,
        Bytes,
        Array,
        Dictionary,
    };
    
    
    class Data {
    public:
        DataType type = DataType::Null;
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Int : public Data {
    public:
        int value = 0;
        
        Int();
        Int(int value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Long : public Data {
    public:
        long long value = 0;
        
        Long();
        Long(long long value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Float : public Data {
    public:
        float value = 0;
        
        Float();
        Float(float value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Double : public Data {
    public:
        double value = 0;
        
        Double();
        Double(double value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Bool : public Data {
    public:
        bool value = false;
        
        Bool();
        Bool(bool value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    class String : public Data {
    public:
        string value = "";
        
        String();
        String(string value);
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
    };
    
    
    class Bytes : public Data {
    public:
        unsigned char *value = nullptr;
        unsigned int length = 0;
        
        Bytes();
        Bytes(unsigned char *value, unsigned int length);
        ~Bytes();
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
        string toString();
        string toString() const;
    };
    
    
    class Array : public Data {
    public:
        Array();
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        void append(const T &data) {
            auto d = shared_ptr<T>(new T(data));
            this->datum.emplace_back(d);
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        void set(int idx, const T &data) {
            auto d = shared_ptr<T>(new T(data));
            if ((int)this->datum.size() - 1 < idx) {
                int start = (int)this->datum.size();
                for (int i = start; i <= idx; i++) {
                    auto d = make_shared<Data>();
                    this->datum.emplace_back(d);
                }
            }
            this->datum[idx] = d;
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        T at(int idx) const {
            if (idx < this->datum.size()) {
                auto d = static_pointer_cast<T>(this->datum[idx]);
                return *d.get();
                
            } else {
                auto d = T();
                return d;
            }
        }
        
        void remove(int idx);
        void clear();
        size_t size() const;
        DataType atType(int idx) const;
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
        
    private:
        vector<shared_ptr<Data>> datum;
    };
    
    
    class Dictionary : public Data {
    public:
        Dictionary();
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        void put(string key, const T& data) {
            auto d = shared_ptr<T>(new T(data));
            this->datum[key] = d;
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        T get(string key) const {
            if (this->datum.count(key) > 0) {
                return *static_pointer_cast<T>(this->datum.at(key)).get();
                
            } else {
                auto d = T();
                d.type = DataType::Null;
                return d;
            }
        }
        DataType getType(string key) const;
        
        void remove(string key);
        void clear();
        size_t size() const;
        vector<string> getKeys() const;
        
        virtual void write(ostream &out);
        virtual void read(istream &in);
        
    private:
        map<string, shared_ptr<Data>> datum;
    };
    
    
    class Param {
    public:
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        Param(T data) {
            this->data = shared_ptr<T>(new T(data));
        }
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        T get() {
            return *static_pointer_cast<T>(this->data).get();
        }
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        T get() const {
            return *static_pointer_cast<T>(this->data).get();
        }
        DataType getType() {
            return this->data->type;
        }
    private:
        shared_ptr<Data> data;
    };
    
    
    class JsonData {
    public:
        static JsonData parse(string jsonText);
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        T toData() {
            return *(T *)this->data.get();
        }
        
        DataType getType() {
            return this->data->type;
        }
        
    private:
        shared_ptr<Data> data;
    };
}


#endif /* Data_h */


