#include "mog/Constants.h"
#include "mog/core/Data.h"
#include "mog/libs/json.h"
#include "mog/core/mog_functions.h"
#include <vector>
#include <stdlib.h>
#include <string.h>

using namespace mog;

#pragma - Data

void *enabler;

#pragma - Null

std::shared_ptr<Null> Null::create() {
    return std::shared_ptr<Null>(new Null());
}

void Null::write(std::ostream &out) {
}

void Null::read(std::istream &in) {
}


#pragma - Int

std::shared_ptr<Int> Int::create(int value) {
    return std::shared_ptr<Int>(new Int(value));
}

Int::Int(int value) {
    this->type = DataType::Int;
    this->value = value;
}

int Int::getValue() {
    return this->value;
}

void Int::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(int));
}

void Int::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Int) {
        throw std::ios_base::failure("data type is not match. type=Int");
    }
    in.read((char *)&this->value, sizeof(int));
}


#pragma - Long

std::shared_ptr<Long> Long::create(long long value) {
    return std::shared_ptr<Long>(new Long(value));
}

Long::Long(long long value) {
    this->type = DataType::Long;
    this->value = value;
}

long long Long::getValue() {
    return this->value;
}

void Long::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(long long));
}

void Long::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Long) {
        throw std::ios_base::failure("data type is not match. type=Long");
    }
    in.read((char*)&this->value, sizeof(long long));
}


#pragma - Float

std::shared_ptr<Float> Float::create(float value) {
    return std::shared_ptr<Float>(new Float(value));
}

Float::Float(float value) {
    this->type = DataType::Float;
    this->value = value;
}

float Float::getValue() {
    return this->value;
}

void Float::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(float));
}

void Float::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Float) {
        throw std::ios_base::failure("data type is not match. type=Float");
    }
    in.read((char*)&this->value, sizeof(float));
}


#pragma - Double

std::shared_ptr<Double> Double::create(double value) {
    return std::shared_ptr<Double>(new Double(value));
}

Double::Double(double value) {
    this->type = DataType::Double;
    this->value = value;
}

double Double::getValue() {
    return this->value;
}

void Double::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(double));
}

void Double::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Double) {
        throw std::ios_base::failure("data type is not match. type=Double");
    }
    in.read((char*)&this->value, sizeof(double));
}


#pragma - Bool

std::shared_ptr<Bool> Bool::create(bool value) {
    return std::shared_ptr<Bool>(new Bool(value));
}

Bool::Bool(bool value) {
    this->type = DataType::Bool;
    this->value = value;
}

bool Bool::getValue() {
    return this->value;
}

void Bool::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->value, sizeof(bool));
}

void Bool::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Bool) {
        throw std::ios_base::failure("data type is not match. type=Bool");
    }
    in.read((char*)&this->value, sizeof(bool));
}


#pragma - ByteArray

std::shared_ptr<ByteArray> ByteArray::create(unsigned char *value, unsigned int length, bool copy) {
    return std::shared_ptr<ByteArray>(new ByteArray(value, length, copy));
}

ByteArray::ByteArray(unsigned char *value, unsigned int length, bool copy) {
    this->type = DataType::ByteArray;
    if (copy) {
        this->value = (unsigned char *)mogmalloc(sizeof(unsigned char) * length);
        memcpy(this->value, value, length);
    } else {
        this->value = value;
    }
    this->length = length;
}

ByteArray::~ByteArray() {
    if (this->value != nullptr) {
        mogfree(this->value);
    }
}

void ByteArray::getValue(unsigned char **value, unsigned int *length) {
    if (value) *value = this->value;
    if (length) *length = this->length;
}

unsigned int ByteArray::getLength() {
    return this->length;
}

unsigned char ByteArray::getByte(int idx) {
    return this->value[idx];
}

unsigned char *ByteArray::getBytes(bool copy) {
    if (copy) {
        unsigned char *value = (unsigned char *)mogmalloc(sizeof(unsigned char) * this->length);
        memcpy(value, this->value, this->length);
        return value;
    } else {
        return this->value;
    }
}

void ByteArray::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    out.write((char *)&this->length, sizeof(unsigned int));
    out.write((char *)this->value, this->length * sizeof(char));
}

void ByteArray::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::ByteArray) {
        throw std::ios_base::failure("data type is not match. type=ByteArray");
    }
    in.read((char *)&this->length, sizeof(unsigned int));
    in.read((char *)this->value, this->length * sizeof(char));
}

std::string ByteArray::toString() {
    return std::string((char *)this->value, this->length);
}

std::string ByteArray::toString() const {
    return std::string((char *)this->value, this->length);
}


#pragma - String

std::shared_ptr<String> String::create(std::string value) {
    return std::shared_ptr<String>(new String(value));
}

std::shared_ptr<String> String::create(const std::shared_ptr<ByteArray> &bytes) {
    return std::shared_ptr<String>(new String(bytes));
}

String::String(std::string value) {
    this->type = DataType::String;
    this->value = value;
}

