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
        Void,
        Int,
        Long,
        Float,
        Double,
        Bool,
        String,
        ByteArray,
        List,
        Dictionary,
        NativeObject,
    };
    
    
    class Data : public std::enable_shared_from_this<Data> {
    public:
        DataType type;
        
        virtual void write(std::ostream &out) = 0;
        virtual void read(std::istream &in) = 0;
    };
    
    
    class Int : public Data {
    public:
        static std::shared_ptr<Int> create(int value);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;
        
        int getValue();

    private:
        Int(int value);
        int value = 0;
    };
    
    
    class Long : public Data {
    public:
        static std::shared_ptr<Long> create(long long value);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;

        long long getValue();

    private:
        Long(long long value);
        long long value = 0;
    };
    
    
    class Float : public Data {
    public:
        static std::shared_ptr<Float> create(float value);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;

        float getValue();

    private:
        Float(float value);
        float value = 0;
    };
    
    
    class Double : public Data {
    public:
        static std::shared_ptr<Double> create(double value);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;

        double getValue();

    private:
        Double(double value);
        double value = 0;
    };
    
    
    class Bool : public Data {
    public:
        static std::shared_ptr<Bool> create(bool value);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;

        bool getValue();

    private:
        Bool(bool value);
        bool value = false;
    };
    
    
    class ByteArray : public Data {
    public:
        static std::shared_ptr<ByteArray> create(unsigned char *value, unsigned int length, bool copy = false);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;
        std::string toString();
        std::string toString() const;
        ~ByteArray();

        void getValue(unsigned char **value, unsigned int *length);
        
    private:
        ByteArray(unsigned char *value, unsigned int length, bool copy = false);
        unsigned char *value = nullptr;
        unsigned int length = 0;
    };

    
    class String : public Data {
    public:
        static std::shared_ptr<String> create(std::string value);
        static std::shared_ptr<String> create(const std::shared_ptr<ByteArray> &bytes);
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;

        std::string getValue();

    private:
        String(std::string value);
        String(const std::shared_ptr<ByteArray> bytes);
        std::string value = "";
    };
    
    
    class List : public Data {
    public:
        static std::shared_ptr<List> create();
        void append(const std::shared_ptr<Data> &data);
        void set(int idx, const std::shared_ptr<Data> &data);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> at(int idx) const {
            if (idx < this->datum.size()) {
                return std::static_pointer_cast<T>(this->datum[idx]);
            } else {
                return nullptr;
            }
        }
        
        void remove(int idx);
        void clear();
        unsigned int size() const;
        DataType atType(int idx) const;
        
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;
        
    private:
        List();
        
        std::vector<std::shared_ptr<Data>> datum;
    };
    
    
    class Dictionary : public Data {
    public:
        static std::shared_ptr<Dictionary> create();

        void put(std::string key, const std::shared_ptr<Data> &data);
        
        template <class T, typename std::enable_if<std::is_base_of<Data, T>::value>::type*& = enabler>
        std::shared_ptr<T> get(std::string key) const {
            if (this->datum.count(key) > 0) {
                auto d = this->datum.at(key);
                return std::static_pointer_cast<T>(d);
                
            } else {
                return nullptr;
            }
        }
        DataType getType(std::string key) const;
        
        void remove(std::string key);
        void clear();
        unsigned int size() const;
        std::vector<std::string> getKeys() const;
        
        virtual void write(std::ostream &out) override;
        virtual void read(std::istream &in) override;
        
    private:
        Dictionary();
        
        std::map<std::string, std::shared_ptr<Data>> datum;
    };
    
    
    /*
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
    */
    
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


