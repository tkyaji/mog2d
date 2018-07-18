#ifndef DataStore_h
#define DataStore_h

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <mutex>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "mog/Constants.h"
#include "mog/core/FileUtils.h"
#include "mog/core/Data.h"
#include "mog/libs/sha256.h"

using namespace std;

namespace mog {
    
    class DataStore {
    public:
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> getData(string key, const std::shared_ptr<T> &defaultValue) {
            std::lock_guard<std::mutex> lock(mtx);
            
            if (!_hasKey(key)) {
                return defaultValue;
            }
            
            auto data = DataStore::caches[key];
            if (data) {
                return static_pointer_cast<T>(data);
            } else {
                return _deserialize<T>(key);
            }
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> getData(string key) {
            auto defaultValue = std::shared_ptr<T>(new T());
            return getData<T>(key, defaultValue);
        }
        
        static void setData(string key, const std::shared_ptr<Data> &value, bool immediatelySave = false);
        static void serialize(string filepath, const std::shared_ptr<Data> &data);
        static void serialize(unsigned char **byteData, int *len, const std::shared_ptr<Data> &data);

        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> deserialize(string filepath) {
            auto data = std::shared_ptr<T>(new T());
            ifstream fin;
            fin.exceptions(ios::failbit|ios::badbit);
            fin.open(filepath, ios::in|ios::binary);
            data->read(fin);
            fin.close();
            return data;
        }
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> deserialize(const std::shared_ptr<ByteArray> &bytes) {
            auto data = std::shared_ptr<T>(new T());
            unsigned char *byteData = nullptr;
            unsigned int len = 0;
            bytes->getValue(&byteData, &len);
            auto str = std::string(byteData, byteData + len);
            istringstream sin(str, ios::binary);
            sin.exceptions(ios::failbit|ios::badbit);
            data->read(sin);
            return data;
        }
        
        static string getStoreDirectory() {
            return mog::FileUtils::getDocumentsDirectory() + "/data_store/";
        }
        static string getStoreFilePath(string key) {
            return getStoreDirectory() + sha256(key);
        }
        
        static bool hasKey(string key);
        static void remove(string key);
        static void removeAll();
        static void save();
        static void save(string key);
        static void clearCache();
        
    private:
        static unordered_map<string, std::shared_ptr<Data>> caches;
        static unordered_map<string, bool> unsaved;
        static mutex mtx;
        
        static bool _hasKey(string key);
        static void _remove(string key);
        static void _removeAll();
        static void _save();
        static void _save(string key);
        
        static void _serialize(string key, const std::shared_ptr<Data> &data);
        
        template <class T, typename enable_if<is_base_of<Data, T>::value>::type*& = enabler>
        static std::shared_ptr<T> _deserialize(string key) {
            std::shared_ptr<T> d = nullptr;
            string file = getStoreFilePath(key);
            struct stat st;
            if (stat(file.c_str(), &st) != 0) return d;
            
            d = deserialize<T>(file);
            DataStore::caches[key] = d;
            return d;
        }
        
        
    };
}

#endif /* DataStore_h */

