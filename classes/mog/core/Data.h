#ifndef Data_h
#define Data_h

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

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
        
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Int : public Data {
    public:
        int value = 0;
        
        Int();
        Int(int value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Long : public Data {
    public:
        long long value = 0;
        
        Long();
        Long(long long value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Float : public Data {
    public:
        float value = 0;
        
        Float();
        Float(float value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Double : public Data {
    public:
        double value = 0;
        
        Double();
        Double(double value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Bool : public Data {
    public:
        bool value = false;
        
        Bool();
        Bool(bool value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    class String : public Data {
    public:
        std::string value = "";
        
        String();
        String(std::string value);
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
    };
    
    
    class Bytes : public Data {
    public:
        unsigned char *value = nullptr;
        unsigned int length = 0;
        
        Bytes();
        Bytes(unsigned char *value, unsigned int length);
        ~Bytes();
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
        std::string toString();
        std::string toString() const;
    };
    
    
    class Array : public Data {
    public:
        Array();
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void append(const T &data) {
            auto d = std::shared_ptr<T>(new T(data));
            this->datum.emplace_back(d);
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void set(int idx, const T &data) {
            auto d = std::shared_ptr<T>(new T(data));
            if ((int)this->datum.size() - 1 < idx) {
                int start = (int)this->datum.size();
                for (int i = start; i <= idx; i++) {
                    auto d = std::make_shared<Data>();
                    this->datum.emplace_back(d);
                }
            }
            this->datum[idx] = d;
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T at(int idx) const {
            if (idx < this->datum.size()) {
                auto d = std::static_pointer_cast<T>(this->datum[idx]);
                return *d.get();
                
            } else {
                auto d = T();
                return d;
            }
        }
        
        void remove(int idx);
        void clear();
        unsigned int size() const;
        DataType atType(int idx) const;
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
        
    private:
        std::vector<std::shared_ptr<Data>> datum;
    };
    
    
    class Dictionary : public Data {
    public:
        Dictionary();
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        void put(std::string key, const T& data) {
            auto d = std::shared_ptr<T>(new T(data));
            this->datum[key] = d;
        }
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T get(std::string key) const {
            if (this->datum.count(key) > 0) {
                return *std::static_pointer_cast<T>(this->datum.at(key)).get();
                
            } else {
                auto d = T();
                d.type = DataType::Null;
                return d;
            }
        }
        DataType getType(std::string key) const;
        
        void remove(std::string key);
        void clear();
        unsigned int size() const;
        std::vector<std::string> getKeys() const;
        
        virtual void write(std::ostream &out);
        virtual void read(std::istream &in);
        
    private:
        std::map<std::string, std::shared_ptr<Data>> datum;
    };
    
    
    class DataValue {
    public:
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        DataValue(T data) {
            this->data = std::unique_ptr<T>(new T(data));
        }
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T get() {
            return *std::static_pointer_cast<T>(this->data).get();
        }
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T get() const {
            return *std::static_pointer_cast<T>(this->data).get();
        }
        DataType getType() {
            return this->data->type;
        }
    private:
        std::unique_ptr<Data> data;
    };
    
    
    class JsonData {
    public:
        static JsonData parse(std::string jsonText);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        T toData() {
            return *(T *)this->data.get();
        }
        
        DataType getType() {
            return this->data->type;
        }
        
    private:
        std::shared_ptr<Data> data;
    };
}


#endif /* Data_h */