String::String(const std::shared_ptr<ByteArray> bytes) {
    this->type = DataType::String;
    unsigned char *value = nullptr;
    unsigned int length = 0;
    bytes->getValue(&value, &length);
    this->value = std::string((char *)value, length);
}

std::string String::getValue() {
    return this->value;
}

void String::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    unsigned int size = (unsigned int)this->value.size();
    out.write((char *)&size, sizeof(unsigned int));
    out.write((char *)this->value.c_str(), size * sizeof(char));
}

void String::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::String) {
        throw std::ios_base::failure("data type is not match. type=String");
    }
    unsigned int size;
    in.read((char *)&size, sizeof(unsigned int));
    char *str = (char *)mogmalloc(sizeof(char) * size);
    in.read((char *)str, size * sizeof(char));
    this->value = std::string(str, size);
    mogfree(str);
}


#pragma - List

std::shared_ptr<List> List::create() {
    return std::shared_ptr<List>(new List());
}

List::List() {
    this->type = DataType::List;
};

void List::remove(int idx) {
    this->datum.erase(this->datum.begin() + idx);
}

void List::clear() {
    this->datum.clear();
}

unsigned int List::size() const {
    return (unsigned int)this->datum.size();
}

void List::append(const std::shared_ptr<Data> &data) {
    this->datum.emplace_back(data);
}

void List::set(int idx, const std::shared_ptr<Data> &data) {
    this->datum[idx] = data;
}

DataType List::atType(int idx) const {
    return this->datum.at(idx)->type;
}

void List::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    unsigned int size = (unsigned int)this->datum.size();
    out.write((char *)&size, sizeof(unsigned int));
    for (auto &d : this->datum) {
        d->write(out);
    }
}

void List::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::List) {
        throw std::ios_base::failure("data type is not match. type=List");
    }
    unsigned int dataSize;
    in.read((char *)&dataSize, sizeof(unsigned int));
    for (int i = 0; i < dataSize; i++) {
        auto pos = in.tellg();
        DataType type;
        in.read((char *)&type, sizeof(char));
        in.seekg(pos);
        
        std::shared_ptr<Data> data = nullptr;
        switch (type) {
            case DataType::Int: {
                data = Int::create(0);
                break;
            }
            case DataType::Long: {
                data = Long::create(0);
                break;
            }
            case DataType::Float: {
                data = Float::create(0.0f);
                break;
            }
            case DataType::Double: {
                data = Double::create(0.0);
                break;
            }
            case DataType::Bool: {
                data = Bool::create(false);
                break;
            }
            case DataType::String: {
                data = String::create("");
                break;
            }
            case DataType::ByteArray: {
                data = ByteArray::create(nullptr, 0);
                break;
            }
            case DataType::List: {
                data = List::create();
                break;
            }
            case DataType::Dictionary: {
                data = Dictionary::create();
                break;
            }
            default:
                break;
        }
        data->read(in);
        this->append(data);
    }
}


#pragma - Dictionary

std::shared_ptr<Dictionary> Dictionary::create() {
    return std::shared_ptr<Dictionary>(new Dictionary());
}

Dictionary::Dictionary() {
    this->type = DataType::Dictionary;
}

void Dictionary::remove(std::string key) {
    this->datum.erase(key);
}

void Dictionary::clear() {
    this->datum.clear();
}

unsigned int Dictionary::size() const {
    return (unsigned int)this->datum.size();
}

void Dictionary::put(std::string key, const std::shared_ptr<Data> &data) {
    this->datum[key] = data;
}

std::vector<std::string> Dictionary::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(this->datum.size());
    for (auto &pair : this->datum) {
        keys.emplace_back(pair.first);
    }
    return keys;
}

std::pair<std::string, std::shared_ptr<Data>> Dictionary::getKeyValue(int idx) {
    auto itr = this->datum.begin();
    for (int i = 0; i < idx; i++) itr++;
    return std::pair<std::string, std::shared_ptr<Data>>(itr->first, itr->second);
}

bool Dictionary::hasKey(std::string key) {
    return this->datum.count(key) > 0;
}

DataType Dictionary::getType(std::string key) const {
    return this->datum.at(key)->type;
}

void Dictionary::write(std::ostream &out) {
    out.write((char *)&this->type, sizeof(char));
    unsigned int size = (unsigned int)this->datum.size();
    out.write((char *)&size, sizeof(unsigned int));
    for (auto &kv : this->datum) {
        auto key = String::create(kv.first);
        key->write(out);
        kv.second->write(out);
    }
}

