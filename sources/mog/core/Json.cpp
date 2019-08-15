#include "mog/core/Json.h"
#include "mog/core/mog_functions.h"

using namespace mog;

static std::shared_ptr<Dictionary> parseObject(const char *json, int &i, int len, bool &hasError);
static std::shared_ptr<List> parseArray(const char *json, int &i, int len, bool &hasError);
static std::string parseText(const char *json, int &i, int len, bool &hasError);
static void parseComment(const char *json, int &i, int len, bool &hasError);
static std::shared_ptr<Data> parseValue(const char *json, int &i, int len, bool &hasError);
static std::shared_ptr<Data> parseNumber(const char *json, int &i, int len, bool &hasError);
static void skipWhiteSpace(const char *json, int &i, int len);

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


std::shared_ptr<Dictionary> Json::parse(std::string jsonText) {
    int len = (int)jsonText.size();
    const char *json = jsonText.c_str();
    int i = 0;
    
    skipWhiteSpace(json, i, len);
    if (json[i] != '{') {
        return nullptr;
    }
    
    bool hasError;
    auto obj = parseObject(json, i, len, hasError);
    if (hasError) {
        LOGE("Invalid Json.");
        return nullptr;
    }
    return obj;
}

std::string Json::toJson(const std::shared_ptr<Dictionary> &data) {
    std::stringstream ss;
    dataTojsonString(ss, data);
    return ss.str();
}



static std::shared_ptr<Dictionary> parseObject(const char *json, int &i, int len, bool &hasError) {
    auto dict = Dictionary::create();
    
    int state = 0;   // 0:findingFirstKey, 1:finding':', 2:findingValue, 3:findingEnd, 4:findingNextKey
    std::string key;
    
    i++;
    while (i < len) {
        switch (json[i]) {
            case '}':
                if (state != 0 && state != 3) {
                    hasError = true;
                    return nullptr;
                }
                return dict;

            case ',':
                if (state != 3) {
                    hasError = true;
                    return nullptr;
                }
                state = 4;
                break;

            case '"': {
                if (state == 0 || state == 4) {
                    key = parseText(json, i, len, hasError);
                    if (hasError) return nullptr;
                    
                    state = 1;
                    
                } else if (state == 2) {
                    std::string value = parseText(json, i, len, hasError);
                    if (hasError) return nullptr;
                    
                    dict->put(key, String::create(value));
                    state = 3;
                    
                } else {
                    hasError = true;
                    return nullptr;
                }
                break;
            }

            case ':':
                if (state == 1) {
                    state = 2;
                } else {
                    hasError = true;
                    return nullptr;
                }
                break;

            case ' ':
            case '\t':
            case '\n':
                break;
                
            case '/':
                parseComment(json, i, len, hasError);
                if (hasError) return nullptr;
                break;
                
            default:
                if (state == 2) {
                    auto value = parseValue(json, i, len, hasError);
                    if (hasError) return nullptr;
                    
                    dict->put(key, value);
                    state = 3;
                    
                } else {
                    hasError = true;
                    return nullptr;
                }
                break;
        }
        i++;
    }
    return dict;
}

static std::shared_ptr<List> parseArray(const char *json, int &i, int len, bool &hasError) {
    auto list = List::create();
    
    int state = 0;   // 0:findingFirstValue, 1:findingEnd, 2:findingNextKey

    i++;
    while (i < len) {
        switch (json[i]) {
            case ']':
                if (state != 0 && state != 1) {
                    hasError = true;
                    return nullptr;
                }
                return list;

            case ',':
                if (state != 1) {
                    hasError = true;
                    return nullptr;
                }
                state = 0;
                break;
                
            case '"': {
                if (state == 0 || state == 2) {
                    std::string value = parseText(json, i, len, hasError);
                    if (hasError) return nullptr;
                    
                    list->append(String::create(value));
                    state = 1;
                }
                break;
            }
                
            case ' ':
            case '\t':
            case '\n':
                break;
                
            case '/':
                parseComment(json, i, len, hasError);
                if (hasError) return nullptr;
                break;
                
            default:
                if (state == 0 || state == 2) {
                    auto value = parseValue(json, i, len, hasError);
                    if (hasError) return nullptr;
                    
                    list->append(value);
                    state = 1;
                    
                } else {
                    hasError = true;
                    return nullptr;
                }
                break;
        }
        i++;
    }
    return list;
}

