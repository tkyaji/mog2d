#include "mog/core/Data.h"
#include "mog/libs/json.h"
#include "mog/core/mog_functions.h"
#include <vector>
#include <stdlib.h>

using namespace mog;

#pragma - Data

void *enabler;

void Data::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)this, sizeof(Data));
}

void Data::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Null) {
        throw std::ios_base::failure("data type is not match. type=Null");
    }
    in.read((char*)this, sizeof(Data));
}


#pragma - Int

Int::Int() {
    this->type = DataType::Int;
}
Int::Int(int value) {
    this->type = DataType::Int;
    this->value = value;
}

void Int::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(int));
}

void Int::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Int) {
        throw std::ios_base::failure("data type is not match. type=Int");
    }
    in.read((char *)&this->value, sizeof(int));
}


#pragma - Long

Long::Long() {
    this->type = DataType::Long;
}

Long::Long(long long value) {
    this->type = DataType::Long;
    this->value = value;
}

void Long::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(long long));
}

void Long::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Long) {
        throw std::ios_base::failure("data type is not match. type=Long");
    }
    in.read((char*)&this->value, sizeof(long long));
}


#pragma - Float

Float::Float() {
    this->type = DataType::Float;
}
Float::Float(float value) {
    this->type = DataType::Float;
    this->value = value;
}

void Float::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(float));
}

void Float::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Float) {
        throw std::ios_base::failure("data type is not match. type=Float");
    }
    in.read((char*)&this->value, sizeof(float));
}


#pragma - Double

Double::Double() {
    this->type = DataType::Double;
}
Double::Double(double value) {
    this->type = DataType::Double;
    this->value = value;
}

void Double::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(double));
}

void Double::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Double) {
        throw std::ios_base::failure("data type is not match. type=Double");
    }
    in.read((char*)&this->value, sizeof(double));
}


#pragma - Bool

Bool::Bool() {
    this->type = DataType::Bool;
}

Bool::Bool(bool value) {
    this->type = DataType::Bool;
    this->value = value;
}

void Bool::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(bool));
}

void Bool::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Bool) {
        throw std::ios_base::failure("data type is not match. type=Bool");
    }
    in.read((char*)&this->value, sizeof(bool));
}


#pragma - String

String::String() {
    this->type = DataType::String;
}

String::String(string value) {
    this->type = DataType::String;
    this->value = value;
}

void String::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    size_t size = this->value.size();
    out.write((char *)&size, sizeof(size_t));
    out.write((char *)this->value.c_str(), size * sizeof(char));
}

void String::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::String) {
        throw std::ios_base::failure("data type is not match. type=String");
    }
    size_t size;
    in.read((char *)&size, sizeof(size_t));
    char *str = new char[size];
    in.read((char *)str, size * sizeof(char));
    this->value = string(str, size);
    safe_delete_arr(str);
}


#pragma - Bytes

Bytes::Bytes() {
    this->type = DataType::Bytes;
}

Bytes::Bytes(unsigned char *value, unsigned int length) {
    this->type = DataType::Bytes;
    this->value = (unsigned char *)malloc(length);
    memcpy(this->value, value, length);
    this->length = length;
}

Bytes::~Bytes() {
    if (this->length > 0) {
        safe_free(this->value);
    }
}

void Bytes::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->length, sizeof(unsigned int));
    out.write((char *)this->value, this->length * sizeof(char));
}

void Bytes::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Bytes) {
        throw std::ios_base::failure("data type is not match. type=Bytes");
    }
    in.read((char *)&this->length, sizeof(unsigned int));
    in.read((char *)this->value, this->length * sizeof(char));
}

string Bytes::toString() {
    return string((char *)this->value, this->length);
}

string Bytes::toString() const {
    return string((char *)this->value, this->length);
}


#pragma - Array

Array::Array() {
    this->type = DataType::Array;
};

void Array::remove(int idx) {
    this->datum.erase(this->datum.begin() + idx);
}

void Array::clear() {
    this->datum.clear();
}

size_t Array::size() const {
    return this->datum.size();
}

DataType Array::atType(int idx) const {
    return this->datum.at(idx)->type;
}

void Array::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    size_t size = this->datum.size();
    out.write((char *)&size, sizeof(size_t));
    for (auto &d : this->datum) {
        d->write(out);
    }
}

void Array::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Array) {
        throw std::ios_base::failure("data type is not match. type=Array");
    }
    size_t dataSize;
    in.read((char *)&dataSize, sizeof(size_t));
    for (int i = 0; i < dataSize; i++) {
        auto pos = in.tellg();
        DataType type;
        in.read((char *)&type, sizeof(char));
        in.seekg(pos);
        
        switch (type) {
            case DataType::Int: {
                Int i;
                i.read(in);
                this->append(i);
                break;
            }
            case DataType::Long: {
                Long l;
                l.read(in);
                this->append(l);
                break;
            }
            case DataType::Float: {
                Float f;
                f.read(in);
                this->append(f);
                break;
            }
            case DataType::Double: {
                Double d;
                d.read(in);
                this->append(d);
                break;
            }
            case DataType::Bool: {
                Bool b;
                b.read(in);
                this->append(b);
                break;
            }
            case DataType::String: {
                String s;
                s.read(in);
                this->append(s);
                break;
            }
            case DataType::Bytes: {
                Bytes b;
                b.read(in);
                this->append(b);
                break;
            }
            case DataType::Array: {
                Array a;
                a.read(in);
                this->append(a);
                break;
            }
            case DataType::Dictionary: {
                Dictionary d;
                d.read(in);
                this->append(d);
                break;
            }
            default:
                break;
        }
    }
}


#pragma - Dictionary