void Dictionary::read(std::istream &in) {
    in.read((char *)&this->type, sizeof(char));
    if (this->type != DataType::Dictionary) {
        throw std::ios_base::failure("data type is not match. type=Dictionary");
    }
    unsigned int dataSize;
    in.read((char *)&dataSize, sizeof(unsigned int));
    for (int i = 0; i < dataSize; i++) {
        auto keyStr = String::create("");
        keyStr->read(in);
        std::string key = keyStr->getValue();
        
        auto pos = in.tellg();
        DataType type;
        in.read((char *)&type, sizeof(char));
        in.seekg(pos);

        std::shared_ptr<Data> data = nullptr;
        switch (type) {
            case DataType::Int: {
                data = Int::create(0);
                break;
            }
            case DataType::Long: {
                data = Long::create(0);
                break;
            }
            case DataType::Float: {
                data = Float::create(0.0f);
                break;
            }
            case DataType::Double: {
                data = Double::create(0.0);
                break;
            }
            case DataType::Bool: {
                data = Bool::create(false);
                break;
            }
            case DataType::String: {
                data = String::create("");
                break;
            }
            case DataType::ByteArray: {
                data = ByteArray::create(nullptr, 0);
                break;
            }
            case DataType::List: {
                data = List::create();
                break;
            }
            case DataType::Dictionary: {
                data = Dictionary::create();
                break;
            }
            default:
                break;
        }
        data->read(in);
        this->put(key, data);
    }
}


static std::shared_ptr<Data> jsonValueToData(json_value_s *value) {
    switch (value->type) {
        case json_type_object: {
            auto dict = Dictionary::create();
            json_object_s *obj = (json_object_s *)value->payload;
            json_object_element_s *elm = obj->start;
            while (elm != NULL) {
                json_string_s *name = elm->name;
                std::string key = std::string(name->string);
                auto value = jsonValueToData(elm->value);
                dict->put(key, value);
                elm = elm->next;
            }
            return dict;
        }
        case json_type_array: {
            auto list = List::create();
            json_array_s *arr = (json_array_s *)value->payload;
            json_array_element_s *elm = arr->start;
            while (elm != NULL) {
                auto value = jsonValueToData(elm->value);
                list->append(value);
                elm = elm->next;
            }
            return list;
        }
        case json_type_string: {
            json_string_s *str = (json_string_s *)value->payload;
            return String::create(str->string);
        }
        case json_type_number: {
            json_number_s *number = (json_number_s *)value->payload;
            double d = atof(number->number);
            int i = atoi(number->number);
            if (d == i) {
                return Int::create(i);
            } else {
                return Double::create(d);
            }
        }
        case json_type_true: {
            return Bool::create(true);
        }
        case json_type_false: {
            return Bool::create(false);
        }
        default: {
            return nullptr;
        }
    }
}

static void dataTojsonString(std::stringstream &ss, const std::shared_ptr<Data> &data) {
    switch (data->type) {
        case DataType::Int:
            ss << std::static_pointer_cast<Int>(data)->getValue();
            break;
            
        case DataType::Long:
            ss << std::static_pointer_cast<Long>(data)->getValue();
            break;
            
        case DataType::Float:
            ss << std::static_pointer_cast<Float>(data)->getValue();
            break;
            
        case DataType::Double:
            ss << std::static_pointer_cast<Double>(data)->getValue();
            break;
            
        case DataType::Bool: {
            bool b = std::static_pointer_cast<Bool>(data)->getValue();
            if (b) {
                ss << "true";
            } else {
                ss << "false";
            }
            break;
        }
            
        case DataType::String: {
            std::string str = std::static_pointer_cast<String>(data)->getValue();
            if (!str.empty()) {
                std::string::size_type pos = 0;
                while ((pos = str.find('"', pos)) != std::string::npos) {
                    str.replace(pos, 1, "\\\"");
                    pos += 2;
                }
            }
            ss << "\"" << str << "\"";
            break;
        }
            
        case DataType::ByteArray: {
            auto bytes = std::static_pointer_cast<ByteArray>(data);
            ss << "[";
            for (int i = 0; i < bytes->getLength(); i++) {
                if (i > 0) ss << ",";
                unsigned char b = bytes->getByte(i);
                ss << (int)b;
            }
            ss << "]";
            break;
        }
            
        case DataType::Dictionary: {
            auto dict = std::static_pointer_cast<Dictionary>(data);
            ss << "{";
            for (int i = 0; i < dict->size(); i++) {
                if (i > 0) ss << ",";
                auto pair = dict->getKeyValue(i);
                dataTojsonString(ss, String::create(pair.first));
                ss << ":";
                dataTojsonString(ss, pair.second);
            }
            ss << "}";
            break;
        }
            
        case DataType::List: {
            auto list = std::static_pointer_cast<List>(data);
            ss << "[";
            for (int i = 0; i < list->size(); i++) {
                if (i > 0) ss << ",";
                dataTojsonString(ss, list->at<Data>(i));
            }
            ss << "]";
            break;
        }
            
        case DataType::NativeObject:
        case DataType::Void:
            break;
    }
}


std::shared_ptr<Data> Json::_parse(std::string jsonText) {
    json_value_s *root = json_parse(jsonText.c_str(), jsonText.size());
    return jsonValueToData(root);
}

std::string Json::toJson(const std::shared_ptr<Data> &data) {
    std::stringstream ss;
    dataTojsonString(ss, data);
    return ss.str();
}