static std::string parseText(const char *json, int &i, int len, bool &hasError) {
    i++;
    int startIdx = i;
    while (i < len) {
        if (json[i] == '"') {
            int textLen = i - startIdx;
            char *text = new char[textLen + 1];
            strncpy(text, &json[startIdx], textLen);
            text[textLen] = '\0';
            auto str = std::string(text);
            delete[] text;
            hasError = false;
            return str;
        }
        i++;
    }
    hasError = true;
    return "";
}

static void parseComment(const char *json, int &i, int len, bool &hasError) {
    if (i + 1 >= len) {
        hasError = true;
        return;
    }
    i++;
    
    if (json[i] == '/') {
        while (i < len) {
            if (json[i] == '\n') {
                hasError = false;
                return;
            }
        }
    } else if (json[i] == '*') {
        while (i < len - 1) {
            if (json[i] == '*' && json[i+1] == '/') {
                hasError = false;
                return;
            }
        }
    }
    hasError = true;
}

static std::shared_ptr<Data> parseValue(const char *json, int &i, int len, bool &hasError) {
    while (i < len) {
        switch (json[i]) {
            case '{':
                return parseObject(json, i, len, hasError);
                break;

            case '[':
                return parseArray(json, i, len, hasError);
                break;

            case 'n':
                if (i + 4 >= len) {
                    hasError = true;
                    return nullptr;
                }
                if (json[i+1] == 'u' &&
                    json[i+2] == 'l' &&
                    json[i+3] == 'l') {
                    hasError = false;
                    i += 3;
                    return Null::create();
                }
                break;

            case 't':
                if (i + 4 >= len) {
                    hasError = true;
                    return nullptr;
                }
                if (json[i+1] == 'r' &&
                    json[i+2] == 'u' &&
                    json[i+3] == 'e') {
                    hasError = false;
                    i += 3;
                    return Bool::create(true);
                }
                break;

            case 'f':
                if (i + 5 >= len) {
                    hasError = true;
                    return nullptr;
                }
                if (json[i+1] == 'a' &&
                    json[i+2] == 'l' &&
                    json[i+3] == 's' &&
                    json[i+4] == 'e') {
                    hasError = false;
                    i += 4;
                    return Bool::create(false);
                }
                break;

            default:
                return parseNumber(json, i, len, hasError);
        }
        i++;
    }
    hasError = true;
    return nullptr;
}

static std::shared_ptr<Data> parseNumber(const char *json, int &i, int len, bool &hasError) {
    int startIdx = i;
    bool hasColon = false;
    while (i < len) {
        char c = json[i];
        
        if (c >= '0' && c <= '9') {
            // Valid Number
        } else if (c == '-' || c == '+' || c == 'e') {
            // Valid Sign
        } else if (c == '.') {
            hasColon = true;
        } else {
            i--;
            break;
        }
        i++;
    }
    
    if (hasColon) {
        char *end;
        double dValue = std::strtod(&json[startIdx], &end);
        hasError = false;
        return Double::create(dValue);
        
    } else {
        char *end;
        long long lValue = std::strtoll(&json[startIdx], &end, 10);
        hasError = false;
        return Long::create(lValue);
    }
}

static void skipWhiteSpace(const char *json, int &i, int len) {
    while (i < len) {
        switch (json[i]) {
            case ' ':
            case '\n':
            case '\t':
                break;
                
            default:
                return;
        }
        i++;
    }
}