Dictionary::Dictionary() {
    this->type = DataType::Dictionary;
}

void Dictionary::remove(string key) {
    this->datum.erase(key);
}

void Dictionary::clear() {
    this->datum.clear();
}

size_t Dictionary::size() const {
    return this->datum.size();
}

vector<string> Dictionary::getKeys() const {
    vector<string> keys;
    keys.reserve(this->datum.size());
    for (auto &pair : this->datum) {
        keys.emplace_back(pair.first);
    }
    return keys;
}

DataType Dictionary::getType(string key) const {
    return this->datum.at(key)->type;
}

void Dictionary::write(ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    size_t size = this->datum.size();
    out.write((char *)&size, sizeof(size_t));
    for (auto &kv : this->datum) {
        String key = String(kv.first);
        key.write(out);
        kv.second->write(out);
    }
}

void Dictionary::read(istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Dictionary) {
        throw std::ios_base::failure("data type is not match. type=Dictionary");
    }
    size_t dataSize;
    in.read((char *)&dataSize, sizeof(size_t));
    for (int i = 0; i < dataSize; i++) {
        String keyStr;
        keyStr.read(in);
        string key = keyStr.value;
        
        auto pos = in.tellg();
        DataType type;
        in.read((char *)&type, sizeof(char));
        in.seekg(pos);

        switch (type) {
            case DataType::Int: {
                Int i;
                i.read(in);
                this->put(key, i);
                break;
            }
            case DataType::Long: {
                Long l;
                l.read(in);
                this->put(key, l);
                break;
            }
            case DataType::Float: {
                Float f;
                f.read(in);
                this->put(key, f);
                break;
            }
            case DataType::Double: {
                Double d;
                d.read(in);
                this->put(key, d);
                break;
            }
            case DataType::Bool: {
                Bool b;
                b.read(in);
                this->put(key, b);
                break;
            }
            case DataType::String: {
                String s;
                s.read(in);
                this->put(key, s);
                break;
            }
            case DataType::Bytes: {
                Bytes b;
                b.read(in);
                this->put(key, b);
                break;
            }
            case DataType::Array: {
                Array a;
                a.read(in);
                this->put(key, a);
                break;
            }
            case DataType::Dictionary: {
                Dictionary d;
                d.read(in);
                this->put(key, d);
                break;
            }
            default:
                break;
        }
    }
}

shared_ptr<Data> jsonValueToData(json_value_s *value) {
    switch (value->type) {
        case json_type_object: {
            auto dict = shared_ptr<Dictionary>(new Dictionary());
            json_object_s *obj = (json_object_s *)value->payload;
            json_object_element_s *elm = obj->start;
            while (elm != NULL) {
                json_string_s *name = elm->name;
                string key = string(name->string);
                auto value = jsonValueToData(elm->value);
                switch (value->type) {
                    case DataType::Int:
                        dict->put(key, *(Int *)value.get()); break;
                    case DataType::Long:
                        dict->put(key, *(Long *)value.get()); break;
                    case DataType::Float:
                        dict->put(key, *(Float *)value.get()); break;
                    case DataType::Double:
                        dict->put(key, *(Double *)value.get()); break;
                    case DataType::Bool:
                        dict->put(key, *(Bool *)value.get()); break;
                    case DataType::String:
                        dict->put(key, *(String *)value.get()); break;
                    case DataType::Bytes:
                        dict->put(key, *(Bytes *)value.get()); break;
                    case DataType::Dictionary:
                        dict->put(key, *(Dictionary *)value.get()); break;
                    case DataType::Array:
                        dict->put(key, *(Array *)value.get()); break;
                    default:
                        dict->put(key, *value.get());
                }
                elm = elm->next;
            }
            return dict;
        }
        case json_type_array: {
            auto array = shared_ptr<Array>(new Array());
            json_array_s *arr = (json_array_s *)value->payload;
            json_array_element_s *elm = arr->start;
            while (elm != NULL) {
                auto value = jsonValueToData(elm->value);
                switch (value->type) {
                    case DataType::Int:
                        array->append(*(Int *)value.get()); break;
                    case DataType::Long:
                        array->append(*(Long *)value.get()); break;
                    case DataType::Float:
                        array->append(*(Float *)value.get()); break;
                    case DataType::Double:
                        array->append(*(Double *)value.get()); break;
                    case DataType::Bool:
                        array->append(*(Bool *)value.get()); break;
                    case DataType::String:
                        array->append(*(String *)value.get()); break;
                    case DataType::Bytes:
                        array->append(*(Bytes *)value.get()); break;
                    case DataType::Dictionary:
                        array->append(*(Dictionary *)value.get()); break;
                    case DataType::Array:
                        array->append(*(Array *)value.get()); break;
                    default:
                        array->append(*value.get());
                }
                elm = elm->next;
            }
            return array;
        }
        case json_type_string: {
            json_string_s *str = (json_string_s *)value->payload;
            return shared_ptr<String>(new String(string(str->string)));
        }
        case json_type_number: {
            json_number_s *number = (json_number_s *)value->payload;
            double d = atof(number->number);
            int i = atoi(number->number);
            if (d == i) {
                return shared_ptr<Int>(new Int(i));
            } else {
                return shared_ptr<Double>(new Double(d));
            }
        }
        case json_type_true: {
            return shared_ptr<Bool>(new Bool(true));
        }
        case json_type_false: {
            return shared_ptr<Bool>(new Bool(false));
        }
        default: {
            return shared_ptr<Data>(new Data());
        }
    }
}

JsonData JsonData::parse(string jsonText) {
    JsonData jsonData;
    json_value_s *root = json_parse(jsonText.c_str(), jsonText.size());
    jsonData.data = jsonValueToData(root);
    return jsonData;
}

